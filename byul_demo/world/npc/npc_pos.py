from route import RouteDir
from dataclasses import dataclass

@dataclass
class NpcPos:
    abs_coord: tuple  # 셀 단위 절대 좌표
    disp_dx: float = 0.0  # -1.0 ~ 1.0 비율 단위
    disp_dy: float = 0.0  # -1.0 ~ 1.0 비율 단위
    offset_x: int = 0
    offset_y: int = 0


    def set_disp_dx(self, val:float):
        self.disp_dx = val

    def set_disp_dy(self, val:float):
        self.disp_dy = val
        
    def to_pixel(self, cell_size: int) -> tuple:
        px = (self.abs_coord[0] + self.disp_dx) * cell_size + self.offset_x
        py = (self.abs_coord[1] + self.disp_dy) * cell_size + self.offset_y
        return px, py

    def update_disp(self, dx_ratio: float, dy_ratio: float):
        self.disp_dx = dx_ratio
        self.disp_dy = dy_ratio

    def snap_to(self, new_coord: tuple, tolerance: float = 1e-4):
        """
        위치를 새 좌표로 고정하고, 보간값이 거의 0에 가까울 경우 보간 상태도 초기화한다.
        """
        self.abs_coord = new_coord
        if abs(self.disp_dx) < tolerance:
            self.disp_dx = 0.0
        if abs(self.disp_dy) < tolerance:
            self.disp_dy = 0.0

    def mirrored_pos_from(self, direction: RouteDir) -> "NpcPos":
        """
        현재 위치와 보간값을 기준으로, 
        주어진 방향으로 이동했을 때의 
        다음 셀에서의 반대 보간값을 계산하여 NpcPos를 반환한다.
        disp_dx, disp_dy는 비율(-1.0 ~ 1.0) 단위로 처리된다.
        """
        direction_map = {
            RouteDir.LEFT: (-1, 0), RouteDir.RIGHT: (1, 0),
            RouteDir.UP: (0, -1), RouteDir.DOWN: (0, 1),
            RouteDir.UP_LEFT: (-1, -1), RouteDir.UP_RIGHT: (1, -1),
            RouteDir.DOWN_LEFT: (-1, 1), RouteDir.DOWN_RIGHT: (1, 1),
        }
        dx, dy = direction_map[direction]

        mirror_x = 1.0 - self.disp_dx if dx != 0 else self.disp_dx
        mirror_y = 1.0 - self.disp_dy if dy != 0 else self.disp_dy

        next_x = self.abs_coord[0] + dx
        next_y = self.abs_coord[1] + dy

        return NpcPos(
            abs_coord=(next_x, next_y),
            disp_dx=mirror_x,
            disp_dy=mirror_y
        )

    def mirrored_pos_to(self, next_coord: tuple) -> "NpcPos":
        """
        현재 보간 상태를 기준으로, 
        명시된 다음 좌표에서 보간 상태를 반영한 NpcPos를 생성한다.

        좌표 변화량이 있는 축(x 또는 y)에 대해서만 보간 비율을 반전하여 적용하며,
        disp_dx, disp_dy는 비율(-1.0 ~ 1.0) 단위로 저장된다.
        """
        dx = next_coord[0] - self.abs_coord[0]
        dy = next_coord[1] - self.abs_coord[1]

        mirror_x = 1.0 - self.disp_dx if dx != 0 else self.disp_dx
        mirror_y = 1.0 - self.disp_dy if dy != 0 else self.disp_dy

        return NpcPos(
            abs_coord=next_coord,
            disp_dx=mirror_x,
            disp_dy=mirror_y
        )

    def set_offset(self, x: int, y: int):
        self.offset_x = x
        self.offset_y = y
