#pragma once

#include <iostream>
#include <string>
#include <functional>
#include <memory>
#include <filesystem>

#include "request.hxx"
#include "response.hxx"
#include "route.hxx"
#include "filter.hxx"
#include "interface.hxx"
#include "options.hxx"

namespace justresting {
    using namespace std;
    namespace fs = std::filesystem;

    class Application : NotCopyable {
        class AppImpl;

        unique_ptr<AppImpl> impl;

    public:
        Application& route(string method, string path, RouteHandler handler);
        Application& filter(Filter* filter);
        Application& assets(fs::path assetsDir);
        Application& port(unsigned short port);
        Application& debug(bool enable);
        Application& options(Options options);
        void launch();

        Application();
        ~Application();

        string name() const;
        string version() const;
        void printRoutes(ostream& out) const;
        ostream& print(ostream& out) const;
    };

    inline ostream& operator<<(ostream& os, const Application& app) {
        return app.print(os);
    }

}

