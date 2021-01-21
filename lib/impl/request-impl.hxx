#pragma once

#include "request.hxx"
#include <unordered_map>

namespace just_resting {

    struct RequestImpl : public Request {
        string_view                             method_;
        string_view                             path_;
        string_view                             protocol_;
        string_view                             body_;
        string_view                             query_;
        string_view                             param_;
        unordered_map<string_view, string_view> headers_;
        unordered_map<string, any>              attributes_;

        RequestImpl() = default;
        ~RequestImpl() = default;

        string_view method()    const override;
        string_view path()      const override;
        string_view protocol()  const override;
        string_view body()      const override;
        StringValue param()     const override;
        string_view query()     const override;


        optional<string_view> header(string_view name) const override;
        set<string_view> headers() const override;

        void attribute(string name, any value) override;
        any attribute(string name) const override;
        set<string> attributes() const override;
    };

}

