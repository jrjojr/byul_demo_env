#include "internal/algo.h"
#include "internal/bfs.h"
#include "internal/dfs.h"
#include "internal/dijkstra.h"
#include "internal/astar.h"
#include "internal/weighted_astar.h"
#include "internal/greedy_best_first.h"
#include "internal/ida_star.h"
#include "internal/rta_star.h"
#include "internal/sma_star.h"
#include "internal/fast_marching.h"
#include "internal/fringe_search.h"
// #include "internal/dstar_lite.h"

#include <math.h>

/** --- 비용 계산 함수들 --- */

gfloat default_cost(const map m, 
    const coord start, const coord goal, gpointer userdata) {

    return 1.0f;
}

gfloat zero_cost(const map m, 
    const coord start, const coord goal, gpointer userdata) {

    return 0.0f;
}

gfloat terrain_cost(const map m, 
    const coord start, const coord goal, gpointer userdata) {

    if (!m || !goal) return G_MAXFLOAT;
    gint terrain = map_get_tile_type(m, goal->x, goal->y); // 사용자 구현 필요
    switch (terrain) {
        case 0: return 1.0f; // 평지
        case 1: return 2.0f; // 숲
        case 2: return 5.0f; // 산
        default: return G_MAXFLOAT;
    }
}

gfloat diagonal_cost(const map m, 
    const coord start, const coord goal, gpointer userdata) {

    if (!start || !goal) return G_MAXFLOAT;
    gint dx = abs(start->x - goal->x);
    gint dy = abs(start->y - goal->y);
    return (dx != 0 && dy != 0) ? DIAGONAL_COST : 1.0f;
}

gfloat height_cost(const map m, 
    const coord start, const coord goal, gpointer userdata) {

    if (!m || !start || !goal) return G_MAXFLOAT;
    gint h1 = map_get_tile_height(m, start->x, start->y); // 사용자 구현 필요
    gint h2 = map_get_tile_height(m, goal->x, goal->y);
    return 1.0f + abs(h1 - h2) * 0.5f;
}

gfloat euclidean_heuristic(const coord start, const coord goal, 
    gpointer userdata) {

    if (!start || !goal) return G_MAXFLOAT;
    gint dx = start->x - goal->x;
    gint dy = start->y - goal->y;
    return sqrtf((float)(dx * dx + dy * dy));
}

gfloat manhattan_heuristic(const coord start, const coord goal, 
    gpointer userdata) {

    if (!start || !goal) return G_MAXFLOAT;
    return abs(start->x - goal->x) + abs(start->y - goal->y);
}

gfloat chebyshev_heuristic(const coord start, const coord goal, 
    gpointer userdata) {

    if (!start || !goal) return G_MAXFLOAT;
    gint dx = abs(start->x - goal->x);
    gint dy = abs(start->y - goal->y);
    return dx > dy ? dx : dy;
}

gfloat octile_heuristic(const coord start, const coord goal, 
    gpointer userdata) {

    if (!start || !goal) return G_MAXFLOAT;
    gint dx = abs(start->x - goal->x);
    gint dy = abs(start->y - goal->y);
    return (gfloat)(MAX(dx, dy) + (sqrtf(2.0f) - 1.0f) * MIN(dx, dy));
}

gfloat zero_heuristic(const coord start, const coord goal, 
    gpointer userdata) {

    return 0.0f;
}

gfloat default_heuristic(const coord start, const coord goal, 
    gpointer userdata) {

    return euclidean_heuristic(start, goal, userdata);
}



algo algo_new(void) {
    return algo_new_full(
        10, 10, MAP_NEIGHBOR_8, 
        PATH_ALGO_BFS,        
        default_cost,
        euclidean_heuristic,
        NULL,
        NULL,
        FALSE
    );
}

// 8방향, 유클리드 거리, 비용 1.0 이 기본이다.
algo algo_new_default(
    gint width,                        // 맵 가로 크기
    gint height,                       // 맵 세로 크기
    route_algotype_t algotype,    
    gboolean visited_logging    // 디버깅용 방문 순서 로깅 여부
) {
    return algo_new_full(
        width, 
        height, 
        MAP_NEIGHBOR_8, 
        algotype,        
        default_cost,
        euclidean_heuristic,
        NULL,
        NULL,
        visited_logging
    );    
}

algo algo_new_full(
    gint width, 
    gint height,
    map_neighbor_mode_t mode, 
    route_algotype_t algotype,    
    cost_func cost_fn,
    heuristic_func heuristic_fn,
    gpointer userdata,
    gpointer algo_specific,
    gboolean visited_logging
) {
    algo al = g_malloc0(sizeof(algo_t));
    if (!al) return NULL;

    al->m = map_new_full(width, height, mode);
    al->algotype = algotype;
    al->frontier_type = get_frontier_type(algotype);
    al->algo_find_fn = get_algo_find_func(algotype);
    al->cost_fn = cost_fn;
    al->heuristic_fn = heuristic_fn;
    al->userdata = userdata;
    al->algo_specific = algo_specific;

    // GHashTable* visited;                 // 방문 여부 (set)
    al->visited = g_hash_table_new_full(
        (GHashFunc) coord_hash,
        (GEqualFunc) coord_equal,
        (GDestroyNotify) coord_free,
        NULL
    );

    // GHashTable* came_from;              // 현재 좌표가 어디에서 왔나 (dict)
    al->came_from = g_hash_table_new_full(
        (GHashFunc) coord_hash,
        (GEqualFunc) coord_equal,
        (GDestroyNotify) coord_free,
        (GDestroyNotify) coord_free
    );

    // GHashTable* cost_so_far;            // 현재 좌표의 비용 (dict)
    al->cost_so_far = g_hash_table_new_full(
        (GHashFunc) coord_hash,
        (GEqualFunc) coord_equal,
        (GDestroyNotify) coord_free,
        g_free  //float 동적메모리 할당
    );

    // 프론티어는 알고리즘에서 초기화
    al->frontier = NULL;

    al->visited_logging = visited_logging;

    return al;
}

// void algo_free(algo al) {
//     if (!al) return;
//     map_free(al->m);
//     g_hash_table_destroy(al->visited);
//     g_hash_table_destroy(al->came_from);
//     g_hash_table_destroy(al->cost_so_far);
//     g_free(al);
// }

void algo_free(algo al) {
    if (!al) return;

    // 포함된 맵 해제
    if (al->m)
        map_free(al->m);

    // 해시테이블 해제
    if (al->visited)
        g_hash_table_destroy(al->visited);

    if (al->came_from)
        g_hash_table_destroy(al->came_from);

    if (al->cost_so_far)
        g_hash_table_destroy(al->cost_so_far);

    // 프론티어는 별도 처리 필요 시 알고리즘 내부에서 관리
    // if (al->frontier) ... (우선순위 큐, GQueue 등 해제 필요할 경우)
    if (al->frontier) algo_free_frontier(al);

    // 알고리즘 전용 구조체도 필요 시 사용자가 해제하도록
    // if (al->algo_specific) g_free(al->algo_specific); <-- 직접 할당했을 경우에만

    g_free(al);
}

void algo_free_full(algo al, GDestroyNotify specific_free_func) {
    if (!al) return;

    if (al->algo_specific && specific_free_func)
        specific_free_func(al->algo_specific);

    algo_free(al);
}

route algo_find(const algo al, const coord start, const coord goal) {
    return al->algo_find_fn(al, start, goal);
}

void algo_set_cost_func(algo al, cost_func func) {
    if (al) al->cost_fn = func;
}

void algo_set_heuristic_func(algo al, heuristic_func func) {
    if (al) al->heuristic_fn = func;
}

void algo_set_userdata(algo al, gpointer userdata) {
    if (al) al->userdata = userdata;
}

void algo_set_algo_specific(algo al, gpointer specific) {
    if (al) al->algo_specific = specific;
}

// route_algotype_t algo_get_algotype(const algo al) {
//     return al->algotype;
// }

// algo_find_func algo_get_algo_find_func(const algo al) {
//     return al ? al->algo_find_fn : NULL;
// }

cost_func algo_get_cost_func(const algo al) {
    return al ? al->cost_fn : NULL;
}

heuristic_func algo_get_heuristic_func(const algo al) {
    return al ? al->heuristic_fn : NULL;
}

gpointer algo_get_userdata(const algo al) {
    return al ? al->userdata : NULL;
}

gpointer algo_get_algo_specific(const algo al) {
    return al ? al->algo_specific : NULL;
}

const map algo_get_map(const algo al) {
    return al ? al->m : NULL;
}

const gchar* algo_get_name(route_algotype_t pa) {
    switch (pa) {
        case PATH_ALGO_BFS: return "bfs";
        case PATH_ALGO_DFS: return "dfs";
        case PATH_ALGO_DIJKSTRA: return "dijkstra";
        case PATH_ALGO_ASTAR: return "astar";
        case PATH_ALGO_WEIGHTED_ASTAR: return "weighted_astar";
        case PATH_ALGO_GREEDY_BEST_FIRST: return "greedy_best_first";
        case PATH_ALGO_BIDIRECTIONAL_ASTAR: return "bidirectional_astar";
        case PATH_ALGO_ANY_ANGLE_ASTAR: return "any_angle_astar";
        case PATH_ALGO_THETA_STAR: return "theta_star";
        case PATH_ALGO_JUMP_POINT_SEARCH: return "jump_point_search";
        case PATH_ALGO_DSTAR_LITE: return "dstar_lite";
        case PATH_ALGO_DYNAMIC_ASTAR: return "dynamic_astar";
        case PATH_ALGO_FRINGE_SEARCH: return "fringe_search";
        case PATH_ALGO_FAST_MARCHING: return "fast_marching";
        default: return "unknown";
    }
}

// route algo_find(const algo al, const coord start, const coord goal) {
//     if (!al) return NULL;

//     switch (al->algotype) {
//         case PATH_ALGO_BFS:
//             return bfs_find(al, start, goal);
//         case PATH_ALGO_DFS:
//             return dfs_find(al, start, goal);
//         case PATH_ALGO_DIJKSTRA:
//             return dijkstra_find(al, start, goal);
//         case PATH_ALGO_ASTAR:
//             return astar_find(al, start, goal);
//         case PATH_ALGO_WEIGHTED_ASTAR:
//             return weighted_astar_find(al, start, goal);
//         default:
//             return NULL;
//     }
// }

// route algo_find_simple(const map m, const coord start, const coord goal) {
//     if (!m) return NULL;

//     // algo al = algo_new_full(m->width, m->height, m->mode, PATH_ALGO_BFS);
//     algo al = algo_new_full(m->width, m->height, m->mode, PATH_ALGO_BFS,
//         bfs_find,
//         default_cost, default_heuristic, NULL, NULL, FALSE);    
//     route p = algo_find(al, start, goal);
//     algo_free(al);
//     return p;
// }

// void algo_reset(algo al) {
//     if (!al) return;

//     if (al->visited) {
//         g_hash_table_remove_all(al->visited);
//     }

//     if (al->came_from) {
//         g_hash_table_remove_all(al->came_from);
//     }

//     if (al->cost_so_far) {
//         g_hash_table_remove_all(al->cost_so_far);
//     }
// }

void algo_reset(algo al) {
    if (!al) return;

    if (al->m) {
        // map을 해제하고 새로 설정해야한다.
        // map_free(al->m);

    }

    if (al->visited) {
        g_hash_table_destroy(al->visited);
        al->visited = g_hash_table_new_full(
            (GHashFunc)       coord_hash,
            (GEqualFunc)      coord_equal,
            (GDestroyNotify)  coord_free,
            NULL
        );
    }

    if (al->came_from) {
        g_hash_table_destroy(al->came_from);
        al->came_from = g_hash_table_new_full(
            (GHashFunc)       coord_hash,
            (GEqualFunc)      coord_equal,
            (GDestroyNotify)  coord_free,
            (GDestroyNotify)  coord_free
        );
    }

    if (al->cost_so_far) {
        g_hash_table_destroy(al->cost_so_far);
        al->cost_so_far = g_hash_table_new_full(
            (GHashFunc)       coord_hash,
            (GEqualFunc)      coord_equal,
            (GDestroyNotify)  coord_free,
            g_free
        );
    }

    if (al->frontier) {
        g_queue_free_full((GQueue*)al->frontier, (GDestroyNotify)coord_free);
        al->frontier = g_queue_new();
    }
}

// ---------------------
// frontier (탐색 큐)
// ---------------------

// void algo_push_frontier(algo al, const coord c, gfloat priority) {
//     if (!al || !c) return;
//     coord_pq item = g_malloc(sizeof(coord_pq_t));
//     item->c = coord_copy(c);
//     item->priority = priority;

//     al->frontier = g_list_insert_sorted(
//         (GList*)al->frontier, item, coord_pq_compare);
// }

void algo_push_frontier(algo al, const coord c, gfloat priority) {
    if (!al || !c) return;

    // 중복 좌표 방지: 이미 같은 좌표가 있으면 삽입하지 않음
    for (GList* l = al->frontier; l; l = l->next) {
        coord_pq existing = l->data;
        if (coord_equal(existing->c, c)) {
            return;
        }
    }

    coord_pq item = g_malloc(sizeof(coord_pq_t));
    item->c = coord_copy(c);
    item->priority = priority;

    al->frontier = g_list_insert_sorted(
        (GList*)al->frontier, item, coord_pq_compare);
}

coord algo_pop_frontier(algo al) {
    if (!al || !al->frontier) return NULL;
    GList* list = (GList*)al->frontier;
    coord_pq item = (coord_pq)list->data;
    coord c = item->c;
    g_free(item);
    al->frontier = g_list_delete_link(list, list);
    return c;  // caller가 coord_free() 해야 함
}

// void algo_push_frontier(algo al, const coord c, gfloat priority) {
//     if (!al || !c) return;

//     coord_pq item = g_malloc(sizeof(coord_pq_t));
//     item->c = coord_copy(c);
//     item->priority = priority;  // 일반 큐에서는 사용 안 하지만 남겨둠

//     g_queue_push_tail((GQueue*)al->frontier, item);
// }

// coord algo_pop_frontier(algo al) {
//     if (!al || !al->frontier) return NULL;

//     coord_pq item = (coord_pq)g_queue_pop_head((GQueue*)al->frontier);
//     if (!item) return NULL;

//     coord c = item->c;
//     g_free(item);
//     return c;  // caller가 coord_free() 해야 함
// }

void algo_append_frontier(algo al, const coord c) {
    if (!al || !al->frontier || !c) return;
    g_queue_push_tail((GQueue*)al->frontier, coord_copy(c));
}

void algo_prepend_frontier(algo al, const coord c) {
    if (!al || !al->frontier || !c) return;
    g_queue_push_head((GQueue*)al->frontier, coord_copy(c));
}

coord algo_pop_frontier_head(algo al) {
    if (!al || !al->frontier) return NULL;
    return (coord)g_queue_pop_head((GQueue*)al->frontier);
}

gboolean algo_is_frontier_empty(const algo al) {
    if (!al || !al->frontier) return TRUE;

    switch(al->frontier_type) {
    case FRONTIER_QUEUE : return g_queue_is_empty((GQueue*)al->frontier);
    case FRONTIER_PRIORQ : return g_list_length((GList*)al->frontier) == 0;
    default : return TRUE;
    } 
}

// void algo_frontier_clear(algo al) {
//     if (!al || !al->frontier) return;
//     g_list_free_full((GList*)al->frontier, coord_pq_free);
//     al->frontier = NULL;
// }

// void     algo_free_frontier(algo al){
//     if(!al || !al->frontier) return;

//     g_queue_free_full((GQueue*)al->frontier, (GDestroyNotify)coord_free);    
// }

void     algo_free_frontier(algo al){
    if(!al || !al->frontier) return;

    switch(al->frontier_type) {
    case FRONTIER_QUEUE : 
        g_queue_free_full((GQueue*)al->frontier, (GDestroyNotify)coord_free);
        break;
    case FRONTIER_PRIORQ : 
        g_list_free_full((GList*)al->frontier, 
        (GDestroyNotify) coord_pq_free);
        al->frontier = NULL;
        break;
    default : 
        break;
    }    
}

void algo_trim_frontier(algo al) {
    if (!al || !al->frontier) return;

    switch (al->frontier_type) {
        case FRONTIER_QUEUE: {
            // 일반 큐: GQueue → tail 제거 (FIFO)
            GQueue* q = (GQueue*)al->frontier;
            coord_pq_t* oldest = g_queue_pop_tail(q);
            if (oldest) {
                coord_free(oldest->c);
                g_free(oldest);
            }
            break;
        }

        case FRONTIER_PRIORQ: {
            // 우선순위 큐: 가장 f값이 큰 노드 제거
            GList* list = (GList*)al->frontier;
            GList* max_node = NULL;
            gfloat max_f = -FLT_MAX;

            for (GList* l = list; l; l = l->next) {
                coord_pq_t* item = l->data;
                if (item->priority > max_f) {
                    max_f = item->priority;
                    max_node = l;
                }
            }

            if (max_node) {
                coord_pq_t* removed = max_node->data;
                al->frontier = g_list_delete_link(list, max_node);
                coord_free(removed->c);
                g_free(removed);
            }
            break;
        }

        default:
            break;
    }
}

gint algo_frontier_size(const algo al) {
    if (!al || !al->frontier) return 0;

    switch (al->frontier_type) {
        case FRONTIER_QUEUE:
            return g_queue_get_length((GQueue*)al->frontier);

        case FRONTIER_PRIORQ:
            return g_list_length((GList*)al->frontier);

        default:
            return 0;
    }
}

gboolean algo_remove_frontier(algo al, gpointer coord_or_coord_pq) {
    if (!al || !al->frontier) return TRUE;

    switch(al->frontier_type) {
    case FRONTIER_QUEUE : 
        return g_queue_remove((GQueue*) al->frontier, coord_or_coord_pq);

    case FRONTIER_PRIORQ : 
        al->frontier = (GList*)g_list_remove((GList*)al->frontier, 
            coord_or_coord_pq);

        return TRUE;
    default : return FALSE;
    } 
}

// ---------------------
// came_from (경로 추적)
// ---------------------

void algo_insert_came_from(algo al, const coord start, const coord goal) {
    if (!al || !al->came_from || !start || !goal) return;
    g_hash_table_replace(al->came_from, coord_copy(start), coord_copy(goal));
}

gboolean algo_contains_came_from(const algo al, const coord key) {
    if (!al || !al->came_from || !key) return FALSE;
    return g_hash_table_contains(al->came_from, key);
}

coord algo_lookup_came_from(const algo al, const coord key) {
    if (!al || !al->came_from || !key) return NULL;
    return (coord)g_hash_table_lookup(al->came_from, key);
}

// ---------------------
// visited (탐색 여부)
// ---------------------

gboolean algo_contains_visited(const algo al, const coord c) {
    if (!al || !al->visited || !c) return FALSE;
    return g_hash_table_contains(al->visited, c);
}

void algo_add_visited(algo al, const coord c) {
    if (!al || !al->visited || !c) return;
    g_hash_table_add(al->visited, coord_copy(c));
}

// ---------------------
// cost_so_far (누적 비용)
// ---------------------

void algo_set_cost_so_far(algo al, const coord c, gfloat cost) {
    if (!al || !al->cost_so_far || !c) return;
    gfloat* val = g_memdup2(&cost, sizeof(gfloat));
    g_hash_table_replace(al->cost_so_far, coord_copy(c), val);
}

gboolean algo_get_cost_so_far(const algo al, const coord c, gfloat* out) {
    if (!al || !al->cost_so_far || !c || !out) return FALSE;
    gpointer val = g_hash_table_lookup(al->cost_so_far, c);
    if (!val) return FALSE;
    *out = *(gfloat*)val;
    return TRUE;
}

// ---------------------
// coord 리스트 조작 (역추적)
// ---------------------

GList* prepend_coord_to_list(GList* list, const coord c) {
    if (!c) return list;
    return g_list_prepend(list, coord_copy(c));
}

void free_coord_list(GList* list) {
    g_list_free_full(list, (GDestroyNotify)coord_free);
}

frontier_type_t get_frontier_type(route_algotype_t algotype) {
    switch(algotype) {
        case PATH_ALGO_BFS : return FRONTIER_QUEUE;
        case PATH_ALGO_DFS : return FRONTIER_QUEUE;
        case PATH_ALGO_DIJKSTRA : return FRONTIER_PRIORQ;
        case PATH_ALGO_ASTAR : return FRONTIER_PRIORQ;
        case PATH_ALGO_WEIGHTED_ASTAR : return FRONTIER_PRIORQ;
        case PATH_ALGO_GREEDY_BEST_FIRST : return FRONTIER_PRIORQ;
        case PATH_ALGO_IDA_STAR : return FRONTIER_PRIORQ;
        case PATH_ALGO_RTA_STAR : return FRONTIER_PRIORQ;
        case PATH_ALGO_SMA_STAR : return FRONTIER_PRIORQ;
        case PATH_ALGO_FAST_MARCHING : return FRONTIER_PRIORQ;
        case PATH_ALGO_FRINGE_SEARCH : return FRONTIER_PRIORQ;
        case PATH_ALGO_DSTAR_LITE : return FRONTIER_PRIORQ;
        case PATH_ALGO_DYNAMIC_ASTAR : return FRONTIER_PRIORQ;
        case PATH_ALGO_LPA_STAR : return FRONTIER_PRIORQ;
        case PATH_ALGO_HPA_STAR : return FRONTIER_PRIORQ;
        case PATH_ALGO_ANY_ANGLE_ASTAR : return FRONTIER_PRIORQ;
        case PATH_ALGO_ALT : return FRONTIER_PRIORQ;
        case PATH_ALGO_THETA_STAR : return FRONTIER_PRIORQ;
        case PATH_ALGO_LAZY_THETA_STAR : return FRONTIER_PRIORQ;
        case PATH_ALGO_JUMP_POINT_SEARCH : return FRONTIER_PRIORQ;
        case PATH_ALGO_JPS_PLUS : return FRONTIER_PRIORQ;
        case PATH_ALGO_BIDIRECTIONAL_ASTAR : return FRONTIER_PRIORQ;
        default : return FRONTIER_PRIORQ;
    }
}

algo_find_func get_algo_find_func(route_algotype_t algotype) {
    switch(algotype) {
        case PATH_ALGO_BFS : return bfs_find;
        case PATH_ALGO_DFS : return dfs_find;
        case PATH_ALGO_DIJKSTRA : return dijkstra_find;
        case PATH_ALGO_ASTAR : return astar_find;
        case PATH_ALGO_WEIGHTED_ASTAR : return weighted_astar_find;
        case PATH_ALGO_GREEDY_BEST_FIRST : return greedy_best_first_find;
        case PATH_ALGO_IDA_STAR : return ida_star_find;
        case PATH_ALGO_RTA_STAR : return rta_star_find;
        case PATH_ALGO_SMA_STAR : return sma_star_find;
        case PATH_ALGO_FAST_MARCHING : return fast_marching_find;
        case PATH_ALGO_FRINGE_SEARCH : return fringe_search_find;
        // case PATH_ALGO_DSTAR_LITE : return dstar_lite_find;
        case PATH_ALGO_DYNAMIC_ASTAR : return NULL;
        case PATH_ALGO_LPA_STAR : return NULL;
        case PATH_ALGO_HPA_STAR : return NULL;
        case PATH_ALGO_ANY_ANGLE_ASTAR : return NULL;
        case PATH_ALGO_ALT : return NULL;
        case PATH_ALGO_THETA_STAR : return NULL;
        case PATH_ALGO_LAZY_THETA_STAR : return NULL;
        case PATH_ALGO_JUMP_POINT_SEARCH : return NULL;
        case PATH_ALGO_JPS_PLUS : return NULL;
        case PATH_ALGO_BIDIRECTIONAL_ASTAR : return NULL;
        default : return NULL;
    }
}

void algo_reconstruct_route(const algo al, route result, coord start, coord goal) {
    if (!al || !result || !start || !goal) return;

    GList* reversed = NULL;
    coord current = goal;

    while (!coord_equal(current, start)) {
        reversed = prepend_coord_to_list(reversed, current);
        coord prev = algo_lookup_came_from(al, current);
        if (!prev) break;
        current = prev;
    }

    reversed = prepend_coord_to_list(reversed, start);

    for (GList* l = reversed; l; l = l->next)
        route_add_coord(result, l->data);

    free_coord_list(reversed);
}

coord_pq coord_pq_new(void) {
    coord_pq cp = g_malloc0(sizeof(coord_pq_t));
    cp->c = NULL;
    cp->priority = 0.0f;
    return cp;
}

coord_pq coord_pq_new_full(const coord c, gfloat prior) {
    if (!c) return NULL;

    coord_pq cp = g_malloc(sizeof(coord_pq_t));
    cp->c = coord_copy(c);  // 안전하게 복사
    cp->priority = prior;
    return cp;
}

GList* append_coord_pq_to_list(GList* gl, coord_pq cp) {
    if (!gl || !cp) return NULL;
    coord_pq new_cp = g_malloc(sizeof(coord_pq_t));
    new_cp->c = coord_copy(cp->c);
    new_cp->priority = cp->priority;
    return g_list_append(gl, new_cp);
}

void coord_pq_free(coord_pq cp) {
    if (!cp) return;
    coord_free(cp->c);
    g_free(cp);
}

gint coord_pq_compare(gconstpointer coord_pq_a, gconstpointer coord_pq_b) {
    const coord_pq pa = (const coord_pq) coord_pq_a;
    const coord_pq pb = (const coord_pq) coord_pq_b;
    return (pa->priority > pb->priority) - (pa->priority < pb->priority);
}
