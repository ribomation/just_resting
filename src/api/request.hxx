#pragma once

#include <string>
#include <string_view>
#include <optional>
#include <set>
#include <any>
#include "interface.hxx"
#include "string-value.hxx"

namespace justresting {
    using namespace std;

    interface Request : NotCopyable {
        virtual string_view method()   const = abstract;
        virtual string_view path()     const = abstract;
        virtual string_view protocol() const = abstract;
        virtual string_view body()     const = abstract;
        virtual string_view query()    const = abstract;
        virtual StringValue param()    const = abstract;

        virtual optional<string_view> header(string_view name) const = abstract;
        virtual set<string_view>      headers()                const = abstract;

        virtual void        attribute(string name, any value) = abstract;
        virtual any         attribute(string name) const      = abstract;
        virtual set<string> attributes() const                = abstract;
    };

}
