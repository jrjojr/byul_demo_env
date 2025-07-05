from pathlib import Path
import sys

g_root_path = Path('C:/Users/critl/docs/byul_demo_env/byul_demo')
wrapper_path = g_root_path / Path("wrapper/modules")

sys.path.insert(0, str(wrapper_path.resolve()))

import unittest

from coord import c_coord
from map import c_map, MapNeighborMode
from algo import c_algo
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

    def test_find_default(self):
        # start = c_coord(0, 0)
        # goal = c_coord(9, 9)
        # self.algo.set_start(start)
        # self.algo.set_goal(goal)

        # # // 장애물 삽입 (세로 차단)
        # for i in range(1, 10):
        #     self.map.block(5, i)

        route = self.algo.find()
        route.print()
        c_algo_utils.map_print_with_visited(self.map, route)

# 🔽 여기서부터 직접 실행 시 동작
if __name__ == '__main__':
    unittest.main()