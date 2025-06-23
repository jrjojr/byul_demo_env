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
        self.parent = parent

        self.pending_spawn_batches: deque[list[tuple[str, tuple]]] = deque()
        self.queued_despawn_ids: OrderedDict[str, None] = OrderedDict()

        self._block_load_queue: deque[tuple] = deque()
        self._block_evict_queue: deque[tuple] = deque()

        self._loading_scheduled = False
        self._evicting_scheduled = False
        self._spawning_scheduled = False
        self._despawning_scheduled = False

    def on_block_loaded(self, block_key: tuple, interval_msec=1):
        if block_key not in self._block_load_queue:
            self._block_load_queue.append(block_key)
        if not self._loading_scheduled:
            self._loading_scheduled = True
            QTimer.singleShot(interval_msec, self._process_block_load)

    def on_block_evicted(self, block_key: tuple, interval_msec=50):
        if block_key not in self._block_evict_queue:
            self._block_evict_queue.append(block_key)
        if not self._evicting_scheduled:
            self._evicting_scheduled = True
            QTimer.singleShot(interval_msec, self._process_block_evict)

    def _process_block_load(self, batch_size=100):
        self._loading_scheduled = False

        while self._block_load_queue:
            block_key = self._block_load_queue.popleft()
            block = self.block_cache.get(block_key)
            if not block:
                continue

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

        self._schedule_spawn()

    def _process_block_evict(self):
        self._evicting_scheduled = False

        while self._block_evict_queue:
            block_key = self._block_evict_queue.popleft()
            for npc_id, npc in self.npc_dict.items():
                npc_block_key = self.parent.grid_map.get_origin(npc.start[0], npc.start[1])
                if npc_block_key == block_key:
                    self.queued_despawn_ids[npc_id] = None

        self._schedule_despawn()

    def _schedule_spawn(self, interval_msec: int = 1):
        if not self._spawning_scheduled and self.pending_spawn_batches:
            self._spawning_scheduled = True
            QTimer.singleShot(interval_msec, self._spawn_next_batch)

    def _schedule_despawn(self, interval_msec: int = 1):
        if not self._despawning_scheduled and self.queued_despawn_ids:
            self._despawning_scheduled = True
            QTimer.singleShot(interval_msec, self._despawn_next_batch)

    def _spawn_next_batch(self, batch_size: int = 10, interval_msec: int = 1):
        self._spawning_scheduled = False

        if not self.pending_spawn_batches:
            return

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
                self._schedule_spawn(interval_msec)

        run_batch()

    def _despawn_next_batch(self, batch_size: int = 1, interval_msec: int = 50):
        self._despawning_scheduled = False

        if not self.queued_despawn_ids:
            return

        npc_ids = list(self.queued_despawn_ids.keys())[:batch_size]

        def run_batch():
            nonlocal npc_ids
            for npc_id in npc_ids:
                self.parent.remove_npc(npc_id)
                self.queued_despawn_ids.pop(npc_id, None)
                g_logger.log_debug(f"[Despawn] NPC 제거됨: {npc_id}")

            self._schedule_despawn(interval_msec)

        run_batch()
