
#include "request-impl.hxx"
#include <regex>
#include <stdexcept>
#include <iostream>
#include <iterator>

namespace justresting {
    using namespace std;
    using namespace literals;

    string_view RequestImpl::method() const {
        return method_;
    }

    string_view RequestImpl::path() const {
        return path_;
    }

    string_view RequestImpl::protocol() const {
        return protocol_;
    }

    StringValue RequestImpl::param() const {
        return param_;
    }

    string_view RequestImpl::query() const {
        return query_;
    }

    string_view RequestImpl::body() const {
        return body_;
    }

    optional<string_view> RequestImpl::header(string_view name) const {
        if (headers_.count(name) > 0) {
            return make_optional<string_view>(headers_.at(name));
        }
        return optional<string_view>();
    }

    set<string_view> RequestImpl::headers() const {
        set<string_view> names;
        for (auto        p : headers_) names.insert(p.first);
        return names;
    }

    void RequestImpl::attribute(string name, any value) {
        attributes_[name] = value;
    }

    any RequestImpl::attribute(string name) const {
        if (attributes_.count(name) == 1) return attributes_.at(name);
        return {};
    }

    set<string> RequestImpl::attributes() const {
        set<string> names;
        for (auto   p : attributes_) names.insert(p.first);
        return names;
    }

}

