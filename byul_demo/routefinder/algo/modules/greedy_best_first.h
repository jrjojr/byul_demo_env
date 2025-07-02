#ifndef GREEDY_BEST_FIRST_H
#define GREEDY_BEST_FIRST_H

#include "internal/algo.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Greedy Best-First Search 알고리즘으로 경로를 탐색합니다.
 *
 * 이 알고리즘은 누적 비용(g)을 무시하고, 휴리스틱(h) 값만을 기준으로
 * 우선순위 큐를 사용하여 목적지까지 가장 "가까워 보이는" 경로를 탐색합니다.
 *
 * - cost_fn은 사용되지 않으며 NULL로 설정해도 됩니다.
 * - heuristic_fn은 반드시 설정되어야 합니다 (예: default_heuristic).
 * - algo_specific은 사용하지 않습니다 (NULL).
 *
 * 사용 예:
 * @code
 * coord start = coord_new_full(0, 0);
 * coord goal = coord_new_full(9, 9);
 * algo al = algo_new_full(
 *     10, 10,
 *     MAP_NEIGHBOR_8,
 *     PATH_ALGO_GREEDY_BEST_FIRST,
 *     NULL,
 *     default_heuristic,
 *     NULL,
 *     NULL,
 *     TRUE
 * );
 * route p = algo_find(al, start, goal);
 * @endcode
 *
 * @param al    알고리즘 컨텍스트 (필수)
 * @param start  시작 좌표
 * @param goal    도착 좌표
 * @return 경로 결과. 실패 시 success == FALSE
 */
BYUL_API route greedy_best_first_find(const algo al,
    const coord start, const coord goal);

#ifdef __cplusplus
}
#endif

#endif // GREEDY_BEST_FIRST_H
