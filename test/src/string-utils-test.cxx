#include <string>
#include "catch.hpp"
#include "string-utils.hxx"

using namespace just_resting::utils;
using namespace std;
using namespace std::literals;

TEST_CASE("converting to lower-case should work", "[conversion]") {
    REQUIRE(lc(" Foo Bar! "s) == " foo bar! "s);
}

TEST_CASE("converting to upper-case should work", "[conversion]") {
    REQUIRE(uc(" Foo Bar! "s) == " FOO BAR! "s);
}

TEST_CASE("converting to empty string should work", "[conversion]") {
    REQUIRE(uc(""s) == ""s);
    REQUIRE(uc("  "s) == "  "s);
}

TEST_CASE("trimming leading and trailing spaces should work", "[trim]") {
    REQUIRE(trim("   foo  "s) == "foo"s);
    REQUIRE(trim(" \t\n\rfoo  "s) == "foo"s);
    REQUIRE(trim("foo\0\0"s) == "foo"s);
}

