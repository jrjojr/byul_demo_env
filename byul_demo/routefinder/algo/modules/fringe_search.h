#ifndef FRINGE_SEARCH_H
#define FRINGE_SEARCH_H

#include "internal/algo.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct s_fringe_search_config{
    gfloat delta_epsilon; // 허용 가능한 threshold 오차
} fringe_search_config_t;
typedef fringe_search_config_t* fringe_search_config;

/**
 * @brief fringe_search_config 객체를 생성합니다 (기본값 사용).
 * 
 * 기본 threshold 오차값으로 0.5f가 설정됩니다.
 * fringe_search 알고리즘에서 f값의 비교에 유연성을 부여합니다.
 * 
 * @return 새로 할당된 fringe_search_config 포인터
 */
BYUL_API fringe_search_config fringe_search_config_new();

/**
 * @brief fringe_search_config 객체를 생성합니다 (사용자 지정 오차값).
 * 
 * fringe_search 알고리즘은 f = g + h 값을 기반으로 threshold 이내 노드만 탐색합니다.
 * 이때 delta_epsilon은 threshold 경계에서의 허용 오차 범위를 의미하며,
 * 값이 클수록 더 많은 노드가 확장 대상으로 인정됩니다.
 * 
 * 예를 들어 맵이 10x10이고 heuristic이 유클리드 거리인 경우,
 * 최대 f값은 약 14~20 정도로 예상되며, 
 * delta_epsilon을 1.0~2.0 정도로 설정하는 것이 보통입니다.
 * 값이 지나치게 크면 fringe의 pruning 특성이 사라지고, 
 * 너무 작으면 경로를 찾지 못할 수 있습니다.
 * 
 * @param delta threshold 비교 시 허용할 f값 오차 범위
 * @return 새로 할당된 fringe_search_config 포인터
 */
BYUL_API fringe_search_config fringe_search_config_new_full(gfloat weight);

/**
 * @brief fringe_search_config 메모리 해제
 * 
 * @param cfg 해제할 설정 포인터
 */
BYUL_API void fringe_search_config_free(fringe_search_config cfg);

/**
 * @brief Fringe Search 알고리즘을 사용하여 경로를 탐색합니다.
 *
 * Fringe Search는 A* 알고리즘의 성능 병목인 Open List 정렬을 제거하고,
 * f = g + h 값에 기반한 threshold를 점진적으로 확장하며 경로를 탐색합니다.
 *
 * 내부적으로는 두 개의 리스트를 번갈아 사용하며,
 * 현재 threshold 이내의 노드를 순차적으로 처리하고,
 * 초과하는 노드는 다음 라운드로 이월합니다.
 *
 * 이 구조는 A*보다 빠른 속도를 제공하지만,
 * 다음 노드를 선택할 때 정렬을 하지 않기 때문에 경로가 최적이 아닐 수 있습니다.
 *
 * threshold 비교에는 fringe_search_config 구조체에 포함된 
 * delta_epsilon 값을 사용합니다.
 * delta_epsilon 값이 클수록 더 많은 노드를 허용하고,
 * 작을수록 pruning 효과는 크지만 실패 확률도 증가합니다.
 *
 * 보통 맵이 10x10이고 유클리드 휴리스틱을 사용하는 경우,
 * delta_epsilon은 1.0 ~ 3.0 사이가 적절합니다.
 *
 * @note 이 알고리즘은 heuristic_fn이 반드시 설정되어 있어야 하며,
 *       algo_new_full() 호출 시 algo_specific 인자로 
 *      fringe_search_config를 전달해야 합니다.
 *
 * @code
 * // 오차 범위 지정
 * fringe_search_config cfg = fringe_search_config_new_full(2.0f); 
 * algo al = algo_new_full(
 *     10, 10,
 *     MAP_NEIGHBOR_8,
 *     PATH_ALGO_FRINGE_SEARCH,
 *     default_cost,
 *     default_heuristic,
 *     NULL,
 *     cfg,
 *     TRUE
 * );
 *
 * route p = algo_find(al, start, goal);
 * // ...
 * algo_free(al);
 * fringe_search_config_free(cfg);
 * @endcode
 *
 * @param al    알고리즘 컨텍스트 (algo_new_full()로 생성)
 * @param start  시작 좌표
 * @param goal    도착 좌표
 * @return 탐색 결과 route 객체. success == TRUE이면 경로 찾기 성공.
 *         실패 시 success == FALSE이며 coords는 비어 있을 수 있습니다.
 */
BYUL_API route fringe_search_find(const algo al,
    const coord start, const coord goal);

#ifdef __cplusplus
}
#endif

#endif // FRINGE_SEARCH_H
