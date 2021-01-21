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

SCENARIO("fetching assets", "[asset]") {
    GIVEN("an assets dir") {
        fs::path     assetsDir{"./data"s};
        RequestImpl  req;
        ResponseImpl res;

        WHEN("handling a plain text file") {
            AssetHandler assetHandler{assetsDir / "sample.txt"s, assetsDir};
            assetHandler(req, res);

            THEN("the body should contain the file") {
                REQUIRE(res.body().size() == 86UL);
                REQUIRE(res.contentLength() == 86UL);
                REQUIRE(res.contentType() == "text/plain"s);
            }
        }

        WHEN("handling a HTML file") {
            AssetHandler assetHandler{assetsDir / "index.html"s, assetsDir};
            assetHandler(req, res);

            THEN("the body should contain the file") {
                REQUIRE(res.contentLength() == 181UL);
                REQUIRE(res.contentType() == "text/html"s);
            }
        }

        WHEN("handling a CSS file in a subdir") {
            AssetHandler assetHandler{assetsDir / "css/style.css"s, assetsDir};
            assetHandler(req, res);

            THEN("the body should contain the file") {
                REQUIRE(res.contentLength() == 65UL);
                REQUIRE(res.contentType() == "text/css"s);
            }
        }

        WHEN("handling an image file") {
            AssetHandler assetHandler{assetsDir / "blue-parrot-large.png"s, assetsDir};
            assetHandler(req, res);
            THEN("the body should be the image") {
                REQUIRE(res.contentLength() == 159904UL);
                REQUIRE(res.contentType() == "image/png"s);
            }
        }

    }
}

SCENARIO("configuration", "[asset]") {
    GIVEN("an assets handler") {
        WHEN("checking the mime type") {
            THEN("it should work") {
                REQUIRE(AssetHandler::mimeType("whatever.png"s) == "image/png"s);
                REQUIRE(AssetHandler::mimeType("whatever.css"s) == "text/css"s);
                REQUIRE(AssetHandler::mimeType("whatever.js"s) == "application/javascript"s);
                REQUIRE(AssetHandler::mimeType("whatever.xyz"s) == "application/octet-stream"s);
            }
        }

        WHEN("adding a mime type") {
            AssetHandler::addMimeType(".xyz"s, "application/foobar"s);
            THEN("it should find the new type") {
                REQUIRE(AssetHandler::mimeType("whatever.xyz"s) == "application/foobar"s);
                REQUIRE(AssetHandler::mimeType("whatever.zzz"s) == "application/octet-stream"s);
            }
        }
    }
}

