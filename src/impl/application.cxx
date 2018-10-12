#include <vector>
#include <stdexcept>
#include <algorithm>
#include <filesystem>

#include <cerrno>
#include <cstring>
#include <cstdio>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "application.hxx"
#include "filter.hxx"
#include "request-impl.hxx"
#include "response-impl.hxx"
#include "http-handler.hxx"
#include "engine.hxx"
#include "options.hxx"
#include "asset-handler.hxx"

namespace justresting {
    const string NAME    = "JustRESTing"s;
    const string VERSION = "0.1"s;
    const string LOGO    = R"(
     _           _   ____  _____ ____ _____ _
    | |_   _ ___| |_|  _ \| ____/ ___|_   _(_)_ __   __ _
 _  | | | | / __| __| |_) |  _| \___ \ | | | | '_ \ / _` |
| |_| | |_| \__ \ |_|  _ <| |___ ___) || | | | | | | (_| |
 \___/ \__,_|___/\__|_| \_\_____|____/ |_| |_|_| |_|\__, |
                                                    |___/
)";


    struct RouteInvoker : public Filter {
        void invoke(Request& req, Response& res, Route& route) override {
            route.handler(req, res);
        }
    };


    class Application::AppImpl {
        Options         options;
        vector<Route>   routes{};
        vector<Filter*> filters{};
        friend Engine;

    public:
        void set_port(unsigned short port) {
            options.port = port;
        }

        void set_debug(bool enable) {
            options.debug = enable;
        }

        void set_options(Options options) {
            this->options = options;
        }

        void add_route(string method, string path, RouteHandler handler) {
            routes.emplace_back(method, path, handler);
        }

        void assets_dir(fs::path dir) {
            for (auto& asset : fs::recursive_directory_iterator(dir)) {
                if (asset.is_regular_file()) {
                    AssetHandler handler{asset.path(), dir};
                    routes.emplace_back("GET"s, handler.uri(), handler);
                }
            }
        }

        void add_filter(Filter* filter) {
            filters.push_back(filter);
        }

        void launch() {
            Engine       engine{options, routes, filters};
            RouteInvoker routeInvoker;
            engine.setup(&routeInvoker);
            engine.launch();
        }

        string name() const { return NAME; }

        string version() const { return VERSION; }

        void dump_routes(ostream& out) const {
            for_each(routes.begin(), routes.end(), [&](auto r) {
                out << r.toString() << endl;
            });
        }

        void print(ostream& os) const {
            os << LOGO
               << name() << "/" << version()
               << "  [(C) Ribomation AB, 2018.]"
               << endl;
            os << "HTTP port: " << options.port << endl;
            dump_routes(os);
        }
    };


    Application::Application() : impl{make_unique<AppImpl>()} {}

    Application::~Application() = default;


    Application& Application::port(unsigned short port) {
        impl->set_port(port);
        return *this;
    }

    Application& Application::debug(bool enable) {
        impl->set_debug(enable);
        return *this;
    }

    Application& Application::options(Options options) {
        impl->set_options(options);
        return *this;
    }

    Application& Application::route(string method, string path, RouteHandler handler) {
        impl->add_route(method, path, handler);
        return *this;
    }

    Application& Application::filter(Filter* invoker) {
        impl->add_filter(invoker);
        return *this;
    }

    Application& Application::assets(std::filesystem::path assetsDir) {
        impl->assets_dir(assetsDir);
        return *this;
    }

    void Application::printRoutes(ostream& out) const {
        impl->dump_routes(out);
    }

    ostream& Application::print(ostream& out) const {
        impl->print(out);
        return out;
    }

    void Application::launch() {
        impl->launch();
    }

    string Application::name() const {
        return impl->name();
    }

    string Application::version() const {
        return impl->version();
    }


}
