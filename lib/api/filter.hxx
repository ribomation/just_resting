#pragma once

#include "request.hxx"
#include "response.hxx"
#include "route.hxx"
#include "interface.hxx"

namespace just_resting {
    class Engine;

    class Filter : NotCopyable {
        Filter* next_ = nullptr;
        friend Engine;

    protected:
        Filter* next() const { return next_; }

    public:
        virtual void invoke(Request& req, Response& res, Route& route) = abstract;
    };

}
