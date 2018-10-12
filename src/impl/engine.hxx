#pragma once

#include <vector>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>

#include "application.hxx"
#include "route.hxx"
#include "filter.hxx"
#include "http-handler.hxx"
#include "options.hxx"

namespace justresting {

    class Engine {
        Options        & options;
        vector<Route>  & routes;
        vector<Filter*>& filters;
        bool running = true;

        int createServerSocket();
        void serverLoopBody(int serverSocketFd);

    public:
        Engine(Options& options, vector<Route>& routes, vector<Filter*>& filters)
                : options{options}, routes{routes}, filters{filters} {}

        void setup(Filter* routeInvoker);
        void launch();

        void shutdown() {
            running = false;
        }

        string hostName();
        string domainName();
        string os();
        unsigned maxWaitingSocketConnections(unsigned defaultMaxConnections);
        string addr2ip(const sockaddr_in* addr);
    };


}

