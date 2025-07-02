#ifndef FAST_MARCHING_H
#define FAST_MARCHING_H

#include "internal/algo.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Fast Marching Method (FMM)를 사용하여 거리 기반 경로를 탐색합니다.
 *
 * 이 알고리즘은 시작점에서부터 모든 방향으로 점진적으로 거리장을 확장하면서
 * 가장 빠르게 도달 가능한 좌표부터 탐색을 진행합니다.
 * FMM은 Dijkstra와 유사하지만, 
 * 각 좌표에 정의된 **속도 함수(speed function)**를 기반으로
 * 실수 단위 거리 필드를 계산하며, 경로가 아닌 거리장의 정확한 확산이 핵심입니다.
 *
 * 이때 사용되는 `cost_fn()`은 일반적인 고정 비용 함수가 아니라,
 * **속도 함수의 역수(= 1 / speed)**처럼 작동해야 하며,
 * FMM의 품질과 의미를 좌우하는 가장 중요한 요소입니다.
 *
 * 만약 모든 좌표의 cost가 동일하다면, FMM은 Dijkstra와 동일한 경로를 탐색하게 되며,
 * 그 차별점이 사라지므로 **전용 terrain_cost() 함수**를 반드시 구현해야 합니다.
 *
 * 예시: 진흙, 풀, 도로 등 서로 다른 이동 속도를 갖는 지형을 cost로 표현
 * @code
 * gfloat terrain_cost(const map m, 
 *      const coord start, const coord goal, gpointer data) {
 * 
 *     tile_type type = get_tile_type(goal->x, goal->y);
 *     switch (type) {
 *         case TILE_ROAD: return 0.3f;   // 빠름
 *         case TILE_GRASS: return 1.0f;
 *         case TILE_SWAMP: return 4.0f;   // 느림
 *         default: return 1.0f;
 *     }
 * }
 * @endcode
 *
 * 예시 테스트:
 * @code
 * coord start = coord_new_full(0, 0);
 * coord goal   = coord_new_full(9, 9);
 *
 * algo al = algo_new_full(
 *     10, 10,
 *     MAP_NEIGHBOR_8,
 *     PATH_ALGO_FAST_MARCHING,
 *     terrain_cost,  // ✅ 반드시 전용 cost 사용
 *     NULL,
 *     NULL,
 *     NULL,
 *     TRUE
 * );
 *
 * // 중앙 장애물 추가
 * for (int y = 1; y < 10; y++)
 *     map_block_coord(al->m, 5, y);
 *
 * route p = algo_find(al, start, goal);
 * map_print_ascii_with_visited_count(al->m, p, start, goal);
 * @endcode
 *
 * @note Fast Marching은 휴리스틱을 사용하지 않으며,
 *       f = g + h 대신 g 값만을 기반으로 우선순위를 결정합니다.
 *
 * @param al    알고리즘 컨텍스트 (algo_new_full로 생성)
 * @param start  시작 좌표
 * @param goal    도착 좌표
 * @return 경로 결과. success == TRUE면 경로를 찾았으며,
 *         실패 시 success == FALSE가 반환됩니다.
 */
BYUL_API route fast_marching_find(const algo al, 
    const coord start, const coord goal);

#ifdef __cplusplus
}
#endif

#endif // FAST_MARCHING_H
