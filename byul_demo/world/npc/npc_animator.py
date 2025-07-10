from route import RouteDir
from dataclasses import dataclass
import time

class DirectionalAnimator:
    def __init__(self, cell_size: int):
        self.cell_size = cell_size
        self.reset()

    def reset(self):
        self.total_elapsed_sec = 0.0
        self.output_arrived = False

    def animate_direction_move(
        self,
        npc,
        direction: RouteDir,
        world,
        elapsed_sec: float = 0.016,
        on_tick: callable = None,
        on_complete: callable = None,
        on_start: callable = None,
    ):
        direction_map = {
            RouteDir.UP: (0, -1),
            RouteDir.DOWN: (0, 1),
            RouteDir.LEFT: (-1, 0),
            RouteDir.RIGHT: (1, 0),
            RouteDir.UP_LEFT: (-1, -1),
            RouteDir.UP_RIGHT: (1, -1),
            RouteDir.DOWN_LEFT: (-1, 1),
            RouteDir.DOWN_RIGHT: (1, 1),
        }

        dx, dy = direction.value if isinstance(
            direction.value, tuple) else direction_map[direction]
        
        start = npc.pos.abs_coord
        goal = (start[0] + dx, start[1] + dy)

        self.reset()

        if on_start:
            on_start(npc)

        while True:
            if self.total_elapsed_sec < npc.start_delay_sec:
                self.total_elapsed_sec += elapsed_sec
                time.sleep(elapsed_sec)
                continue

            arrived = self._step_ratio(
                npc, goal, elapsed_sec, world.grid_unit_m)

            if on_tick:
                on_tick(npc)

            if arrived:
                npc.pos.snap_to(goal)
                world.add_changed_coord(goal)
                break

            time.sleep(elapsed_sec)

        if on_complete:
            on_complete(npc)

    def _step_ratio(self, npc, goal: tuple, elapsed_sec: float, 
                    grid_unit_m: float) -> bool:
        
        speed_cells_per_sec = npc.speed_kmh * 1000 / 3600.0 / grid_unit_m
        delta = speed_cells_per_sec * elapsed_sec
        epsilon = 1e-4

        rel_x = npc.pos.disp_dx
        rel_y = npc.pos.disp_dy

        target_x = 1.0 if goal[0] != npc.pos.abs_coord[0] else 0.0
        target_y = 1.0 if goal[1] != npc.pos.abs_coord[1] else 0.0

        dx = target_x - rel_x
        dy = target_y - rel_y

        if abs(dx) <= delta + epsilon:
            rel_x = target_x
            dx_arrived = True
        else:
            rel_x += delta if dx > 0 else -delta
            dx_arrived = False

        if abs(dy) <= delta + epsilon:
            rel_y = target_y
            dy_arrived = True
        else:
            rel_y += delta if dy > 0 else -delta
            dy_arrived = False

        npc.pos.update_disp(rel_x, rel_y)
        return dx_arrived and dy_arrived
