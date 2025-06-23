from collections import deque
from PySide6.QtCore import QObject, QTimer

from grid.grid_block import GridBlock
from npc.npc import NPC

from utils.log_to_panel import g_logger

class NPCManager(QObject):
    def __init__(self, block_cache: dict[tuple, GridBlock], parent=None):
        super().__init__(parent)
        self.block_cache = block_cache
        self.npc_dict: dict[str, NPC] = {}
        self.parent = parent  # GridMapController 등
        self.pending_spawn_batches: deque[tuple[tuple, list[str]]] = deque()
        self.pending_despawn_batches: deque[tuple[tuple, list[str]]] = deque()

    def on_block_loaded(self, block_key: tuple, batch_size=100):
        block = self.block_cache.get(block_key)
        if not block:
            return

        cells = list(block.cells.values())

        def process_cell_batch(start_idx=0):
            end_idx = min(start_idx + batch_size, len(cells))
            pending_npcs = []

            for i in range(start_idx, end_idx):
                cell = cells[i]
                if not cell.npc_ids:
                    continue
                for npc_id in cell.npc_ids:
                    pending_npcs.append((npc_id, (cell.x, cell.y)))

            self._spawn_npc_batch(pending_npcs)

            if end_idx < len(cells):
                QTimer.singleShot(1, lambda: process_cell_batch(end_idx))

        process_cell_batch()

    def _spawn_npc_batch(self, pending_npcs: list[tuple[str, tuple]],
                         batch_size: int = 5, interval_msec=1):
        if not pending_npcs:
            return

        def run_batch():
            count = 0
            while pending_npcs and count < batch_size:
                npc_id, coord = pending_npcs.pop(0)
                self.parent.add_npc(npc_id, coord)
                count += 1

            if pending_npcs:
                QTimer.singleShot(interval_msec, run_batch)

        run_batch()

    def on_block_evicted(self, block_key: tuple):
        pending_npc_removals = []

        for npc_id, npc in self.npc_dict.items():
            key_for_npc = self.parent.grid_map.get_origin(npc.start[0], npc.start[1])
            if key_for_npc == block_key:
                pending_npc_removals.append(npc_id)

        self._evict_npc_batch(pending_npc_removals)

    def _evict_npc_batch(self, pending_npc_removals: list[str],
                         batch_size: int = 1, interval_msec=500):
        if not pending_npc_removals:
            return

        def run_batch():
            count = 0
            while pending_npc_removals and count < batch_size:
                npc_id = pending_npc_removals.pop(0)
                self.parent.remove_npc(npc_id)
                g_logger.log_debug(f"[on_npc_evict:batch] NPC 제거됨: {npc_id}")
                count += 1

            if pending_npc_removals:
                QTimer.singleShot(interval_msec, run_batch)

        run_batch()
