#pragma once

#include <unordered_set>
#include <string>
#include <string_view>

#include "just_resting.hxx"

using namespace std;
using namespace std::literals;
using namespace just_resting;
using nlohmann::json;

struct JsonFilter : Filter {
    void invoke(Request& req, Response& res, Route& route) override {
        next()->invoke(req, res, route);

        if (any maybe = res.attribute("books"s); maybe.has_value()) {
            auto booksList = any_cast<vector<books::Book>>(maybe);
            json jsonList  = json::array();
            for (auto& book : booksList) {
                json jsonBook;
                books::to_json(jsonBook, book);
                jsonList.push_back(jsonBook);
            }
            res.type("application/json"s);
            res.body(jsonList.dump(1));
        }

        if (any maybe = res.attribute("book"s); maybe.has_value()) {
            auto book = any_cast<books::Book>(maybe);
            json jsonBook;
            books::to_json(jsonBook, book);
            res.type("application/json"s);
            res.body(jsonBook.dump(1));
        }

    }
};
