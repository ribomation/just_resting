#include <memory>
#include "catch.hpp"
#include "response-impl.hxx"

using namespace std;
using namespace std::literals;
using namespace justresting;

SCENARIO("response handling", "[response]") {
    GIVEN("a respons object") {
        ResponseImpl res;

        WHEN("checking the defaults") {
            THEN("it should work") {
                REQUIRE(res.statusCode() == 200U);
                REQUIRE(res.statusText() == "OK"s);
                REQUIRE(res.contentType() == "text/plain"s);
                REQUIRE(res.contentLength() == 0UL);
                REQUIRE(res.body().empty());
                REQUIRE(res.headers().empty());
                REQUIRE(res.attributes().empty());
            }
        }

        WHEN("setting the status and type") {
            res.status(201, "Created"s);
            res.type("application/json");
            THEN("it should provide these") {
                REQUIRE(res.statusCode() == 201U);
                REQUIRE(res.statusText() == "Created"s);
                REQUIRE(res.contentType() == "application/json"s);
            }
        }

        WHEN("setting some headers") {
            res.header("X-Powered-By"s, "Ribomation"s);
            res.header("Server"s, "justRESTing, version 0.1"s);
            THEN("it should provide the headers") {
                REQUIRE(res.headers().size() == 2U);
                REQUIRE(res.header("Server"s) == "justRESTing, version 0.1"s);
                REQUIRE(res.header("X-Powered-By"s) == "Ribomation"s);
                REQUIRE(res.header("What-Ever"s).has_value() == false);
            }
        }

        WHEN("setting some attributes") {
            res.attribute("uint", 42U);
            res.attribute("float", 3.1415F);
            res.attribute("string", "Tjolla Hopp"s);
            THEN("is should provide these attributes") {
                REQUIRE(res.attributes().size() == 3U);
                REQUIRE(res.attribute("uint").type() == typeid(unsigned));
                REQUIRE(res.attribute("float").type() == typeid(float));
                REQUIRE(res.attribute("string").type() == typeid(string));

                REQUIRE(any_cast<unsigned>(res.attribute("uint")) == 42U);
                REQUIRE(any_cast<float>(res.attribute("float")) == 3.1415F);
                REQUIRE(any_cast<string>(res.attribute("string")) == "Tjolla Hopp"s);
                REQUIRE(res.attribute("whatever"s).has_value() == false);
            }
        }

        WHEN("setting the body as a string") {
            res.body("tjolla hopp"s);
            THEN("it should provide it") {
                REQUIRE(res.contentLength() == 11UL);
                REQUIRE(res.body().substr(0, 6) == "tjolla"s);
            }
        }

        WHEN("setting the body as smart pointer") {
            const auto SIZE = 1024UL * 1024UL;
            auto payload = unique_ptr<char[]>(new char[SIZE]);
            res.body(move(payload), SIZE);
            THEN("it should provide it") {
                REQUIRE(res.contentLength() == SIZE);
            }
        }

    }
}

