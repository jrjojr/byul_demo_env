
#ifndef DSTAR_LITE_PQUEUE_H
#define DSTAR_LITE_PQUEUE_H

#include "byul_config.h"
#include "internal/coord.h"
#include "internal/dstar_lite_key.h"
#include "internal/pqueue.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct s_dstar_lite_pqueue {
    pqueue pq;  // 내부 우선순위 큐 (key: dstar_lite_key, value: coord_t*)
    GHashTable* coord_to_key; // coord_t** → float* 역참조용
} *dstar_lite_pqueue;

BYUL_API dstar_lite_pqueue dstar_lite_pqueue_new();

BYUL_API void dstar_lite_pqueue_free(dstar_lite_pqueue q);

BYUL_API dstar_lite_pqueue dstar_lite_pqueue_copy(dstar_lite_pqueue src);

BYUL_API void dstar_lite_pqueue_push(
    dstar_lite_pqueue q, const dstar_lite_key key, const coord_t* c);

BYUL_API coord_t* dstar_lite_pqueue_peek(dstar_lite_pqueue q);

BYUL_API coord_t* dstar_lite_pqueue_pop(dstar_lite_pqueue q);

BYUL_API gboolean dstar_lite_pqueue_is_empty(dstar_lite_pqueue q);

/// @brief 해당 key에 해당하는 value를 순서대로 제거
BYUL_API gboolean dstar_lite_pqueue_remove(
    dstar_lite_pqueue q, const coord_t* u);

/// @brief 해당 key, coord_t* 쌍을 큐에서 제거
BYUL_API gboolean dstar_lite_pqueue_remove_full(
    dstar_lite_pqueue q, const dstar_lite_key key, const coord_t* c);

/// @brief 값(c)에 해당하는 key를 찾아 반환 (없으면 NULL)
BYUL_API dstar_lite_key dstar_lite_pqueue_find_key_by_coord(
    dstar_lite_pqueue q, const coord_t* c);

/// @brief 현재 top key를 복사해서 out_key에 반환
BYUL_API dstar_lite_key dstar_lite_pqueue_top_key(dstar_lite_pqueue q);

BYUL_API gboolean dstar_lite_pqueue_contains(
    dstar_lite_pqueue, const coord_t* u);

#ifdef __cplusplus
}
#endif

#endif // DSTAR_LITE_PQUEUE_H
