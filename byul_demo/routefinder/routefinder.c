// routefinder.c
#include "routefinder.h"
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


static algo get_bfs_singleton(const map_t* m) {
    static algo singleton = NULL;
    if (!singleton) {
        singleton = algo_new_full(
            0, 0, 
            MAP_NEIGHBOR_8,
            PATH_ALGO_BFS,
            NULL,
            NULL,
            NULL,
            NULL,
            TRUE
        );
    }
    return singleton;
}

route_t* find_bfs(const map_t* m, const coord_t* start, const coord_t* goal) {
    return algo_find(get_bfs_singleton(), start, goal);
}

static algo get_dfs_singleton(const map_t* m) {
    static algo singleton = NULL;
    if (!singleton) {
        singleton = algo_new_full(
            0, 0, 
            MAP_NEIGHBOR_8,
            PATH_ALGO_DFS,
            NULL,
            NULL,
            NULL,
            NULL,
            TRUE
        );
    }
    return singleton;
}

route_t* find_dfs(const map_t* m, const coord_t* start, const coord_t* goal) {
    return algo_find(get_dfs_singleton(), start, goal);
}

static algo get_dijkstra_singleton(const map_t* m) {
    static algo singleton = NULL;
    if (!singleton) {
        singleton = algo_new_full(
            0, 0, 
            MAP_NEIGHBOR_8,
            PATH_ALGO_DIJKSTRA,
            default_cost,
            default_heuristic,
            NULL,
            NULL,
            TRUE
        );
    }
    return singleton;
}

route_t* find_dijkstra(const map_t* m, const coord_t* start, const coord_t* goal) {
    return algo_find(get_dijkstra_singleton(), start, goal);
}

static algo get_fast_marching_singleton(const map_t* m) {
    static algo singleton = NULL;
    if (!singleton) {
        singleton = algo_new_full(
            0, 0, 
            MAP_NEIGHBOR_8,
            PATH_ALGO_FAST_MARCHING,
            default_cost,
            NULL,  // 휴리스틱 없음
            NULL,
            NULL,
            TRUE
        );
    }
    return singleton;
}

route_t* find_fast_marching(const map_t* m, const coord_t* start, const coord_t* goal) {
    return algo_find(get_fast_marching_singleton(), start, goal);
}

static algo get_fringe_search_singleton(const map_t* m) {
    static algo singleton = NULL;
    if (!singleton) {
        singleton = algo_new_full(
            0, 0, 
            MAP_NEIGHBOR_8,
            PATH_ALGO_FRINGE_SEARCH,
            default_cost,
            default_heuristic,
            NULL,
            NULL,
            TRUE
        );
    }
    return singleton;
}

route_t* find_fringe_search(const map_t* m, const coord_t* start, const coord_t* goal) {
    return algo_find(get_fringe_search_singleton(), start, goal);
}

static algo get_greedy_best_first_singleton(const map_t* m) {
    static algo singleton = NULL;
    if (!singleton) {
        singleton = algo_new_full(
            0, 0, 
            MAP_NEIGHBOR_8,
            PATH_ALGO_GREEDY_BEST_FIRST,
            NULL,                // 비용 함수 없음
            default_heuristic,  // h(x)만 사용
            NULL,
            NULL,
            TRUE
        );
    }
    return singleton;
}

route_t* find_greedy_best_first(const map_t* m, const coord_t* start, const coord_t* goal) {
    return algo_find(get_greedy_best_first_singleton(), start, goal);
}

static algo get_ida_star_singleton(const map_t* m) {
    static algo singleton = NULL;
    if (!singleton) {
        singleton = algo_new_full(
            0, 0, 
            MAP_NEIGHBOR_8,
            PATH_ALGO_IDA_STAR,
            default_cost,
            default_heuristic,
            NULL,
            NULL,
            TRUE
        );
    }
    return singleton;
}

route_t* find_ida_star(const map_t* m, const coord_t* start, const coord_t* goal) {
    return algo_find(get_ida_star_singleton(), start, goal);
}

route_t* find_rta_star(const map_t* m, const coord_t* start, const coord_t* goal, int depth_limit) {
    rta_star_config cfg = rta_star_config_new_full(depth_limit);

    algo al = algo_new_full(
        0, 0, 
        MAP_NEIGHBOR_8,
        PATH_ALGO_RTA_STAR,
        default_cost,
        default_heuristic,
        NULL,
        cfg,
        TRUE
    );

    route_t* r = algo_find(al, start, goal);
    algo_free(al); // 동적 생성된 algo 해제
    return r;
}

route_t* find_sma_star(const map_t* m, 
    const coord_t* start, const coord_t* goal, int memory_limit) {
    sma_star_config cfg = sma_star_config_new_full(memory_limit);

    algo al = algo_new_full(
        0, 0, 
        MAP_NEIGHBOR_8,
        PATH_ALGO_SMA_STAR,
        default_cost,
        default_heuristic,
        NULL,
        cfg,
        TRUE
    );

    route_t* r = algo_find(al, start, goal);
    algo_free(al);
    return r;
}

route_t* find_weighted_astar(const map_t* m, 
    const coord_t* start, const coord_t* goal, float weight) {
    weighted_astar_config cfg = weighted_astar_config_new_full(weight);

    algo al = algo_new_full(
        0, 0, 
        MAP_NEIGHBOR_8,
        PATH_ALGO_WEIGHTED_ASTAR,
        default_cost,
        default_heuristic,
        NULL,
        cfg,
        TRUE
    );

    route_t* r = algo_find(al, start, goal);
    algo_free(al);
    return r;
}

// 내부 static 싱글턴 A* 알고리즘 인스턴스
static algo get_astar_singleton(const map_t* m) {
    static algo singleton = NULL;
    if (!singleton) {
        singleton = algo_new_full(
            0, 0,                      // width, height
            MAP_NEIGHBOR_8,              // 8방향 탐색
            PATH_ALGO_ASTAR,             // 알고리즘 종류
            default_cost,                // 비용 함수
            default_heuristic,         // 휴리스틱 함수
            NULL,                        // 사용자 데이터
            NULL,                        // 사용자 해제 함수
            TRUE                         // visited logging 여부
        );
    }
    return singleton;
}

route_t* find_astar(const map_t* m, const coord_t* start, const coord_t* goal) {
    return algo_find(get_astar_singleton(), start, goal);
}
