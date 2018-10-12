#include <iostream>
#include <sstream>
#include <iomanip>
#include <optional>
#include <unordered_set>
#include <unordered_map>

#include "application.hxx"
#include "request.hxx"
#include "response.hxx"
#include "filter.hxx"
#include "route.hxx"
#include "json/json.hpp"

#include "samples/books-app/books.hxx"
#include "samples/books-app/log-filter.hxx"
#include "samples/books-app/auth-filter.hxx"

using namespace std;
using namespace std::literals;
using namespace justresting;
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

string createToken() {
    return "qwertyuiop"s;
}


int main(int argc, char** argv) {
    unordered_set<string>         authTokens;
    unordered_map<string, string> users      = {
            {"nisse"s, "hult"s}
    };

    AuthFilter  authFilter{authTokens};
    LogFilter   logFilter;
    JsonFilter  jsonFilter;
    Application app;

    app.filter(&logFilter);
    app.filter(&authFilter);
    app.filter(&jsonFilter);
    app.assets("../src/samples/books-app/assets"s);
    app.debug(true);


    app.route("POST"s, "/auth/login"s, [&](Request& req, Response& res) {
        json data = json::parse(req.body());
        cout << "JSON: " << data.dump(2) << endl;

        if (data.count("username"s) || data.count("password"s)) {
            auto username = data.at("username").get<string>();
            auto password = data.at("password").get<string>();
            if (users.count(username) > 0 && users[username] == password) {
                auto token = createToken();
                authTokens.insert(token);
                json reply = {
                        {"token", token},
                        {"username", username},
                };
                res.body(reply.dump(1));
                res.type("application/json"s);
            } else {
                res.status(401, "mismatch of username/password"s);
            }
        } else {
            res.status(401, "missing username/password"s);
        }
    });

    app.route("POST"s, "/auth/logout"s, [&](Request& req, Response& res) {
        json data = json::parse(req.body());
        cout << "JSON: " << data.dump(2) << endl;
        if (data.count("token"s)) {
            auto token = data.at("token").get<string>();
            authTokens.erase(token);
            res.status(204, "logged out"s);
        } else {
            res.status(400, "no token"s);
        }
    });

    app.route("GET"s, "/books"s, [&](Request& req, Response& res) {
        auto bookList = books::all();
        res.attribute("books"s, bookList);
    });

    app.route("POST"s, "/books"s, [&](Request& req, Response& res) {
        json data = json::parse(req.body());
        cout << "JSON: " << data.dump(2) << endl;

        books::Book book;
        books::from_json(data, book);
        auto id = insert(book);

        book = books::findById(id).value();
        res.status(201, "id="s + to_string(id));
        res.attribute("book"s, book);
    });

    app.route("GET"s, "/book/@"s, [&](Request& req, Response& res) {
        int      id    = req.param();
        if (auto maybe = books::findById(id); maybe.has_value()) {
            books::Book book = maybe.value();
            res.attribute("book"s, book);
        } else {
            res.status(404, "id="s + to_string(id));
        }
    });

    app.route("DELETE"s, "/book/@"s, [&](Request& req, Response& res) {
        int id = req.param();
        books::remove(id);
        res.status(204, "id="s + to_string(id));
    });

    app.route("PUT"s, "/book/@"s, [&](Request& req, Response& res) {
        int      id    = req.param();
        if (auto maybe = books::findById(id); maybe.has_value()) {
            books::Book book = maybe.value();

            json data = json::parse(req.body());
            cout << "JSON: " << data.dump(2) << endl;
            books::Book updatedBook{};
            books::from_json(data, updatedBook);
            cout << "UPDT: " << updatedBook << endl;

            if (!updatedBook.title.empty()) {
                book.title = updatedBook.title;
            }
            if (!updatedBook.author.empty()) {
                book.author = updatedBook.author;
            }
            if (updatedBook.price > 0) {
                book.price = updatedBook.price;
            }
            if (updatedBook.pages > 0) {
                book.pages = updatedBook.pages;
            }

            cout << "BOOK: " << book << endl;
            books::update(book);

            res.attribute("book"s, book);
        } else {
            res.status(404, "id="s + to_string(id));
        }
    });

    books::populate();
    cout << app << endl;
    app.launch();
    return 0;
}
