#include <iostream>
#include <iomanip>
#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <tuple>
#include <memory>

#include <cstring>
#include <cctype>
#include <cstdio>
#include <cerrno>
#include <ctime>

#include <unistd.h>
#include <sys/socket.h>
#include <sys/time.h>

#include "options.hxx"
#include "request-impl.hxx"
#include "response-impl.hxx"
#include "http-handler.hxx"


namespace justresting {
    using namespace std;
    using namespace std::literals;


    void HttpHandler::run(int fromClientFd, int toClientFd, string clientIP) {
        RequestImpl  request;
        ResponseImpl response;
        request.headers_["Remote-IP"] = clientIP;
        receiveRequest(fromClientFd, request, response, options.max_request_size_in_bytes);
        sendResponse(toClientFd, response);
    }


    void HttpHandler::receiveRequest(int fromClientFd, RequestImpl& request, ResponseImpl& response,
                                     unsigned long payloadSize) {
        auto payload = make_unique<char[]>(payloadSize);
        try {
            loadAndParseRequest(fromClientFd, request, payload.get(), payloadSize);
            auto route = getRoute(request, routes);
            if (route.has_value()) {
                filters[0]->invoke(request, response, route.value());
            } else {
                string uri = string{request.method()} + " "s + string{request.path()};
                cerr << "*** Route not found: " << uri << endl;
                response.status(404, uri);
            }
        } catch (ClientClosedConnection& err) {
            response.close();
        } catch (Timeout& err) {
            response.close();
        } catch (MalformedRequest& err) {
            cerr << "*** Malformed REQ " << err.what() << endl;
            response.status(400, err.what());
        } catch (ReadError& err) {
            cerr << "*** Read error " << err.what() << endl;
            response.status(400, err.what());
        } catch (runtime_error& err) {
            cerr << "*** Unknown error " << err.what() << endl;
            response.status(400, err.what());
        }
    }


    void HttpHandler::loadAndParseRequest(int clientFd, RequestImpl& req, char* storage, unsigned long storageSize) {
        struct timeval timeout{};
        timeout.tv_sec  = options.max_request_waiting_time_in_seconds;
        timeout.tv_usec = 0;
        setsockopt(clientFd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

        const auto CRNL            = "\r\n"sv;
        auto       chunkStart      = storage;
        auto       chunkSize       = storageSize;
        auto       parseStartIndex = 0UL;
        auto       payloadSize     = 0UL;
        bool       gotAllHeaders   = false;

        do {
            auto rc = read(clientFd, chunkStart, chunkSize);
            if (options.debug) dumpChunk(chunkStart, rc);
            if (rc == 0) {
                throw ClientClosedConnection{};
            }
            if (rc == -1) {
                if (errno == EINTR) continue;
                if (errno == EAGAIN || errno == EWOULDBLOCK) throw Timeout{static_cast<unsigned>(timeout.tv_sec)};
                throw ReadError{};
            }
            chunkStart += rc;
            chunkSize -= rc;
            payloadSize += rc;

            string_view payload{storage, payloadSize};

            if (req.method().empty()) {
                if (auto pos = payload.find(CRNL, parseStartIndex); pos != string_view::npos) {
                    auto[method, path, protocol] = parseRequestLine(payload.substr(parseStartIndex, pos));
                    parseStartIndex = pos + 2;
                    req.method_   = method;
                    req.path_     = path;
                    req.protocol_ = protocol;
                    if (auto question = path.find("?"sv); question != string_view::npos) {
                        req.path_  = path.substr(0, question);
                        req.query_ = path.substr(question + 1);
                    }
                }
            }

            if (!gotAllHeaders) {
                for (auto start = parseStartIndex, end = start;
                     (end = payload.find(CRNL, start)) != string_view::npos; start = end + 2) {
                    parseStartIndex = end + 2;
                    auto headerLine = payload.substr(start, end - start);
                    if (headerLine.empty()) {
                        gotAllHeaders = true;
                        break;
                    }
                    auto[name, value] = parseHeaderLine(headerLine);
                    req.headers_[name] = value;
                }
                if (!hasRequestBody(req)) return;
            }

            if (auto contentLength = req.header("Content-Length"sv); contentLength.has_value() && req.body().empty()) {
                auto bodySize              = stoul(string{contentLength.value()});
                auto numBytesLeftInStorage = payload.size() - parseStartIndex;
                if (numBytesLeftInStorage >= bodySize || chunkSize == 0) {
                    req.body_ = payload.substr(parseStartIndex, numBytesLeftInStorage);
                    return;
                }
            }
        } while (chunkSize > 0);
    }


    auto HttpHandler::parseRequestLine(string_view line) -> tuple<string_view, string_view, string_view> {
        //GET<SP1>/file<SP2>HTTP/1.0
        const auto SPACE = " "sv;

        const auto space1 = line.find(SPACE, 0);
        if (space1 == string_view::npos) throw MalformedRequest{"Method"s, line};

        const auto space2 = line.find(SPACE, space1 + 1);
        if (space2 == string_view::npos) throw MalformedRequest{"Path", line};

        return make_tuple(
                line.substr(0, space1),
                line.substr(space1 + 1, space2 - space1 - 1),
                line.substr(space2 + 1));
    }


    auto HttpHandler::parseHeaderLine(string_view headerLine) -> tuple<string_view, string_view> {
        const auto COLON       = ":"sv;
        const auto COLON_SPACE = ": "sv;
        auto       colon       = headerLine.find(COLON);
        if (colon == string_view::npos) throw MalformedRequest{"Header"s, headerLine};

        auto name  = headerLine.substr(0, colon);
        auto value = headerLine.substr(headerLine.find_first_not_of(COLON_SPACE, colon));
        return make_tuple(name, value);
    }


    bool HttpHandler::hasRequestBody(Request& req) {
        return (req.method() == "POST"sv) || (req.method() == "PUT"sv);
    }


    auto HttpHandler::getRoute(RequestImpl& req, vector<Route>& routes) -> optional<Route> {
        string_view method = req.method();
        string_view path   = req.path();

        for (auto& r : routes) {
            if (method == r.method) {
                if (r.path == path) {
                    return {r};
                }
                if (path.back() == '/' && r.path == (string{path} + "index.html"s)) {
                    return {r};
                }
                if (auto at = r.path.find("@"s); at != string::npos) {
                    if (r.path.compare(0, at, path, 0, at) == 0) {
                        req.param_ = path.substr(at);
                        return {r};
                    }
                }
            }
        }

        return {};
    }


    void HttpHandler::sendResponse(int toClientFd, const ResponseImpl& response) {
        if (response.isClosed()) return;

        const char* CRNL = "\r\n";

        dprintf(toClientFd, "HTTP/1.1 %d %s%s", response.statusCode(), response.statusText().c_str(), CRNL);
        for (auto& name : response.headers()) {
            dprintf(toClientFd, "%s: %s%s", name.c_str(), response.header(name).value().c_str(), CRNL);
        }

        auto      now = time(nullptr);
        struct tm tm{};
        gmtime_r(&now, &tm);
        char dateBuffer[128];
        strftime(dateBuffer, sizeof(dateBuffer), "%a, %d %b %Y %H:%M:%S %Z", &tm);

        dprintf(toClientFd, "%s: %s%s", "Server", "JustRESTing/0.1", CRNL);
        dprintf(toClientFd, "%s: %s%s", "Date", dateBuffer, CRNL);
        dprintf(toClientFd, "%s: %s%s", "Content-Type", response.contentType().c_str(), CRNL);
        dprintf(toClientFd, "%s: %ld%s", "Content-Length", response.contentLength(), CRNL);

        dprintf(toClientFd, "%s", CRNL);
        if (!response.body().empty()) {
            write(toClientFd, response.bodyData(), response.contentLength());
        }
    }


    void HttpHandler::dumpChunk(char* chunkStart, ssize_t size) {
        if (size <= 0) return;

        cerr << "\n\n>>> REQ payload-size=" << size << "\n";
        string_view chunk{chunkStart, min<unsigned long>(size, options.debug_max_payload_trace_size)};
        auto        count = 0U;
        for_each(chunk.begin(), chunk.end(), [&](char ch) {
            if (ch == '\r') {
                cerr << "<CR>";
                count += 4;
            } else if (ch == '\n') {
                cerr << "<NL>\n";
                count = 0;
            } else if (isprint(ch)) {
                cerr << ch;
                count += 1;
            } else {
                fprintf(stderr, "<%.2X>", static_cast<unsigned char>(ch));
                count += 4;
            }
            if (count > 60) {
                count = 0;
                cerr << "\n";
            }
        });
        cerr << "**END**\n\n";
    }

}

