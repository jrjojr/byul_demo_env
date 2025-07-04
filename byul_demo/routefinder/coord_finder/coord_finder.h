#ifndef COORD_RADAR_H
#define COORD_RADAR_H

#include <glib.h>
#include "byul_config.h"

#include "internal/coord.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_RANGE_LIMIT 256

// 🔍 사용자가 지정하는 도달 가능성 판단 함수
typedef gboolean (*is_reeachable_func)(const coord_t* c, gpointer user_data);

/**
 * @brief 기준 좌표 주변에서 가장 가까운 reachable 셀을 BFS로 탐색합니다.
 * 
 * @param start         기준 좌표 (일반적으로 클릭된 좌표)
 * @param is_reachable    도달 가능 여부를 판단하는 콜백 함수
 * @param user_data       콜백에 전달할 사용자 데이터
 * @param max_range      최대 탐색 반경 (예: 5 → 5칸)
 * @param out_result      결과 좌표 포인터 (성공 시 셋팅됨, 실패 시 x=-1, y=-1)
 * 
 * @return gboolean       TRUE: 찾음, FALSE: 실패
 */
BYUL_API gboolean find_goal_bfs(const coord_t* start,
                        is_reeachable_func is_reachable_fn,
                        gpointer user_data,
                        gint max_range,
                        coord_t** out_result);


// 내부 구조체: A* 노드
typedef struct s_astar_node{
    coord_t m_coord_t;
    gint cost;
    gint heuristic;
} astar_node_t;

typedef struct s_astar_node* astar_node;

BYUL_API gint astar_node_compare(gconstpointer a, gconstpointer b, gpointer user_data);

/**
 * @brief GPriorityQueue 기반 A* 방식으로 가장 가까운 reachable 좌표를 탐색
 */
BYUL_API gboolean find_goal_astar(const coord_t* start,
                          is_reeachable_func is_reachable_fn,
                          gpointer user_data,
                          gint max_range,
                          coord_t* *out_result);

#ifdef __cplusplus
}
#endif

#endif // COORD_RADAR_H
