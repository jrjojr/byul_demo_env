#ifndef DSTAR_LITE_KEY_HPP
#define DSTAR_LITE_KEY_HPP

#include "byul_config.h"
#include "internal/coord.h"
#include "internal/dstar_lite_key.h"

#include <functional>
#include <cstddef>
#include <vector>
#include <map>

// ------------------------ 값 기반 연산자 오버로딩 ------------------------

/// @brief std::map / std::set용 정렬 비교자
inline bool operator<(const dstar_lite_key_t& a, const dstar_lite_key_t& b) {
    return dstar_lite_key_compare(&a, &b) < 0;
}

/// @brief 동등 비교 (unordered_map 등에서 사용)
inline bool operator==(const dstar_lite_key_t& a, const dstar_lite_key_t& b) {
    return dstar_lite_key_equal(&a, &b);
}

inline bool operator!=(const dstar_lite_key_t& a, const dstar_lite_key_t& b) {
    return !dstar_lite_key_equal(&a, &b);
}

// ------------------------ 해시 함수 (std namespace) ------------------------

namespace std {
    template<>
    struct hash<dstar_lite_key_t> {
        std::size_t operator()(const dstar_lite_key_t& key) const {
            return static_cast<std::size_t>(dstar_lite_key_hash(&key));
        }
    };
}

// ------------------------ 포인터 기반 큐 구조 ------------------------

struct dstar_lite_key_ptr_less {
    bool operator()(
        const dstar_lite_key_t* a, const dstar_lite_key_t* b) const {
            
        return dstar_lite_key_compare(a, b) < 0;
    }
};

#endif // DSTAR_LITE_KEY_HPP
