from pathlib import Path
import sys

g_root_path = Path('C:/Users/critl/docs/byul_demo_env/byul_demo')
wrapper_path = g_root_path / Path("wrapper/modules")

sys.path.insert(0, str(wrapper_path.resolve()))

import unittest
from coord import c_coord
from coord_finder import c_coord_finder

# 10x10 테스트 맵 전역 배열
test_map = [[False for _ in range(10)] for _ in range(10)]

def setup_map():
    for y in range(10):
        for x in range(10):
            test_map[y][x] = False
    # (4,4)를 중심으로 + 십자형만 열어둠
    test_map[4][4] = True
    test_map[4][5] = True
    test_map[5][4] = True
    test_map[3][4] = True
    test_map[4][3] = True

def is_reachable(coord: c_coord) -> bool:
    if coord.x < 0 or coord.x >= 10 or coord.y < 0 or coord.y >= 10:
        return False
    return test_map[coord.y][coord.x]


class TestCoordFinderGLike(unittest.TestCase):
    def setUp(self):
        setup_map()
        self.start = c_coord(2, 2)

    def tearDown(self):
        self.start.close()

    def _assert_is_reachable(self, coord):
        self.assertIsNotNone(coord)
        self.assertTrue(is_reachable(coord))

    def test_find_goal_bfs(self):
        result = c_coord_finder.find_goal_bfs(self.start, is_reachable, max_range=10)
        self._assert_is_reachable(result)
        print(f"[BFS] found coord: {result.to_tuple()}")
        result.close()

    def test_find_goal_astar(self):
        result = c_coord_finder.find_goal_astar(self.start, is_reachable, max_range=10)
        self._assert_is_reachable(result)
        print(f"[A* ] found coord: {result.to_tuple()}")
        result.close()


if __name__ == '__main__':
    unittest.main(verbosity=2)
