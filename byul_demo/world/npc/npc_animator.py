from route import RouteDir
from dataclasses import dataclass

from route import c_route

class DirectionalAnimator:
    def __init__(self, cell_size=100):
        self.cell_size = cell_size

        self.reset()

    def is_anim_started(self):
        return self.is_running
    
    def get_cell_size(self):
        return self.cell_size
    
    def set_cell_size(self, cell_size:int):
        self.cell_size = cell_size

    def reset(self):
        self.total_elapsed_sec = 0.0
        self.output_arrived = False
        self.is_running = False
        self.npc = None
        self.world = None
        self.goal = None

    def start(self, npc, direction: RouteDir, world):
        """Begin animating the NPC in the given direction."""
        dxdy = c_route.direction_to_coord(direction)

        start = npc.pos.abs_coord

        self.reset()
        self.goal = (start[0] + dxdy.x, start[1] + dxdy.y)
        self.is_running = True
        self.npc = npc
        self.world = world

    def step(self, elapsed_sec: float) -> bool:
        """Advance the animation one tick.

        Returns True when the destination cell is reached."""
        if not self.is_running or not self.npc or not self.world:
            return True

        npc = self.npc

        if self.total_elapsed_sec < npc.start_delay_sec:
            self.total_elapsed_sec += elapsed_sec
            return False

        arrived = self._step_ratio(
            npc, self.goal, elapsed_sec, self.world.grid_unit_m)

        if arrived:
            npc.pos.snap_to(self.goal)
            self.world.add_changed_coord(self.goal)
            self.is_running = False

        return arrived

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
