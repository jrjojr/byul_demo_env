from ffi_core import ffi, C

from coord import c_coord
from coord_list import c_coord_list
from coord_hash import c_coord_hash

from route import c_route
from map import c_map

import weakref

ffi.cdef("""
         
typedef float (*cost_func)(
    const map_t*, const coord_t*, const coord_t*, void*);

typedef float (*heuristic_func)(const coord_t*, const coord_t*, void*);
         
float default_cost(
    const map_t*, const coord_t*, const coord_t*, void*);         
         
/**
 * @brief 0을 반환하는 비용 함수 (모든 경로 동일 비용)
 */
float zero_cost(const map_t*, const coord_t*, const coord_t*, void*);

/**
 * @brief 대각선 이동 비용 함수 (√2 근사값 사용)
 */
float diagonal_cost(
    const map_t*, const coord_t*, const coord_t*, void*);
         
                 
/**
 * @brief 유클리드 거리 휴리스틱
 */
float euclidean_heuristic(const coord_t*, const coord_t*, void*);

/**
 * @brief 맨해튼 거리 휴리스틱
 */
float manhattan_heuristic(const coord_t*, const coord_t*, void*);
         
/**
 * @brief 체비셰프 거리 휴리스틱
 */
float chebyshev_heuristic(const coord_t*, const coord_t*, void*);
                  
/**
 * @brief 옥타일 거리 휴리스틱 (8방향 이동)
 */
float octile_heuristic(const coord_t*, const coord_t*, void*);

/**
 * @brief 항상 0을 반환하는 휴리스틱 (탐색 최소화용)
 */
float zero_heuristic(const coord_t*, const coord_t*, void*);

/**
 * @brief 기본 휴리스틱 ( 유클리드)
 */
float default_heuristic(const coord_t*, const coord_t*, void*);
                 
""")

class AlgoCommon:
    def __init__(self):
        self._cost_funcs = {}
        self._heuristic_funcs = {}

    def register_cost(self, name: str, func):
        self._cost_funcs[name] = func

    def register_heuristic(self, name: str, func):
        self._heuristic_funcs[name] = func

    def get_cost_func(self, name: str):
        if name not in self._cost_funcs:
            raise ValueError(f"[CostFunc] Unknown function: '{name}'")
        return self._cost_funcs[name]

    def get_heuristic_func(self, name: str):
        if name not in self._heuristic_funcs:
            raise ValueError(f"[HeuristicFunc] Unknown function: '{name}'")
        return self._heuristic_funcs[name]

    def all_cost_names(self):
        return list(self._cost_funcs.keys())

    def all_heuristic_names(self):
        return list(self._heuristic_funcs.keys())

g_AlgoCommon = AlgoCommon()

g_AlgoCommon.register_cost("default", C.default_cost)
g_AlgoCommon.register_cost("zero", C.zero_cost)
g_AlgoCommon.register_cost("diagonal", C.diagonal_cost)

g_AlgoCommon.register_heuristic("euclidean", C.euclidean_heuristic)
g_AlgoCommon.register_heuristic("manhattan", C.manhattan_heuristic)
g_AlgoCommon.register_heuristic("chebyshev", C.chebyshev_heuristic)
g_AlgoCommon.register_heuristic("octile", C.octile_heuristic)
g_AlgoCommon.register_heuristic("zero", C.zero_heuristic)
g_AlgoCommon.register_heuristic("default", C.default_heuristic)
