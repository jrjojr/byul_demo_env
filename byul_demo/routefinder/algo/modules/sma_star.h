#ifndef SMA_STAR_H
#define SMA_STAR_H

#include "../algo.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct s_sma_star_config{
    gint memory_limit;  ///< 최대 허용 가능한 노드 수
} sma_star_config_t;

typedef sma_star_config_t* sma_star_config;

/**
 * @brief SMA* 설정 객체 생성 (기본 메모리 제한: 1000 노드)
 */
BYUL_API sma_star_config sma_star_config_new();

/**
 * @brief SMA* (Simplified Memory-Bounded A*) 알고리즘에서 사용할
 *        메모리 제한 설정 객체를 생성합니다.
 *
 * SMA*는 전통적인 A* 알고리즘과 유사하게 휴리스틱 기반의 최단 경로 탐색을 수행하지만,
 * 사용 가능한 메모리를 제한하여, 메모리를 초과하는 경우 우선순위가 낮은 노드를 삭제하고
 * 경로를 재탐색하는 구조입니다.
 *
 * 이 함수는 SMA* 알고리즘에 사용할 메모리 제한을 직접 설정하며,
 * 설정된 memory_limit 값은 frontier(우선순위 큐)에 
 * 유지 가능한 최대 노드 수를 의미합니다.
 *
 * 메모리 제한은 탐색 성능과 직접적인 관계가 있으며, 다음과 같은 원칙이 적용됩니다:
 *
 * - 제한이 낮을수록 경로 탐색 실패 확률이 높아지고, 경로 품질이 저하될 수 있습니다.
 * - 제한이 높을수록 A*에 가까운 탐색 품질을 보장하며, 
 *      트리밍이 발생하지 않으므로 안정적입니다.
 * - 메모리가 너무 낮을 경우 (예: 0 또는 1), 
 *      시작 노드 외에는 아무 것도 유지하지 못하므로,
 *   맵 크기나 장애물 복잡도에 따라 탐색 자체가 불가능할 수 있습니다.
 *
 * 🔢 메모리 제한과 맵 크기의 일반적 관계 (경험적 기준):
 *
 * - 맵 크기: `N = width × height`
 * - 최단 경로 길이: `L` (대각선 포함 약 2×N^0.5)
 *
 * 권장 공식:
 * @code
 * memory_limit ≈ max(L × (1 + ε), N × α)
 * // ε ∈ [0.5, 1.0], α ∈ [0.01, 0.05]
 * @endcode
 *
 * 예시:
 * - 맵 100×100 (N=10,000): memory_limit 1000~2000 이상 권장
 * - 맵 1000×1000 (N=1,000,000): memory_limit 10,000~50,000 이상 추천
 *
 * @param memory_limit 유지 가능한 최대 노드 수 (0 이상, 0이면 탐색 실패 가능성 높음)
 * @return sma_star_config 설정 객체 (해제 시 sma_star_config_free() 사용)
 */
BYUL_API sma_star_config sma_star_config_new_full(gint memory_limit);

/**
 * @brief SMA* 설정 객체 해제
 */
BYUL_API void sma_star_config_free(sma_star_config cfg);

/**
 * @brief SMA* 알고리즘을 사용하여 메모리 제한 하에 최단 경로를 탐색합니다.
 *
 * SMA* (Simplified Memory-Bounded A*)는 전통적인 A* 알고리즘과 유사하게
 * f = g + h 기반의 우선순위 탐색을 수행하지만, 
 * 주어진 메모리 제한(memory_limit) 내에서만
 * frontier 노드를 유지하며, 
 * 초과 시 우선순위(f값)가 가장 높은 노드를 제거하는 방식으로 작동합니다.
 *
 * 메모리 제한이 충분할 경우, A*와 유사한 경로 품질을 보장하지만,
 * 제한이 작을수록 더 많은 노드가 삭제(trimming)되며, 
 * 경로 품질이 낮아지거나 탐색에 실패할 수 있습니다.
 *
 * 제거된 노드는 `came_from` 기록을 유지하지 않기 때문에,
 * 잘못된 방향으로 진행되거나, 
 * 이미 지나온 경로를 복구하지 못하는 상황이 발생할 수 있습니다.
 *
 * 실전 사용 시 memory_limit은 맵 크기와 경로 복잡도에 따라 조정해야 합니다.
 * 일반적으로 경로 길이의 3~10배, 
 * 또는 맵의 1~5% 정도 메모리를 확보하면 안정적인 탐색이 가능합니다.
 *
 * 예제: 메모리 제한이 10인 상태에서 중앙에 수직 장애물이 존재하는 10x10 맵
 * (5열 차단)을 탐색할 경우, 우회 경로를 성공적으로 찾을 수 있습니다.
 *
 * @code
 * coord start = coord_new_full(0, 0);
 * coord goal   = coord_new_full(9, 9);
 *
 * sma_star_config cfg = sma_star_config_new_full(10); // 제한된 메모리
 * algo al = algo_new_full(
 *     10, 10,
 *     MAP_NEIGHBOR_8,
 *     PATH_ALGO_SMA_STAR,
 *     default_cost,
 *     default_heuristic,
 *     NULL,
 *     cfg,
 *     TRUE
 * );
 *
 * for (int y = 1; y < 10; y++)
 *     map_block_coord(al->m, 5, y); // 중앙 차단
 *
 * route p = algo_find(al, start, goal);
 * g_assert_true(route_get_success(p));
 * map_print_ascii_with_visited_count(al->m, p, start, goal);
 * @endcode
 *
 * @param al    알고리즘 컨텍스트 (algo_new_full로 생성)
 * @param start  시작 좌표
 * @param goal    도착 좌표
 * @return 탐색 결과 route. 성공 시 route_get_success(route) == TRUE.
 *         실패 시 success == FALSE이며, route는 비어 있을 수 있음.
 */
BYUL_API route sma_star_find(const algo al, const coord start, const coord goal);

#ifdef __cplusplus
}
#endif

#endif // SMA_STAR_H
