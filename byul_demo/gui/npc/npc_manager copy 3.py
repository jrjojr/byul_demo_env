from collections import deque, OrderedDict
from typing import Optional
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

        self.pending_spawn_batches: deque[list[tuple[str, tuple]]] = deque()
        self.queued_despawn_ids: OrderedDict[str, None] = OrderedDict()

        self._spawning = False
        self._despawning = False

        self._pending_spawn_timer: bool = False
        self._pending_despawn_timer: bool = False

        self._block_load_queue: deque[tuple] = deque()
        self._block_evict_queue: deque[tuple] = deque()
        self._processing_block_load = False
        self._processing_block_evict = False        

    def _schedule_spawn(self, interval_msec: int = 1):
        if not self._pending_spawn_timer:
            self._pending_spawn_timer = True
            QTimer.singleShot(interval_msec, self._spawn_next_batch)

    def _schedule_despawn(self, interval_msec: int = 1):
        if not self._pending_despawn_timer:
            self._pending_despawn_timer = True
            QTimer.singleShot(interval_msec, self._despawn_next_batch)

    def on_block_loaded(self, block_key: tuple, batch_size=100):
        block = self.block_cache.get(block_key)
        if not block:
            return

        pending_npcs: list[tuple[str, tuple]] = []

        for cell in block.cells.values():
            for npc_id in cell.npc_ids:
                if npc_id in self.queued_despawn_ids:
                    del self.queued_despawn_ids[npc_id]
                    g_logger.log_debug(f"[Spawn:{block_key}] 디스폰 예약 취소됨: {npc_id}")

                pending_npcs.append((npc_id, (cell.x, cell.y)))
                if len(pending_npcs) >= batch_size:
                    self.pending_spawn_batches.append(pending_npcs)
                    pending_npcs = []

        if pending_npcs:
            self.pending_spawn_batches.append(pending_npcs)

        if self._despawning:
            g_logger.log_debug("[SpawnPriority] 디스폰 중단 → 스폰 우선 실행")
            self._despawning = False

        self._spawn_next_batch()        

    def _spawn_next_batch(self, batch_size: int = 10, interval_msec: int = 1):
        if self._spawning or not self.pending_spawn_batches:
            if not self._spawning and not self._despawning and self.queued_despawn_ids:
                QTimer.singleShot(0, self._despawn_next_batch)
            return

        self._spawning = True
        pending_npcs = self.pending_spawn_batches.popleft()

        def run_batch():
            nonlocal pending_npcs
            count = 0
            while pending_npcs and count < batch_size:
                npc_id, coord = pending_npcs.pop(0)
                self.parent.add_npc(npc_id, coord)
                count += 1

            if pending_npcs:
                QTimer.singleShot(interval_msec, run_batch)
            else:
                self._spawning = False
                QTimer.singleShot(0, self._spawn_next_batch)

        run_batch()

    def _despawn_next_batch(self, batch_size: int = 1, interval_msec: int = 500):
        if self._spawning or self._despawning or not self.queued_despawn_ids:
            return

        self._despawning = True
        npc_ids = list(self.queued_despawn_ids.keys())[:batch_size]

        def run_batch():
            nonlocal npc_ids
            for npc_id in npc_ids:
                if self._spawning:
                    self._despawning = False
                    return
                self.parent.remove_npc(npc_id)
                self.queued_despawn_ids.pop(npc_id, None)
                g_logger.log_debug(f"[Despawn] NPC 제거됨: {npc_id}")

            self._despawning = False
            QTimer.singleShot(interval_msec, self._despawn_next_batch)

        run_batch()

    def on_block_evicted(self, block_key: tuple):
        for npc_id, npc in self.npc_dict.items():
            npc_block_key = self.parent.grid_map.get_origin(npc.start[0], npc.start[1])
            if npc_block_key == block_key:
                if npc_id not in self.queued_despawn_ids:
                    self.queued_despawn_ids[npc_id] = None
        self._despawn_next_batch()
