#pragma once

#include <string>
#include <unordered_map>
#include <set>
#include <optional>
#include <any>
#include <memory>

#include "interface.hxx"

namespace justresting {
    using namespace std;

    interface Response : NotCopyable {
        virtual void status(unsigned code, string text) = abstract;
        virtual void type(string mimeType)              = abstract;
        virtual void header(string name, string value)  = abstract;
        virtual void body(string body)                  = abstract;
        virtual void body(unique_ptr<char[]> body, unsigned long bodySize) = abstract;

        virtual unsigned            statusCode() const      = abstract;
        virtual string              statusText() const      = abstract;
        virtual string              contentType() const     = abstract;
        virtual unsigned long       contentLength() const   = abstract;
        virtual string              body() const            = abstract;

        virtual set<string>         headers() const             = abstract;
        virtual optional<string>    header(string name) const   = abstract;

        virtual void        attribute(string name, any value) = abstract;
        virtual any         attribute(string name) const      = abstract;
        virtual set<string> attributes() const                = abstract;
    };

}
