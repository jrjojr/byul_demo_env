#ifndef DFS_H
#define DFS_H

#include "byul_config.h"
#include "core.h"
#include "internal/coord.h"
#include "internal/map.h"
#include "internal/route.h"
#include "internal/algo.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief DFS(Depth-First Search) 알고리즘을 이용해 경로를 탐색합니다.
 *
 * DFS는 스택 기반의 깊이 우선 탐색 알고리즘으로, 가능한 한 깊게 탐색한 뒤 
 * 막히면 이전 지점으로 백트래킹(backtracking)하여 다시 진행합니다.
 * 
 * 이 함수는 휴리스틱이나 가중치 없이 단순히 경로를 찾으며,
 * 항상 최적 경로를 찾는 것은 보장하지 않지만 구현이 단순하고 빠릅니다.
 * 
 * @param al     알고리즘 컨텍스트 (algo 구조체). 반드시 초기화되어 있어야 하며, 
 *                  내부적으로 visited, frontier 등 상태를 관리합니다.
 * @param start   시작 좌표
 * @param goal     도착 좌표
 * @return route  탐색 결과 경로 객체. 
 *              @c route_get_success(p) 가 TRUE이면 경로 탐색에 성공한 것입니다.
 *
 * @usage 예시:
 * @code
 * algo al = algo_new_default(10, 10, PATH_ALGO_DFS, TRUE);
 *
 * // 맵 중간에 수직 벽을 생성하여 경로를 막음
 * for (int y = 1; y < 10; y++)
 *     map_block_coord(al->m, 3, y);
 *
 * coord start = coord_new_full(0, 0);
 * coord goal   = coord_new_full(9, 9);
 *
 * route p = dfs_find(al, start, goal);
 * if (route_get_success(p)) {
 *     map_print_ascii_with_visited_count(al->m, p, start, goal);
 * }
 *
 * route_free(p);
 * coord_free(start);
 * coord_free(goal);
 * algo_free(al);
 * @endcode
 *
 * @note
 * DFS는 막힌 영역이 많거나 루프가 있는 경우 비효율적일 수 있으며,
 * 최단 경로가 아닌 **가장 먼저 발견되는 경로**를 반환합니다.
 *
 * @see algo_new_default(), 
 *      map_block_coord(), 
 *      map_print_ascii_with_visited_count()
 */
BYUL_API route dfs_find(const algo al, const coord start, const coord goal);

#ifdef __cplusplus
}
#endif

#endif // DFS_H
