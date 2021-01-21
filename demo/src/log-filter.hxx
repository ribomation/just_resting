#pragma once

#include <sstream>
#include <iomanip>
#include <chrono>

#include "just_resting.hxx"

using namespace just_resting;

struct LogFilter : Filter {
    LogFilter() = default;

    string fmtElapsed(long ns) {
        ostringstream buf{};
        if (ns < 1E3) {
            buf << ns << " ns";
        } else if (ns < 1E6) {
            buf << fixed << setprecision(3) << ns * 1E-3 << " us";
        } else if (ns < 1E9) {
            buf << fixed << setprecision(3) << ns * 1E-6 << " ms";
        } else {
            buf << fixed << setprecision(5) << ns * 1E-9 << " s";
        }
        return buf.str();
    }

    void invoke(Request& req, Response& res, Route& route) override {
        using namespace std::chrono;

        cout << "REQ: " << req.method() << " " << req.path() << " " << req.protocol() << endl;

        auto start = chrono::steady_clock::now();
        next()->invoke(req, res, route);
        auto end = chrono::steady_clock::now();
        auto ns  = duration_cast<nanoseconds>(end - start).count();

        cout << "RES: " << res.statusCode() << " " << res.statusText()
             << ", " << res.contentType() << " " << res.contentLength() << " bytes"
             << ", elapsed " << fmtElapsed(ns) << endl;
    }
};


