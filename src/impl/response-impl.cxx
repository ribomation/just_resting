
#include "response-impl.hxx"

namespace justresting {
    using namespace std;
    using namespace literals;

    void ResponseImpl::status(unsigned code, string text) {
        statusCode_ = code;
        statusText_ = text;
    }

    void ResponseImpl::type(string mimeType) {
        contentType_ = mimeType;
    }

    void ResponseImpl::body(string body) {
        contentLength_ = body.size();
        auto buffer = new char[contentLength_];
        body.copy(buffer, contentLength_);
        body_ = unique_ptr<char[]>(buffer);
    }

    void ResponseImpl::body(unique_ptr<char[]> body, unsigned long bodySize) {
        contentLength_ = bodySize;
        body_      = move(body);
    }

    const void* ResponseImpl::bodyData() const {
        return body_.get();
    }

    unsigned ResponseImpl::statusCode() const {
        return statusCode_;
    }

    string ResponseImpl::statusText() const {
        return statusText_;
    }

    string ResponseImpl::contentType() const {
        return contentType_;
    }

    unsigned long ResponseImpl::contentLength() const {
        return contentLength_;
    }

    string ResponseImpl::body() const {
        return {body_.get(), contentLength_};
    }

    void ResponseImpl::header(string name, string value) {
        headers_[name] = value;
    }

    set<string> ResponseImpl::headers() const {
        set<string> names;
        for (auto   p : headers_) names.insert(p.first);
        return names;
    }

    optional<string> ResponseImpl::header(string name) const {
        if (headers_.count(name) > 0) {
            return make_optional<string>(headers_.at(name));
        }
        return optional<string>();
    }

    void ResponseImpl::attribute(string name, any value) {
        attributes_[name] = value;
    }

    set<string> ResponseImpl::attributes() const {
        set<string> names;
        for (auto   p : attributes_) names.insert(p.first);
        return names;
    }

    any ResponseImpl::attribute(string name) const {
        if (attributes_.count(name) == 1) return attributes_.at(name);
        return {};
    }


}
