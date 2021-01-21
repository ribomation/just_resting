#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <tuple>
#include <stdexcept>
#include <cstring>

#include "filter.hxx"
#include "route.hxx"
#include "request-impl.hxx"
#include "response-impl.hxx"
#include "options.hxx"

namespace just_resting {
    struct ClientClosedConnection : runtime_error {
        ClientClosedConnection() : runtime_error{"Connection closed by client: "s + strerror(errno)} {}
    };

    struct Timeout : runtime_error {
        Timeout(unsigned numSeconds) : runtime_error{"Timeout after "s + to_string(numSeconds) + " seconds"s} {}
    };

    struct ReadError : runtime_error {
        ReadError() : runtime_error{"Socket RECV failed: "s + strerror(errno)} {}
    };

    struct MalformedRequest : runtime_error {
        MalformedRequest(string missing, string_view line)
                : runtime_error{"Missing "s + missing + " in REQ ["s + string(line) + "]"s} {}
    };

    struct RouteNotFound : runtime_error {
        RouteNotFound(string_view method, string_view uri) : runtime_error{
                "\""s + string(method) + " "s + string(uri) + "\""s} {}
    };

    struct HttpHandler {
        Options        & options;
        vector <Route> & routes;
        vector<Filter*>& filters;

        ~HttpHandler() = default;
        HttpHandler() = delete;
        HttpHandler(const HttpHandler&) = delete;
        HttpHandler& operator =(const HttpHandler&) = delete;

        HttpHandler(Options& options, vector <Route>& routes, vector<Filter*>& filters)
                : options{options}, routes{routes}, filters{filters} {}

        void run(int fromClientFd, int toClientFd, string ip);
        void loadAndParseRequest(int fromClientFd, RequestImpl& req, char* storage, unsigned long storageSize);
        void receiveRequest(int fromClientFd, RequestImpl& request, ResponseImpl& response, unsigned long payloadSize);
        void dumpChunk(char* chunkStart, ssize_t size);
        static auto parseRequestLine(string_view line) -> tuple<string_view, string_view, string_view>;

        static auto parseHeaderLine(string_view headerLine) -> tuple<string_view, string_view>;
        static auto hasRequestBody(Request& req) -> bool;
        static auto getRoute(RequestImpl& req, vector <Route>& routes) -> optional<Route>;
        static void sendResponse(int toClientFd, const ResponseImpl& response);
    };


}

