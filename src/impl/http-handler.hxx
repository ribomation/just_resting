#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <tuple>
#include "filter.hxx"
#include "route.hxx"
#include "request-impl.hxx"
#include "response-impl.hxx"
#include "options.hxx"

namespace justresting {

    struct HttpHandler {
        Options        & options;
        vector <Route> & routes;
        vector<Filter*>& filters;

        HttpHandler(Options& options, vector <Route>& routes, vector<Filter*>& filters)
                : options{options}, routes{routes}, filters{filters} {}

        void run(int fromClientFd, int toClientFd, string ip);
        void loadAndParseRequest(int fromClientFd, RequestImpl& req, char* storage, unsigned long storageSize);
        void receiveRequest(int fromClientFd, RequestImpl& request, ResponseImpl& response, unsigned long payloadSize);
        void sendResponse(int toClientFd, const ResponseImpl& response);
        Route* findRoute(RequestImpl& req);

        tuple<string_view, string_view> parseHeaderLine(string_view headerLine);
        tuple<string_view, string_view, string_view> parseRequestLine(string_view line);
        bool hasRequestBody(Request& req);
        void dumpChunk(char* chunkStart, ssize_t size);
    };


}

