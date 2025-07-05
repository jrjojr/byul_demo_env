from pathlib import Path
import sys

g_root_path = Path('C:/Users/critl/docs/byul_demo_env/byul_demo')
wrapper_path = g_root_path / Path("wrapper/modules")

sys.path.insert(0, str(wrapper_path.resolve()))

import unittest

from coord import c_coord
from map import c_map, MapNeighborMode
from algo import c_algo, RouteAlgotype
from algo_utils import c_algo_utils

class TestAlgo(unittest.TestCase):
    def setUp(self):
        self.map = c_map(width=10, height=10, mode=MapNeighborMode.DIR_8)
        self.algo = c_algo(self.map, visited_logging=True)

        self.start = c_coord(0, 0)
        self.goal = c_coord(9, 9)
        self.algo.set_start(self.start)
        self.algo.set_goal(self.goal)

        # // 장애물 삽입 (세로 차단)
        for i in range(1, 10):
            self.map.block(5, i)

    def tearDown(self):
        self.algo.close()

    # def test_find_default(self):
    #     route = self.algo.find()
    #     route.print()
    #     c_algo_utils.map_print_with_visited(self.map, route)

    def test_find_bfs(self):
        self.algo.set_type(RouteAlgotype.BFS)
        route = self.algo.find()
        route.print()
        c_algo_utils.map_print_with_visited(self.map, route)

    def test_find_astar(self):
        self.algo.set_type(RouteAlgotype.ASTAR)
        route = self.algo.find()
        route.print()
        c_algo_utils.map_print_with_visited(self.map, route)        

    def test_find_dfs(self):
        #include "internal/dfs.h"
        self.algo.set_type(RouteAlgotype.DFS)
        route = self.algo.find()
        route.print()
        c_algo_utils.map_print_with_visited(self.map, route)        

    # def test_find_dijkstra(self):
    #     #include "internal/dijkstra.h"
    #     self.algo.set_type(RouteAlgotype.DIJKSTRA)
    #     route = self.algo.find()
    #     route.print()
    #     c_algo_utils.map_print_with_visited(self.map, route)        

    # def test_find_fast_marching(self):
    #     #include "internal/fast_marching.h"
    #     self.algo.set_type(RouteAlgotype.FAST_MARCHING)
    #     route = self.algo.find()
    #     route.print()
    #     c_algo_utils.map_print_with_visited(self.map, route)        

    # def test_find_fringe_search(self):
    #     #include "internal/fringe_search.h"
    #     self.algo.set_type(RouteAlgotype.FRINGE_SEARCH)
    #     route = self.algo.find()
    #     route.print()
    #     c_algo_utils.map_print_with_visited(self.map, route)        

    # def test_find_greedy_best_first(self):
    #     #include "internal/greedy_best_first.h"
    #     self.algo.set_type(RouteAlgotype.GREEDY_BEST_FIRST)
    #     route = self.algo.find()
    #     route.print()
    #     c_algo_utils.map_print_with_visited(self.map, route)        

    # def test_find_ida_star(self):
    #     #include "internal/ida_star.h"
    #     self.algo.set_type(RouteAlgotype.IDA_STAR)
    #     route = self.algo.find()
    #     route.print()
    #     c_algo_utils.map_print_with_visited(self.map, route)        

    # def test_find_rta_star(self):
    #     #include "internal/rta_star.h"
    #     self.algo.set_type(RouteAlgotype.RTA_STAR)
    #     route = self.algo.find()
    #     route.print()
    #     c_algo_utils.map_print_with_visited(self.map, route)        

    # def test_find_sma_star(self):
    #     #include "internal/sma_star.h"
    #     self.algo.set_type(RouteAlgotype.SMA_STAR)
    #     route = self.algo.find()
    #     route.print()
    #     c_algo_utils.map_print_with_visited(self.map, route)        

    # def test_find_weighted_astar(self):
    #     #include "internal/weighted_astar.h"        
    #     self.algo.set_type(RouteAlgotype.WEIGHTED_ASTAR)
    #     route = self.algo.find()
    #     route.print()
    #     c_algo_utils.map_print_with_visited(self.map, route)                

# 🔽 여기서부터 직접 실행 시 동작
if __name__ == '__main__':
    unittest.main()