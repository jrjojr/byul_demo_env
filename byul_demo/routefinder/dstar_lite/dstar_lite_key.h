#ifndef DSTAR_LITE_KEY_H
#define DSTAR_LITE_KEY_H

#include "byul_config.h"
#include "internal/coord.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct s_dstar_lite_key {
    float k1;
    float k2;
} dstar_lite_key_t;

// ------------------------ 생성/복사/해제 ------------------------

BYUL_API dstar_lite_key_t* dstar_lite_key_new(void);

BYUL_API dstar_lite_key_t* dstar_lite_key_new_full(float k1, float k2);

BYUL_API dstar_lite_key_t* dstar_lite_key_copy(const dstar_lite_key_t* key);

BYUL_API void dstar_lite_key_free(dstar_lite_key_t* key);

// ------------------------ 비교 함수 ------------------------

/// @brief 키 비교 (k1 우선, 동일하면 k2)
/// @return 음수: dsk0 < dsk1, 0: 동일, 양수: dsk0 > dsk1
BYUL_API int dstar_lite_key_compare(const dstar_lite_key_t* dsk0,
                                    const dstar_lite_key_t* dsk1);

/// @brief 키가 동등한지 확인 (float 오차 허용)
/// @return true = 거의 같다
BYUL_API bool dstar_lite_key_equal(const dstar_lite_key_t* dsk0,
                                   const dstar_lite_key_t* dsk1);

/// @brief 키의 해시 값 계산 (hash map용)
BYUL_API unsigned int dstar_lite_key_hash(const dstar_lite_key_t* key);

#ifdef __cplusplus
}
#endif

#endif // DSTAR_LITE_KEY_H
