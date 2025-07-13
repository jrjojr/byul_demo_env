from ffi_core import ffi, C

from map import c_map
from route import c_route

ffi.cdef("""
/**
 * @brief 맵을 ASCII 형태로 출력합니다.
 *
 * 차단된 좌표는 `#`, 그 외는 `.`으로 출력되며,
 * 경로, 시작/도착 지점은 표시되지 않습니다.
 *
 * @param m 출력할 맵
 */
void map_print_ascii(const map_t* m);

/**
 * @brief 경로 정보를 포함하여 맵을 ASCII로 출력합니다.
 *
 * 경로는 `*`, 시작점은 `S`, 도착점은 `E`로 표시되며,
 * 차단된 좌표는 `#`, 나머지는 `.`으로 출력됩니다.
 *
 * @param m 맵 객체
 * @param p 경로 객체 (route_get_success(p)가 TRUE인 경우만 표시)
 */
void map_print_ascii_with_route(
    const map_t* m, const route_t* p, int margin);

/**
 * @brief 방문 횟수를 ASCII 맵 형식으로 출력합니다.
 *
 * route_t* 내부의 visited_logging 해시테이블을 기반으로,
 * 각 좌표의 방문 횟수를 1~99 사이의 2자리 숫자로 출력합니다.
 *
 * 출력 형식:
 * - 시작 좌표는 " S"
 * - 도착 좌표는 " E"
 * - 장애물은 "#" (1자리)
 * - 방문된 좌표는 "%2d" (두 자리 숫자, 99 이상은 99로 고정)
 * - 미방문 좌표는 " ."
 *
 * @param m    맵 객체
 * @param p    경로 결과 객체 (route_get_visited_count() 사용)
 */
void map_print_ascii_with_visited_count(
    const map_t* m, const route_t* p, int margin);
""")

class c_route_finder_utils:
    @staticmethod
    def map_print(m: c_map):
        if not isinstance(m, c_map):
            raise TypeError("map_print expects a c_map object")
        C.map_print_ascii(m.ptr())

    @staticmethod
    def map_print_with_route(m: c_map, route: c_route, margin: int = 0):
        if not isinstance(m, c_map) or not isinstance(route, c_route):
            raise TypeError("map_print_with_route expects (c_map, c_route)")
        C.map_print_ascii_with_route(m.ptr(), route.ptr(), margin)

    @staticmethod
    def map_print_with_visited(m: c_map, route: c_route, margin: int = 0):
        if not isinstance(m, c_map) or not isinstance(route, c_route):
            raise TypeError("map_print_with_visited expects (c_map, c_route)")
        C.map_print_ascii_with_visited_count(m.ptr(), route.ptr(), margin)
