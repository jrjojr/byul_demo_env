from concurrent.futures import ThreadPoolExecutor
from queue import Queue
from typing import Callable
import threading

from route import c_route
from map import c_map
from algo import c_algo, RouteAlgotype
from coord import c_coord

from utils.log_to_panel import g_logger


class RouteRequest:
    def __init__(self,
                 map_ptr,
                 npc_id: str,
                 type: RouteAlgotype,
                 start: tuple,
                 goal: tuple,
                 callback: Callable,
                 max_retry: int = 10000,
                 visited_logging: bool = False,
                 cost_func: str = "default",
                 heuristic_func: str = "euclidean",
                 userdata: any = None):
        self.map_ptr = map_ptr
        self.npc_id = npc_id
        self.type = type
        self.start = start
        self.goal = goal
        self.callback = callback
        self.max_retry = max_retry
        self.visited_logging = visited_logging
        self.cost_func = cost_func
        self.heuristic_func = heuristic_func
        self.userdata = userdata



class RouteResult:
    def __init__(self, npc_id: str, route: 'c_route'):
        self.npc_id = npc_id
        self.route = route


class RouteEngine:
    def __init__(self, max_workers: int = 4):
        self.executor = ThreadPoolExecutor(max_workers=max_workers)
        self.task_queue = Queue()
        self.running = True
        threading.Thread(target=self._dispatcher_loop, daemon=True).start()

    def _dispatcher_loop(self):
        while self.running:
            request: RouteRequest = self.task_queue.get()
            if request is None:
                break
            self.executor.submit(self._process_request, request)

    def _process_request(self, request: RouteRequest):
        g_logger.log_debug_threadsafe('before 길찾기')

        map_obj = c_map(raw_ptr=request.map_ptr, own=False)
        # userdata는 C 쪽에서 직접 쓰지 않고 복제해서 넘겨라
        safe_userdata = request.userdata if isinstance(request.userdata, (int, float, str)) else None
        self.algo = c_algo(
            map=map_obj,
            type=request.type,
            start=c_coord.from_tuple(request.start),
            goal=c_coord.from_tuple(request.goal),
            cost_fn=c_algo.get_cost_func(request.cost_func),
            heuristic_fn=c_algo.get_heuristic_func(request.heuristic_func),
            max_retry=request.max_retry,
            visited_logging=request.visited_logging,
            userdata=safe_userdata
        )

        route: 'c_route' = self.algo.find()
        g_logger.log_debug_threadsafe('after 길찾기')
        result = RouteResult(request.npc_id, route)
        request.callback(result)

    def submit(self,
               map: c_map,
               npc_id: str,
               type: RouteAlgotype,
               start: tuple,
               goal: tuple,
               callback: Callable,
               max_retry: int = 10000,
               visited_logging: bool = False,
               cost_func: str = "default",
               heuristic_func: str = "euclidean",
               userdata: any = None):
        map_ptr = map.ptr()
        request = RouteRequest(
            map_ptr=map_ptr,
            npc_id=npc_id,
            type=type,
            start=start,
            goal=goal,
            callback=callback,
            max_retry=max_retry,
            visited_logging=visited_logging,
            cost_func=cost_func,
            heuristic_func=heuristic_func,
            userdata=userdata
        )
        self.task_queue.put(request)

    def shutdown(self):
        self.running = False
        self.task_queue.put(None)
        self.executor.shutdown(wait=True)
