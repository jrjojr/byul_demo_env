// algo.cpp

#include "internal/algo.h"
#include "internal/cost_coord_pq.h"
#include "internal/coord_list.h"
#include "coord.hpp"

#include <cmath>
#include <vector>
#include <limits>

#include <cstring>

const char* get_algo_name(route_algotype_t pa) {
    switch (pa) {
        case ROUTE_ALGO_BFS: return "bfs";
        case ROUTE_ALGO_DFS: return "dfs";
        case ROUTE_ALGO_DIJKSTRA: return "dijkstra";
        case ROUTE_ALGO_ASTAR: return "astar";
        case ROUTE_ALGO_WEIGHTED_ASTAR: return "weighted_astar";
        case ROUTE_ALGO_GREEDY_BEST_FIRST: return "greedy_best_first";
        case ROUTE_ALGO_IDA_STAR: return "ida_star";
        case ROUTE_ALGO_RTA_STAR: return "rta_star";
        case ROUTE_ALGO_SMA_STAR: return "sma_star";
        case ROUTE_ALGO_FAST_MARCHING: return "fast_marching";
        case ROUTE_ALGO_FRINGE_SEARCH: return "fringe_search";
        case ROUTE_ALGO_DSTAR_LITE: return "dstar_lite";
        case ROUTE_ALGO_DSTAR: return "dynamic_astar";
        case ROUTE_ALGO_LPA_STAR: return "lpa_star";
        case ROUTE_ALGO_HPA_STAR: return "hpa_star";
        case ROUTE_ALGO_ANY_ANGLE_ASTAR: return "any_angle_astar";
        case ROUTE_ALGO_ALT: return "alt";
        case ROUTE_ALGO_THETA_STAR: return "theta_star";
        case ROUTE_ALGO_LAZY_THETA_STAR: return "lazy_theta_star";
        case ROUTE_ALGO_JUMP_POINT_SEARCH: return "jump_point_search";
        case ROUTE_ALGO_JPS_PLUS: return "jps_plus";
        case ROUTE_ALGO_BIDIRECTIONAL_ASTAR: return "bidirectional_astar";
        default: return "unknown";
    }
}

algo_t* algo_new_full(map_t* map, 
    route_algotype_t type, 
    coord_t* start, coord_t* goal,
    cost_func cost_fn, heuristic_func heuristic_fn,
    int max_retry, bool visited_logging, void* userdata) {

    algo_t* a = new algo_t;
    a->type = type;
    a->map = map;
    a->start = coord_copy(start);
    a->goal = coord_copy(goal);
    a->cost_fn = cost_fn;
    a->heuristic_fn = heuristic_fn;
    a->max_retry = max_retry;
    a->visited_logging = visited_logging;
    a->userdata = userdata;
    return a;
}

algo_t* algo_new(map_t* map) {
    coord_t start;
    start.x = 0;
    start.y = 0;
    return algo_new_full(map, ROUTE_ALGO_ASTAR, &start, &start, 
        default_cost, euclidean_heuristic, 10000, false, nullptr);
}

void algo_free(algo_t* a) {
    if(a->start) coord_free(a->start);
    if(a->goal) coord_free(a->goal);
    delete a;
}

algo_t* algo_copy(const algo_t* src) {
    if (!src) return nullptr;
    return algo_new_full(
        src->map,
        src->type,
        src->start,
        src->goal,
        src->cost_fn,
        src->heuristic_fn,
        src->max_retry,
        src->visited_logging,
        src->userdata
    );
}

void algo_clear(algo_t* a) {
    memset(a, 0, sizeof(algo_t));
}

void algo_set_defaults(algo_t* a) {
    a->cost_fn = default_cost;
    a->heuristic_fn = euclidean_heuristic;
    a->max_retry = 10000;
    a->visited_logging = false;
}

bool algo_is_valid(const algo_t* a) {
    if (!a) return false;
    return a && a->map && a->cost_fn && a->heuristic_fn;
}

void algo_print(const algo_t* a) {
    if (!a) {
        printf("(algo: NULL)\n");
        return;
    }

    printf("algo_t {\n");
    printf("  type:        %s\n", get_algo_name(a->type));
    printf("  map:         %p\n", (void*)a->map);
    printf("  start:       (%d, %d)\n", a->start->x, a->start->y);
    printf("  goal:        (%d, %d)\n", a->goal->x, a->goal->y);
    printf("  cost_fn:     %p\n", (void*)a->cost_fn);
    printf("  heuristic_fn:%p\n", (void*)a->heuristic_fn);
    printf("  max_retry:   %d\n", a->max_retry);
    printf("  logging:     %s\n", a->visited_logging ? "true" : "false");
    printf("  userdata:    %p\n", a->userdata);
    printf("}\n");
}

void algo_set_map(algo_t* a, map_t* map) {
     a->map = map; 
}

void algo_set_start(algo_t* a, coord_t* start) { 
    a->start = start; 
}

void algo_set_goal(algo_t* a, coord_t* goal) { 
    a->goal = goal; 
}

map_t* algo_get_map(const algo_t* a) { 
    return a->map; 
}

coord_t* algo_get_start(const algo_t* a) { 
    return a->start; 
}

coord_t* algo_get_goal(const algo_t* a) { 
    return a->goal; 
}

void algo_set_userdata(algo_t* a, void* userdata){
    a->userdata = userdata;
}

void* algo_get_userdata(const algo_t* a){
    return a->userdata;
}

void algo_set_type(algo_t* a, route_algotype_t type){
    a->type = type;
}

route_algotype_t algo_get_type(const algo_t* a){
    return a->type;
}

void algo_set_visited_logging(algo_t* a, bool is_logging){
    a->visited_logging = is_logging;
}

bool algo_is_visited_logging(algo_t* a){
    return a->visited_logging;
}

void algo_set_cost_func(algo_t* a, cost_func cost_fn){
    a->cost_fn = cost_fn;
}

cost_func algo_get_cost_func(algo_t* a){
    return a->cost_fn;
}

void algo_set_heuristic_func(algo_t* a, heuristic_func heuristic_fn){
    a->heuristic_fn = heuristic_fn;
}

heuristic_func algo_get_heuristic_func(algo_t* a){
    return a->heuristic_fn;
}

void algo_set_max_retry(algo_t* a, int max_retry){
    a->max_retry;
}

int algo_get_max_retry(algo_t* a){
    return a->max_retry;
}

route_t* algo_find_with_type(algo_t* a, route_algotype_t type) {
    if (!a) return NULL;
    switch (type) {
        case ROUTE_ALGO_ASTAR: return algo_find_astar(a);
        case ROUTE_ALGO_BFS: return algo_find_bfs(a);
        case ROUTE_ALGO_DFS: return algo_find_dfs(a);
        case ROUTE_ALGO_DIJKSTRA: return algo_find_dijkstra(a);
        case ROUTE_ALGO_FAST_MARCHING: return algo_find_fast_marching(a);
        case ROUTE_ALGO_FRINGE_SEARCH: return algo_find_fringe_search(a);
        case ROUTE_ALGO_GREEDY_BEST_FIRST: return algo_find_greedy_best_first(a);
        case ROUTE_ALGO_IDA_STAR: return algo_find_ida_star(a);
        case ROUTE_ALGO_RTA_STAR: return algo_find_rta_star(a);
        case ROUTE_ALGO_SMA_STAR: return algo_find_sma_star(a);
        case ROUTE_ALGO_WEIGHTED_ASTAR: return algo_find_weighted_astar(a);
        default: return NULL;
    }
}

route_t* algo_find(algo_t* a) {
    if (!a) return NULL;
    return algo_find_with_type(a, a->type);
}

route_t* algo_find_astar(algo_t* a){
    return find_astar(a->map, a->start, a->goal, 
        a->cost_fn, a->heuristic_fn, a->max_retry, a->visited_logging);
}

route_t* algo_find_bfs(algo_t* a){
    return find_bfs(a->map, a->start, a->goal, 
        a->max_retry, a->visited_logging);
}

route_t* algo_find_dfs(algo_t* a){
    return find_dfs(a->map, a->start, a->goal, a->max_retry,
        a->visited_logging);
}

route_t* algo_find_dijkstra(algo_t* a){
    return find_dijkstra(a->map, a->start, a->goal, a->cost_fn,
        a->max_retry, a->visited_logging);
}

route_t* algo_find_fringe_search(algo_t* a) {
    float delta_epsilon = 0.3f;

    if (a->userdata) {
        float v = *(float*)a->userdata;
        if (v >= 0.001f && v <= 5.0f) {
            delta_epsilon = v;
        }
    }

    return find_fringe_search(a->map, a->start, a->goal,
        a->cost_fn, a->heuristic_fn, delta_epsilon,
        a->max_retry, a->visited_logging);
}

route_t* algo_find_greedy_best_first(algo_t* a){
    return find_greedy_best_first(a->map, a->start, a->goal,
    a->heuristic_fn, a->max_retry, a->visited_logging);
}

route_t* algo_find_ida_star(algo_t* a){
    // ida는 유클리드가 아니라 맨하탄으로 휴리스틱을 설정해야 한다.
    algo_set_heuristic_func(a, manhattan_heuristic);
    return find_ida_star(a->map, a->start, a->goal,
    a->cost_fn, a->heuristic_fn, a->max_retry, a->visited_logging);
}

route_t* algo_find_rta_star(algo_t* a) {
    int depth_limit = 5;

    if (a->userdata) {
        int v = *(int*)a->userdata;
        if (v >= 1 && v <= 100) {
            depth_limit = v;
        }
    }

    return find_rta_star(a->map, a->start, a->goal,
        a->cost_fn, a->heuristic_fn, depth_limit,
        a->max_retry, a->visited_logging);
}

route_t* algo_find_sma_star(algo_t* a) {
    if (!a || !a->map) return NULL;

    int memory_limit = 0;

    if (a->userdata) {
        int val = *(int*)a->userdata;
        // 유효 범위 검사 (적당한 하한선 및 상한선 예시)
        if (val >= 10 && val <= 1000000) {
            memory_limit = val;
        }
    }

    // 비정상적인 값이면 맵 크기로 기본값 계산
    if (memory_limit <= 0) {
        int w = map_get_width(a->map);
        int h = map_get_height(a->map);
        int n = w * h;

        // 기본 권장값: α = 0.02
        memory_limit = (int)(n * 0.02f);
        if (memory_limit < 20) memory_limit = 20; // 최소 한도
    }

    return find_sma_star(a->map, a->start, a->goal,
        a->cost_fn, a->heuristic_fn, memory_limit,
        a->max_retry, a->visited_logging);
}

route_t* algo_find_weighted_astar(algo_t* a) {
    float weight = 1.5f;

    if (a->userdata) {
        float v = *(float*)a->userdata;
        if (v >= 0.1f && v <= 10.0f) {
            weight = v;
        }
    }

    return find_weighted_astar(a->map, a->start, a->goal,
        a->cost_fn, a->heuristic_fn, weight,
        a->max_retry, a->visited_logging);
}

route_t* algo_find_fast_marching(algo_t* a){
    return find_fast_marching(a->map, a->start, a->goal,
    a->cost_fn, a->max_retry, a->visited_logging);
}