#ifndef COORD_HPP
#define COORD_HPP

#include "internal/coord.h"
#include <cstddef>     // for size_t
#include <functional>  // for std::hash

/// C++ 전용 확장
/// coord_t를 값 타입으로 사용하기 위한 비교 및 해시 지원

inline bool operator==(const coord_t& a, const coord_t& b) {
    return coord_equal(&a, &b);
}

namespace std {
    template<>
    struct hash<coord_t> {
        std::size_t operator()(const coord_t& c) const {
            return static_cast<std::size_t>(coord_hash(&c));
        }
    };
}

#endif // COORD_HPP
