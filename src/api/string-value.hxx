#pragma once

#include <string>
#include <string_view>
#include <stdexcept>

namespace justresting {
    using namespace std;
    using namespace std::literals;

    struct StringValue {
        string value;

        StringValue(string s) : value{s} {}
        StringValue(string_view s) : value{s} {}

        operator string() const { return value; }
        operator int()    const { return stoi(value); }
        operator float()  const { return stof(value); }
        operator bool()   const {
            if (value == "true"s || value == "false"s) return (value == "true"s);
            throw domain_error{"not supported: "s + value};
        }
    };

}
