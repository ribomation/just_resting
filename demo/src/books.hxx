#pragma once

#include <iostream>
#include <string>
#include <vector>

namespace books {
    using namespace std;
    using namespace std::literals;
    using nlohmann::json;


    struct Book {
        unsigned id     = 0;
        string   title  = ""s;
        string   author = ""s;
        unsigned price  = 0;
        unsigned pages  = 0;
    };

    ostream& operator<<(ostream& os, const Book& book) {
        return os << "Book{#" << book.id << ", " << book.title << ", by " << book.author
                  << ", SEK " << book.price
                  << ", " << book.pages << " pages}";
    }

    void to_json(json& data, const Book& book) {
        data = {
                {"id",     book.id},
                {"title",  book.title},
                {"author", book.author},
                {"price",  book.price},
                {"pages",  book.pages},
        };
    }

    void from_json(const json& data, Book& book) {
        if (data.count("id")) {
            book.id = data.at("id").get<unsigned>();
        }
        if (data.count("title")) {
            book.title = data.at("title").get<string>();
        }
        if (data.count("author")) {
            book.author = data.at("author").get<string>();
        }
        if (data.count("price")) {
            book.price = data.at("price").get<unsigned>();
        }
        if (data.count("pages")) {
            book.pages = data.at("pages").get<unsigned>();
        }
    }

    namespace {
        map<unsigned, Book> booksRepo;
        unsigned            nextId = 1;
    }

    vector<Book> all() {
        vector<Book> books;
        for (auto p : booksRepo) books.push_back(p.second);
        return books;
    }

    unsigned insert(Book book) {
        if (book.id == 0) book.id = nextId++;
        booksRepo[book.id] = book;
        return book.id;
    }

    void update(Book book) {
        booksRepo[book.id] = book;
    }

    optional<Book> findById(int id) {
        auto it = booksRepo.find(static_cast<const unsigned int&>(id));
        if (it != booksRepo.end()) return make_optional(it->second);
        return {};
    }

    void remove(int id) {
        booksRepo.erase(static_cast<const unsigned int&>(id));
    }

    void populate() {
        insert(Book{0, "C++ REST-WS"s, "Anna Conda"s, 512, 256});
        insert(Book{0, "C++ 17"s, "Justin Time"s, 350, 155});
        insert(Book{0, "Modern C++ for Old-Timers"s, "Anna Gram"s, 425, 195});
    }
}

