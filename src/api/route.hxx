#pragma once

#include <string>
#include <functional>
#include "request.hxx"
#include "response.hxx"


namespace justresting {
    using namespace std;
    using namespace std::literals;

    struct Request;
    struct Response;
    using RouteHandler = function<void(Request&, Response&)>;

    struct Route final {
        const string       method;
        const string       path;
        const RouteHandler handler;

        Route(string method, string path, RouteHandler handler)
                : method{method}, path{path}, handler{handler} {}

        ~Route() = default;
        Route(const Route&) = default;
        Route& operator=(const Route&) = default;

        string toString() const {
            return method + " "s + path;
        }
    };

}

