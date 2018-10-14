#include <stdexcept>
#include <vector>
#include <optional>
#include "catch.hpp"
#include "http-handler.hxx"
#include "route.hxx"

using namespace std;
using namespace std::literals;
using namespace justresting;

SCENARIO("request method body", "[request]") {
    GIVEN("a request object") {
        RequestImpl req;

        WHEN("checking if a default request has a body ") {
            THEN("it should be false") {
                req.method_ = "GET"sv;
                CHECK(req.method() == "GET"sv);
                CHECK_FALSE(HttpHandler::hasRequestBody(req));
            }
        }

        WHEN("checking if a DELETE request has a body ") {
            THEN("it should be false") {
                req.method_ = "DELETE"sv;
                CHECK_FALSE(HttpHandler::hasRequestBody(req));
            }
        }

        WHEN("checking if a POST request has a body ") {
            THEN("it should be false") {
                req.method_ = "POST"sv;
                CHECK(HttpHandler::hasRequestBody(req));
            }
        }

        WHEN("checking if a PUT request has a body ") {
            THEN("it should be false") {
                req.method_ = "PUT"sv;
                CHECK(HttpHandler::hasRequestBody(req));
            }
        }
    }
}


SCENARIO("request 1st line", "[request, parsing]") {
    GIVEN("some 1st line:ers") {
        WHEN("parsing a simple GET request") {
            auto[method, path, proto] = HttpHandler::parseRequestLine("GET /message HTTP/1.0"sv);
            THEN("method should be GET") {
                CHECK(method == "GET"sv);
                CHECK(path == "/message"sv);
                CHECK(proto == "HTTP/1.0"sv);
            }
        }

        WHEN("parsing POST request") {
            auto[method, path, _] = HttpHandler::parseRequestLine("POST /auth/login HTTP/1.0"sv);
            THEN("method should be POST") {
                CHECK(method == "POST"sv);
                CHECK(path == "/auth/login"sv);
            }
        }

        WHEN("parsing request with a param") {
            auto[method, path, _] = HttpHandler::parseRequestLine("PUT /api/books/42 HTTP/1.0"sv);
            THEN("method should be PUT") {
                CHECK(method == "PUT"sv);
                CHECK(path == "/api/books/42"sv);
            }
        }

        WHEN("parsing a malformed request") {
            THEN("is should throw an exception of type malformed") {
                CHECK_THROWS_AS(HttpHandler::parseRequestLine("PUT /api/books/42"sv), MalformedRequest);
                CHECK_THROWS_AS(HttpHandler::parseRequestLine("PUT HTTP"sv), MalformedRequest);
                CHECK_THROWS_AS(HttpHandler::parseRequestLine(""sv), MalformedRequest);
            }
        }

        WHEN("parsing a malformed request") {
            THEN("is the exception should contain a message") {
                CHECK_THROWS_WITH(HttpHandler::parseRequestLine("PUT /api/books/42"sv), Catch::Contains("Path"));
                CHECK_THROWS_WITH(HttpHandler::parseRequestLine("PUT"sv), Catch::Contains("Method"));
            }
        }
    }
}


SCENARIO("Request headers", "[request, parsing]") {
    GIVEN("some REQ headers") {
        WHEN("parsing a simple header") {
            auto[name, value] = HttpHandler::parseHeaderLine("Accept: text/plain"sv);
            THEN("it should succeed") {
                CHECK(name == "Accept"sv);;
                CHECK(value == "text/plain"sv);;
            }
        }

        WHEN("parsing a header") {
            auto[name, value] = HttpHandler::parseHeaderLine("Content-Type:       application/json"sv);
            THEN("it should succeed") {
                CHECK(name == "Content-Type"sv);;
                CHECK(value == "application/json"sv);;
            }
        }

        WHEN("parsing a malformed header") {
            THEN("it should throw an exception") {
                CHECK_THROWS_AS(HttpHandler::parseHeaderLine("what-ever"sv), MalformedRequest);
                CHECK_THROWS_WITH(HttpHandler::parseHeaderLine("what-ever"sv), Catch::Contains("ever"));
                CHECK_THROWS_WITH(HttpHandler::parseHeaderLine("name=value"sv), Catch::Contains("name"));
            }
        }
    }
}


SCENARIO("Finding a route", "[request, lookup]") {
    GIVEN("some simple routes") {
        RouteHandler  emptyRoute = [](Request& req, Response& res) {};
        vector<Route> routes     = {
                {"GET"s,  "/"s,                emptyRoute},
                {"GET"s,  "/api/books"s,       emptyRoute},
                {"POST"s, "/api/books"s,       emptyRoute},
                {"GET"s,  "/docs/index.html"s, emptyRoute},
                {"PUT"s,  "/api/books/@"s,     emptyRoute},
        };
        RequestImpl   request;

        WHEN("looking for the first of them") {
            request.method_ = "GET"sv;
            request.path_   = "/"sv;
            auto r = HttpHandler::getRoute(request, routes);
            THEN("it should be found") {
                CHECK(r.has_value());
                CHECK(r.value().method == "GET"s);
                CHECK(r.value().path == "/"s);
            }
        }

        WHEN("looking for one of them") {
            request.method_ = "POST"sv;
            request.path_   = "/api/books"sv;
            auto r = HttpHandler::getRoute(request, routes);
            THEN("it should be found") {
                CHECK(r.has_value());
                CHECK(r.value().method == "POST"s);
                CHECK(r.value().path == "/api/books"s);
            }
        }

        WHEN("looking for a non-existent route") {
            request.method_ = "POST"sv;
            request.path_   = "/what/ever"sv;
            auto r = HttpHandler::getRoute(request, routes);
            THEN("it should not be found") {
                CHECK_FALSE(r.has_value());
            }
        }

        WHEN("looking for route with trailing /") {
            request.method_ = "GET"sv;
            request.path_   = "/docs/"sv;
            auto r = HttpHandler::getRoute(request, routes);
            THEN("it should be found as index.html") {
                CHECK(r.has_value());
                CHECK(r.value().method == "GET"s);
                CHECK(r.value().path == "/docs/index.html"s);
            }
        }

        WHEN("looking for a route with a param") {
            request.method_ = "PUT"sv;
            request.path_   = "/api/books/42"sv;
            auto r = HttpHandler::getRoute(request, routes);
            THEN("it should be found") {
                CHECK(r.has_value());
                CHECK(r.value().method == "PUT"s);
                CHECK(r.value().path == "/api/books/@"s);
                CHECK(request.param().operator int() == 42);
            }
        }
    }
}



//        Options         emptyOpts{};
//        vector<Route>   emptyRoutes{};
//        vector<Filter*> emptyFilters{};
//        HttpHandler     handler{emptyOpts, emptyRoutes, emptyFilters};
