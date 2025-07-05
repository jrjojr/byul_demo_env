from pathlib import Path
import sys

g_root_path = Path('C:/Users/critl/docs/byul_demo_env/byul_demo')
wrapper_path = g_root_path / Path("wrapper/modules")

sys.path.insert(0, str(wrapper_path.resolve()))

import unittest

from coord import c_coord
from map import c_map, MapNeighborMode

class TestMapCloneAtDegree(unittest.TestCase):
    def setUp(self):
        self.map = c_map(width=5, height=5, mode=MapNeighborMode.DIR_8)

    def tearDown(self):
        self.map.close()

    def test_neighbor_at_degree(self):
        neighbor = self.map.neighbor_at_degree(2, 2, 0.0)  # 0도: →
        self.assertEqual((neighbor.x, neighbor.y), (3, 2))

    def test_clone_neighbor_at_goal(self):
        center = c_coord(2, 2)
        goal = c_coord(4, 1)  # ↗ 방향
        neighbor = self.map.neighbor_at_goal(center, goal)
        self.assertEqual((neighbor.x, neighbor.y), (3, 1))

    def test_neighbors_at_degree_range(self):
        center = c_coord(2, 2)
        goal = c_coord(4, 2)  # → 방향

        neighbors = self.map.neighbors_at_degree_range(
            center, goal, -45.0, 45.0, 1
        )

        # 명시적으로 c_coord로 감싸기
        coords = []
        for ptr in neighbors:
            coord_obj = ptr
            coords.append((coord_obj.x, coord_obj.y))

        result = sorted(coords)
        expected = sorted([(3, 1), (3, 2), (3, 3)])
        self.assertEqual(result, expected)

# 🔽 여기서부터 직접 실행 시 동작
if __name__ == '__main__':
    unittest.main()