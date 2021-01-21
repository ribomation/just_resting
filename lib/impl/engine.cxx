
#include <iostream>
#include <fstream>
#include <sstream>

#include <arpa/inet.h>
#include <unistd.h>
#include <sys/utsname.h>

#include "engine.hxx"
#include "string-utils.hxx"

namespace just_resting {
    using namespace std;

    void Engine::setup(Filter* routeInvoker) {
        auto last = routeInvoker;
        for_each(filters.rbegin(), filters.rend(), [&](Filter* invoker) {
            invoker->next_ = last;
            last = invoker;
        });
    }

    void Engine::launch(function<void(int)> onReady) {
        int serverSocketFd = createServerSocket();
        onReady(options.port);
        do {
            serverLoopBody(serverSocketFd);
        } while (running);
    }

    void Engine::serverLoopBody(int serverSocketFd) {
        sockaddr_in clientAddr{};
        socklen_t   clientAddrSize = sizeof(clientAddr);
        int         clientSocketFd = accept(serverSocketFd, (struct sockaddr*) &clientAddr, &clientAddrSize);
        if (clientSocketFd < 0) {
            if (errno == EINTR) return;
            throw runtime_error("[application] failed to accept incoming connections: "s + strerror(errno));
        }

        string clientIP = addr2ip(&clientAddr);
        if (options.debug) {
            cerr << "client connected: IP=" << clientIP << endl;
        }

        HttpHandler httpHandler{options, routes, filters};
        httpHandler.run(clientSocketFd, clientSocketFd, clientIP);
        close(clientSocketFd);
    }

    bool bindServerPort(int serverSocketFd, int port, int maxQueue, bool retry = false) {
        struct sockaddr_in serverAddr{};
        serverAddr.sin_family      = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port        = htons(port);

        if (auto rc = bind(serverSocketFd, (struct sockaddr*) &serverAddr, sizeof(serverAddr)); rc < 0) {
            if (retry) return false;
            throw runtime_error("[application] failed to bind() socket: "s + strerror(errno));
        }

        if (auto rc = listen(serverSocketFd, maxQueue); rc < 0) {
            if (retry) return false;
            throw runtime_error("[application] failed to listen() socket: "s + strerror(errno));
        }

        return true;
    }

    int Engine::createServerSocket() {
        const int DEFAULT_PROTOCOL = 0;
        int       serverSocketFd   = socket(AF_INET, SOCK_STREAM, DEFAULT_PROTOCOL);
        if (serverSocketFd < 0) {
            throw runtime_error("[application] cannot create server socket fd: "s + strerror(errno));
        }

        auto maxConnections = maxWaitingSocketConnections(5);
        auto portFound = false;
        auto port      = options.port;
        do {
            portFound = bindServerPort(serverSocketFd, port, maxConnections, options.any_port);
            options.port = port;
            ++port;
        } while (!portFound);

        return serverSocketFd;
    }

    string Engine::addr2ip(const sockaddr_in* addr) {
        using just_resting::utils::trim;
        char buf[INET_ADDRSTRLEN];
        memset(buf, '\0', sizeof(buf));
        inet_ntop(AF_INET, &(addr->sin_addr), buf, sizeof(buf));
        string result{buf, sizeof(buf)};
        return trim(result);
    }

    unsigned Engine::maxWaitingSocketConnections(unsigned defaultMaxConnections) {
        ifstream f{"/proc/sys/net/core/somaxconn"s};
        if (!f) return defaultMaxConnections;

        auto maxConnections = 0;
        f >> maxConnections;
        return (maxConnections > 0) ? maxConnections : defaultMaxConnections;
    }

    string Engine::hostName() {
        char buf[256];
        auto rc = gethostname(buf, sizeof(buf));
        if (rc != 0) throw runtime_error{"failed gethostname: "s + strerror(errno)};
        return {buf};
    }

    string Engine::domainName() {
        char buf[256];
        auto rc = getdomainname(buf, sizeof(buf));
        if (rc != 0) throw runtime_error{"failed getdomainname: "s + strerror(errno)};
        return {buf};
    }

    string Engine::os() {
        struct utsname uts{};
        auto           rc = uname(&uts);
        if (rc != 0) throw runtime_error{"failed uname: "s + strerror(errno)};

        ostringstream buf;
        buf << uts.sysname << "/" << uts.release << " (" << uts.machine << ")";
        return buf.str();
    }

}

