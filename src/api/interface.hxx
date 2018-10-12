#pragma  once

#define interface struct
#define abstract 0
namespace justresting {

    struct NotCopyable {
        NotCopyable()                             = default;
        virtual ~NotCopyable()                    = default;
        NotCopyable(const NotCopyable&)             = delete;
        NotCopyable& operator=(const NotCopyable&)  = delete;
    };

}
