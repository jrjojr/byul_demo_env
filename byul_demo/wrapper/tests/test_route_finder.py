from pathlib import Path
import sys

g_root_path = Path('C:/Users/critl/docs/byul_demo_env/byul_demo')
wrapper_path = g_root_path / Path("wrapper/modules")

sys.path.insert(0, str(wrapper_path.resolve()))

import unittest

from coord import c_coord
from map import c_map, MapNeighborMode
from route_finder import c_route_finder, RouteFindertype
from route_finder_utils import c_route_finder_utils

class TestAlgo(unittest.TestCase):
    def setUp(self):
        self.map = c_map(width=10, height=10, mode=MapNeighborMode.DIR_8)
        self.route_finder = c_route_finder(self.map, visited_logging=True)

        self.start = c_coord(0, 0)
        self.goal = c_coord(9, 9)
        self.route_finder.set_start(self.start)
        self.route_finder.set_goal(self.goal)

        # // 장애물 삽입 (세로 차단)
        for i in range(1, 10):
            self.map.block(5, i)

    def tearDown(self):
        self.route_finder.close()

    def test_find_default(self):
        print("test_find_default")
        route = self.route_finder.find()
        route.print()
        c_route_finder_utils.map_print_with_visited(self.map, route)

    def test_find_bfs(self):
        print('test_find_bfs')
        self.route_finder.set_type(RouteFindertype.BFS)
        route = self.route_finder.find()
        route.print()
        c_route_finder_utils.map_print_with_visited(self.map, route)

    def test_find_astar(self):
        print('test_find_astar')
        self.route_finder.set_type(RouteFindertype.ASTAR)
        route = self.route_finder.find()
        route.print()
        c_route_finder_utils.map_print_with_visited(self.map, route)        

    def test_find_dfs(self):
        print('test_find_dfs')
        #include "internal/dfs.h"
        self.route_finder.set_type(RouteFindertype.DFS)
        route = self.route_finder.find()
        route.print()
        c_route_finder_utils.map_print_with_visited(self.map, route)        

    def test_find_dijkstra(self):
        print('test_find_dijkstra')
        #include "internal/dijkstra.h"
        self.route_finder.set_type(RouteFindertype.DIJKSTRA)
        route = self.route_finder.find()
        route.print()
        c_route_finder_utils.map_print_with_visited(self.map, route)        

    def test_find_fast_marching(self):
        print('test_find_fast_marching')
        #include "internal/fast_marching.h"
        self.route_finder.set_type(RouteFindertype.FAST_MARCHING)
        route = self.route_finder.find()
        route.print()
        c_route_finder_utils.map_print_with_visited(self.map, route)        

    def test_find_fringe_search(self):
        print('test_find_fringe_search')
        #include "internal/fringe_search.h"
        self.route_finder.set_type(RouteFindertype.FRINGE_SEARCH)
        route = self.route_finder.find()
        route.print()
        c_route_finder_utils.map_print_with_visited(self.map, route)        

    def test_find_greedy_best_first(self):
        print('test_find_greedy_best_first')
        #include "internal/greedy_best_first.h"
        self.route_finder.set_type(RouteFindertype.GREEDY_BEST_FIRST)
        route = self.route_finder.find()
        route.print()
        c_route_finder_utils.map_print_with_visited(self.map, route)        

    def test_find_ida_star(self):
        print('test_find_ida_star')
        #include "internal/ida_star.h"
        self.route_finder.set_type(RouteFindertype.IDA_STAR)
        route = self.route_finder.find()
        route.print()
        c_route_finder_utils.map_print_with_visited(self.map, route)        

    def test_find_rta_star(self):
        print('test_find_rta_star')
        #include "internal/rta_star.h"
        self.route_finder.set_type(RouteFindertype.RTA_STAR)
        route = self.route_finder.find()
        route.print()
        c_route_finder_utils.map_print_with_visited(self.map, route)        

    def test_find_sma_star(self):
        print('test_find_sma_star')
        #include "internal/sma_star.h"
        self.route_finder.set_type(RouteFindertype.SMA_STAR)
        route = self.route_finder.find()
        route.print()
        c_route_finder_utils.map_print_with_visited(self.map, route)        

    def test_find_weighted_astar(self):
        print('/.test_find_weighted_astar')
        #include "internal/weighted_astar.h"        
        self.route_finder.set_type(RouteFindertype.WEIGHTED_ASTAR)
        route = self.route_finder.find()
        route.print()
        c_route_finder_utils.map_print_with_visited(self.map, route)                

# 🔽 여기서부터 직접 실행 시 동작
if __name__ == '__main__':
    unittest.main()