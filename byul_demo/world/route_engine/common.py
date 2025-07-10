# Base classes for route engines
from typing import Callable, Any, Optional
from map import c_map

class RouteRequest:
    """Common request parameters for different route engines."""

    def __init__(self,
                #  map_ptr: Any,
                map:c_map,
                 npc_id: str,
                 start: tuple,
                 goal: tuple,
                 on_route_found_cb: Callable,
                 type: Any = None,
                 move_cb: Optional[Callable] = None,
                 interval_msec: int = 100,
                 max_retry: int = 10000,
                 visited_logging: bool = False,
                 cost_func_name: str = "default",
                 heuristic_func_name: str = "euclidean",
                 userdata: Any = None,
                 on_real_route_found_cb: Optional[Callable] = None):
        # self.map_ptr = map_ptr
        self.map = map
        self.npc_id = npc_id
        self.start = start
        self.goal = goal
        self.on_route_found_cb = on_route_found_cb
        self.type = type
        self.move_cb = move_cb
        self.interval_msec = interval_msec
        self.max_retry = max_retry
        self.visited_logging = visited_logging
        self.cost_func_name = cost_func_name
        self.heuristic_func_name = heuristic_func_name
        self.userdata = userdata
        self.on_real_route_found_cb = on_real_route_found_cb

class RouteResult:
    def __init__(self, npc_id: str, route: 'c_route'):
        self.npc_id = npc_id
        self.route = route
