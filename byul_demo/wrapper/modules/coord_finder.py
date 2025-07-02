from ffi_core import ffi, C
from coord import c_coord
from typing import Callable

# C 함수 시그니처 정의
ffi.cdef("""
#define MAX_RANGE_LIMIT 256

// typedef struct s_coord coord_t;
// typedef coord_t* coord;

// 도달 가능성 판단 함수
typedef gboolean (*is_reeachable_func)(const coord c, gpointer user_data);

// BFS 기반 도달 가능 좌표 탐색
gboolean find_goal_bfs(const coord start,
                       is_reeachable_func is_reachable_fn,
                       gpointer user_data,
                       gint max_range,
                       coord* out_result);

// A* 기반 도달 가능 좌표 탐색
gboolean find_goal_astar(const coord start,
                         is_reeachable_func is_reachable_fn,
                         gpointer user_data,
                         gint max_range,
                         coord* out_result);

// (선택) 비교 함수 — A* 노드 정렬에 사용
gint astar_node_compare(gconstpointer a, gconstpointer b, gpointer user_data);
""")

# Python 래퍼 클래스
class c_coord_finder:
    @staticmethod
    def _wrap_is_reachable(py_func: Callable[[c_coord], bool]):
        @ffi.callback("gboolean(const coord, gpointer)")
        def c_callback(c_coord_raw, user_data):
            # c_coord_raw는 이미 coord_t* (즉, 포인터)임
            coord = c_coord(raw_ptr=c_coord_raw)
            return C.TRUE if py_func(coord) else C.FALSE
        return c_callback

    @staticmethod
    def find_goal_bfs(start: c_coord,
                      is_reachable_fn: Callable[[c_coord], bool],
                      max_range: int = 5) -> c_coord | None:
        """
        BFS 방식으로 가장 가까운 reachable 셀 탐색
        """
        out_result = ffi.new("coord*")
        c_fn = c_coord_finder._wrap_is_reachable(is_reachable_fn)

        found = C.find_goal_bfs(start.ptr(), c_fn, ffi.NULL, max_range, out_result)
        if found:
            return c_coord(raw_ptr=C.coord_copy(out_result[0]))
        return None

    @staticmethod
    def find_goal_astar(start: c_coord,
                        is_reachable_fn: Callable[[c_coord], bool],
                        max_range: int = 5) -> c_coord | None:
        """
        A* 기반으로 가장 가까운 reachable 셀 탐색
        """
        out_result = ffi.new("coord*")
        c_fn = c_coord_finder._wrap_is_reachable(is_reachable_fn)

        found = C.find_goal_astar(start.ptr(), c_fn, ffi.NULL, max_range, out_result)
        if found:
            return c_coord(raw_ptr=C.coord_copy(out_result[0]))
        return None
