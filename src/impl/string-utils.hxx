
#pragma once

#include <string>
#include <string_view>
#include <algorithm>
#include <cstring>

namespace justresting {
    namespace utils {
        using namespace std;
        using namespace std::literals;


        inline string lc(string s) {
            transform(s.begin(), s.end(), s.begin(), [](auto ch) {
                return ::tolower(ch);
            });
            return s;
        }

        inline string lc(string_view s) {
            return lc(string{s});
        }

        inline string uc(string s) {
            transform(s.begin(), s.end(), s.begin(), [](auto ch) {
                return ::toupper(ch);
            });
            return s;
        }

        inline string uc(string_view s) {
            return uc(string{s});
        }

        inline string trim(string s) {
            unsigned long first = 0;
            for (char     ch    = s[first]; ::isspace(ch) || ch == '\0'; ch = s[first]) ++first;

            unsigned long last = s.size() - 1;
            for (char     ch   = s[last]; ::isspace(ch) || ch == '\0'; ch = s[last]) --last;

            return s.substr(first, last - first + 1);
        }

    }
}


