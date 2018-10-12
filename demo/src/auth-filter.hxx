#pragma once

#include <unordered_set>
#include <string>
#include <string_view>

#include "request.hxx"
#include "response.hxx"
#include "filter.hxx"

using namespace justresting;

struct AuthFilter : Filter {
    const unordered_set<string_view> authMethods = {
            "POST"sv, "PUT"sv, "DELETE"sv
    };
    const unordered_set<string>& tokens;

    AuthFilter(unordered_set<string>& tokens) : tokens{tokens} {}

    void invoke(Request& req, Response& res, Route& route) override {
        if (!isAuthenticationRequired(req) || isAuthenticated(req)) {
            next()->invoke(req, res, route);
        } else {
            res.status(401, "must login"s);
        }
    }

    bool isAuthenticationRequired(Request& req) {
        if (req.path() == "/auth/login"sv) return false;
        if (authMethods.count(req.method()) == 0) return false;
        return true;
    }

    bool isAuthenticated(Request& req) {
        if (auto authToken = req.header("X-Auth-Token"sv); authToken.has_value()) {
            string token{authToken.value()};
            if (tokens.count(token) > 0) return true;
        }
        return false;
    }

};
