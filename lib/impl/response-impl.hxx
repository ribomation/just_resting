#pragma once

#include <unordered_map>
#include "response.hxx"

namespace just_resting {

    class ResponseImpl : public Response {
        unsigned                      statusCode_    = 200;
        string                        statusText_    = "OK"s;
        string                        contentType_   = "text/plain"s;
        unsigned long                 contentLength_ = 0;
        bool                          closed         = false;
        unique_ptr<char[]>            body_;
        unordered_map<string, string> headers_;
        unordered_map<string, any>    attributes_;

    public:
        ResponseImpl() = default;
        ~ResponseImpl() = default;

        void status(unsigned code, string text) override;
        void type(string mimeType)              override;
        void header(string name, string value)  override;
        void body(string body)                  override;
        void body(unique_ptr<char[]> body, unsigned long bodySize) override;

        unsigned        statusCode() const override;
        string          statusText() const override;
        string          contentType() const override;
        unsigned long   contentLength() const override;
        string          body() const override;
        const void*     bodyData() const;

        set<string>      headers() const override;
        optional<string> header(string name) const override;

        void        attribute(string name, any value) override;
        any         attribute(string name) const override;
        set<string> attributes() const override;

        bool isClosed() const { return closed; }
        void close()          {closed=true;}
    };


}


