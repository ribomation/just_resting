#include <iostream>
#include <sstream>
#include <optional>
#include <random>
#include <unordered_set>
#include <unordered_map>

#include "just_resting.hxx"
#include "json.hpp"

#include "books.hxx"
#include "log-filter.hxx"
#include "auth-filter.hxx"
#include "json-filter.hxx"

using namespace std;
using namespace std::literals;
using namespace just_resting;
using nlohmann::json;

using UserDB = unordered_map<string, string>;
using TokenStore = unordered_set<string>;

string createToken();
bool isValid(string username, string password, UserDB& users);
void authRoutes(Application& app, UserDB& users, TokenStore& authTokens);
void bookRoutes(Application& app);


int main(int argc, char** argv) {
    auto users = UserDB{
            {"nisse"s,  "hult"s},
            {"anna"s,   "conda"s},
            {"justin"s, "time"s},
    };

    auto authTokens = TokenStore{};
    auto authFilter = AuthFilter{authTokens};
    auto logFilter  = LogFilter{};
    auto jsonFilter = JsonFilter{};
    auto app        = Application{};

    app.port(4200, true);
    app.filter(&logFilter);
    app.filter(&authFilter);
    app.filter(&jsonFilter);
    app.assets("./assets"s);
    app.debug(true);

    authRoutes(app, users, authTokens);
    bookRoutes(app);
    books::populate();

    cout << app << endl;
    app.launch([](auto port) {
        cout << "The books server is up and running. http://localhost:" << port << endl;
    });

    return 0;
}

void authRoutes(Application& app, UserDB& users, TokenStore& authTokens) {
    app.route("POST"s, "/auth/login"s, [&](Request& req, Response& res) {
        json data = json::parse(req.body());
        cout << "JSON: " << data.dump(2) << endl;

        if (data.count("username"s) || data.count("password"s)) {
            auto username = data.at("username").get<string>();
            auto password = data.at("password").get<string>();
            if (isValid(username, password, users)) {
                auto token = createToken();
                authTokens.insert(token);
                json reply = {
                        {"token",    token},
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
}

void bookRoutes(Application& app) {
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
}

bool isValid(string username, string password, UserDB& users) {
    try {
        auto its_password = users.at(username);
        return its_password == password;
    } catch (...) {
        return false;
    }
}

string createToken() {
    auto r   = random_device{};
    auto d   = uniform_int_distribution<long>{999'999, 9'999'999};
    auto buf = ostringstream{};
    buf << "qwerty" << d(r);
    return buf.str();
}
