#include <string>
#include <filesystem>
#include "catch.hpp"
#include "asset-handler.hxx"
#include "request-impl.hxx"
#include "response-impl.hxx"

using namespace just_resting;
using namespace std;
using namespace std::literals;
namespace fs = std::filesystem;

SCENARIO("Assets fetching", "[asset]") {
    GIVEN("an assets dir") {
        auto dir = fs::path{"./data"s};
        auto req = RequestImpl{};
        auto res = ResponseImpl{};

        auto asset = [&req, &res, &dir](string file){
            AssetHandler{dir / file, dir}(req, res);
        };

        WHEN("handling a plain text file") {
            asset("sample.txt");
            THEN("the body should contain the file") {
                REQUIRE(res.body().size() == 86UL);
                REQUIRE(res.contentLength() == 86UL);
                REQUIRE(res.contentType() == "text/plain"s);
            }
        }

        WHEN("handling a HTML file") {
            asset("index.html"s);
            THEN("the body should contain the file") {
                REQUIRE(res.contentLength() == 171UL);
                REQUIRE(res.contentType() == "text/html"s);
            }
        }

        WHEN("handling a CSS file in a subdir") {
            asset("css/style.css"s);
            THEN("the body should contain the file") {
                REQUIRE(res.contentLength() == 55UL);
                REQUIRE(res.contentType() == "text/css"s);
            }
        }

        WHEN("handling an image file") {
            asset("blue-parrot-large.png"s);
            THEN("the body should be the image") {
                REQUIRE(res.contentLength() == 159904UL);
                REQUIRE(res.contentType() == "image/png"s);
            }
        }

    }
}

SCENARIO("Assets Configuration", "[asset]") {
    GIVEN("an assets handler") {
        WHEN("checking a standard mime type") {
            THEN("it should return the type") {
                REQUIRE(AssetHandler::mimeType("whatever.png"s) == "image/png"s);
                REQUIRE(AssetHandler::mimeType("whatever.css"s) == "text/css"s);
                REQUIRE(AssetHandler::mimeType("whatever.js"s) == "application/javascript"s);
                REQUIRE(AssetHandler::mimeType("whatever.xyz"s) == "application/octet-stream"s);
            }
        }

        WHEN("adding a mime type") {
            AssetHandler::addMimeType(".xyz"s, "application/foobar"s);
            THEN("it should return the new type") {
                REQUIRE(AssetHandler::mimeType("whatever.xyz"s) == "application/foobar"s);
                REQUIRE(AssetHandler::mimeType("whatever.zzz"s) == "application/octet-stream"s);
            }
        }
    }
}

