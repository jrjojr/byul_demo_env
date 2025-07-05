from ffi_core import ffi, C
from pathlib import Path
import os
import platform

from coord import c_coord
from coord_list import c_coord_list
from coord_hash import c_coord_hash
from route import c_route
from map import c_map

from dstar_lite_pqueue import c_dstar_lite_pqueue

from dstar_lite import c_dstar_lite

ffi.cdef("""
// ------------------ 디버그용 테이블 출력 ------------------

/// @brief g 테이블 출력 (좌표별 g값)
BYUL_API void dsl_debug_print_g_table(const map_t* m, coord_hash_t* g_table);

/// @brief rhs 테이블 출력 (좌표별 rhs값)
BYUL_API void dsl_debug_print_rhs_table(
    const map_t* m, coord_hash_t* rhs_table);

// ------------------ D* Lite 상태 전체 출력 ------------------

/// @brief D* Lite 전체 내부 상태를 출력합니다.
/// @param dsl               D* Lite 객체
/// @param goal             목표 지점
/// @param km               현재 km 값
/// @param g_table          g값 테이블
/// @param rhs_table        rhs값 테이블
/// @param frontier         우선순위 큐
/// @param max_range        탐색 최대 범위
/// @param retry_limit      최대 재시도 횟수
/// @param debug_mode       디버그 모드 여부
/// @param update_counter   update_vertex 횟수 테이블
BYUL_API void dsl_debug_print_full_state(
    const dstar_lite_t* dsl,
    const coord_t* goal,
    float km,
    coord_hash_t* g_table,
    coord_hash_t* rhs_table,
    dstar_lite_pqueue_t* frontier,
    int max_range,
    int retry_limit,
    bool debug_mode,
    coord_hash_t* update_counter);

// ------------------ 간략 출력 ------------------

/// @brief 핵심 변수만 간략히 출력 (g/rhs/priority queue만)
BYUL_API void dsl_debug_print_state(
    const dstar_lite_t* dsl,
    const coord_t* goal,
    float km,
    coord_hash_t* g_table,
    coord_hash_t* rhs_table,
    dstar_lite_pqueue_t* frontier);

// ------------------ ASCII 맵 출력 ------------------

/// @brief 맵만 출력 (`#`, `.`만 표시)
BYUL_API void dsl_print_ascii_only_map(const dstar_lite_t* dsl);

/// @brief 시작, 목표, 경로까지 포함한 맵 출력 (`S`, `G`, `*`, `.`, `#`)
BYUL_API void dsl_print_ascii_route(
    const dstar_lite_t* dsl, const route_t* route, int margin);

/// @brief update_vertex 횟수 포함한 맵 출력 (`S`, `G`, `*`, `#`, 숫자 등)
BYUL_API void dsl_print_ascii_update_count(
    const dstar_lite_t* dsl, const route_t* route, int margin);

""")

class c_dstar_lite_utils:
    # ───── g / rhs 테이블 출력 ─────
    @staticmethod
    def print_g_table(m: c_map, g_table: c_coord_hash):
        C.dsl_debug_print_g_table(m.ptr(), g_table.ptr())

    @staticmethod
    def print_rhs_table(m: c_map, rhs_table: c_coord_hash):
        C.dsl_debug_print_rhs_table(m.ptr(), rhs_table.ptr())

    # ───── D* Lite 전체 상태 출력 ─────
    @staticmethod
    def print_full_state(
        dsl, goal: c_coord, km: float,
        g_table: c_coord_hash, rhs_table: c_coord_hash,
        frontier, max_range: int, retry_limit: int,
        debug_mode: bool, update_counter: c_coord_hash
    ):
        C.dsl_debug_print_full_state(
            dsl.ptr(), goal.ptr(), km,
            g_table.ptr(), rhs_table.ptr(),
            frontier.ptr(), max_range, retry_limit,
            debug_mode, update_counter.ptr()
        )

    @staticmethod
    def print_state(
        dsl, goal: c_coord, km: float,
        g_table: c_coord_hash, rhs_table: c_coord_hash,
        frontier
    ):
        C.dsl_debug_print_state(
            dsl.ptr(), goal.ptr(), km,
            g_table.ptr(), rhs_table.ptr(), frontier.ptr()
        )

    # ───── ASCII 시각화 출력 ─────
    @staticmethod
    def print_ascii_only_map(dsl:c_dstar_lite):
        C.dsl_print_ascii_only_map(dsl.ptr())

    @staticmethod
    def print_ascii_route(dsl:c_dstar_lite, route: c_route, margin: int = 0):
        C.dsl_print_ascii_route(dsl.ptr(), route.ptr(), margin)

    @staticmethod
    def print_ascii_update_count(dsl:c_dstar_lite, 
        route: c_route, margin: int = 0):
        
        C.dsl_print_ascii_update_count(dsl.ptr(), route.ptr(), margin)
