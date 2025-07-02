#ifndef IDA_STAR_H
#define IDA_STAR_H

#include "internal/algo.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief IDA* (Iterative Deepening A*) 알고리즘을 사용하여 경로를 탐색합니다.
 *
 * 이 알고리즘은 깊이 우선 탐색(DFS)과 A*의 휴리스틱을 결합한 방식으로,
 * f = g + h 값을 임계값(threshold)으로 설정하고, 
 * 이를 점진적으로 증가시키며 반복 탐색합니다.
 * 메모리 사용을 최소화하면서도 최적 경로를 찾을 수 있습니다.
 *
 * 내부적으로 algo 구조체의 frontier, visited, cost_so_far, 
 * came_from를 모두 활용하며,
 * 각 반복에서 frontier와 visited는 새롭게 초기화됩니다.
 *
 * - cost_fn과 heuristic_fn은 모두 필요합니다.
 * - algo_specific은 사용하지 않으며 NULL로 설정해야 합니다.
 * - frontier에는 priority 값으로 g(누적 비용)를 전달합니다.
 *
 * 사용 예:
 * @code
 * coord start = coord_new_full(0, 0);
 * coord goal = coord_new_full(9, 9);
 * algo al = algo_new_full(
 *     10, 10,
 *     MAP_NEIGHBOR_8,
 *     PATH_ALGO_IDA_STAR,
 *     default_cost,
 *     default_heuristic,
 *     NULL,
 *     NULL,
 *     TRUE
 * );
 *
 * // 장애물 설정
 * for (int y = 1; y < 10; y++)
 *     map_block_coord(al->m, 5, y);
 *
 * route p = algo_find(al, start, goal);
 * map_print_ascii_with_visited_count(al->m, p, start, goal);
 *
 * route_free(p);
 * coord_free(start);
 * coord_free(goal);
 * algo_free(al);
 * @endcode
 *
 * @param al    알고리즘 컨텍스트 (algo_new_full로 생성)
 * @param start  시작 좌표
 * @param goal    도착 좌표
 * @return 경로 결과. 실패 시 success == FALSE
 */
BYUL_API route ida_star_find(const algo al, const coord start, const coord goal);


#ifdef __cplusplus
}
#endif

#endif // IDA_STAR_H
