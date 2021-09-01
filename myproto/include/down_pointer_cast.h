#pragma once

#include <memory>

namespace google {
namespace protobuf {

template<typename To, typename From>     // use like this: down_pointer_cast<T>(foo);
inline ::std::shared_ptr<To> down_pointer_cast(const ::std::shared_ptr<From>& f) {
    // so we only accept smart pointers
    // Ensures that To is a sub-type of From *.  This test is here only
    // for compile-time type checking, and has no overhead in an
    // optimized build at run-time, as it will be optimized away
    // completely.
    if (false) {
        implicit_cast<const From*, To*>(0);
    }

#if !defined(NDEBUG) && !defined(GOOGLE_PROTOBUF_NO_RTTI)
    assert(f == NULL || dynamic_cast<To*>(f.get()) != NULL);  // RTTI: debug mode only!
#endif
    return ::std::static_pointer_cast<To>(f);
}

}  // namespace protobuf
}  // namespace google

