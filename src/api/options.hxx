#pragma once

namespace justresting {

    using IntType = unsigned long long;
    using FloatType = long double;
    constexpr static unsigned K = 1024;

    constexpr IntType operator "" _kb(IntType value) {
        return value * K;
    }

    constexpr IntType operator "" _kb(FloatType value) {
        return static_cast<IntType>(value * K);
    }

    constexpr IntType operator "" _mb(IntType value) {
        return value * (K * K);
    }

    constexpr IntType operator "" _mb(FloatType value) {
        return static_cast<IntType>(value * (K * K));
    }

    constexpr IntType operator "" _gb(IntType value) {
        return value * (K * K * K);
    }

    constexpr IntType operator "" _gb(FloatType value) {
        return static_cast<IntType>(value * (K * K * K));
    }

    struct Options {
        unsigned short port                                = 4242;
        unsigned long  max_request_size_in_bytes           = 10_mb;
        unsigned int   max_request_waiting_time_in_seconds = 4;
        bool           debug                               = false;
        unsigned int   debug_max_payload_trace_size        = 512;
    };

}
