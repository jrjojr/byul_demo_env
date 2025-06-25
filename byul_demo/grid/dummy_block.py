import json
import time
from pathlib import Path
from typing import Callable, Optional, Tuple

from PySide6.QtCore import QThread, Signal

from grid.grid_block import GridBlock, BlockLoaderThread
from grid.grid_cell import GridCell, TerrainType

from utils.log_to_panel import g_logger

import random

class DummyBlock(GridBlock):
    def __init__(
        self,
        x0: int, y0: int,
        block_size: int = 100,
        make_cell_func: Optional[Callable[[int, int], GridCell]] = None,
        cells: dict = None,
    ):
        """
        make_cell_func: 셀을 생성하는 함수 (x, y) -> GridCell
        """
        self.block_size = block_size
        self.make_cell_func = make_cell_func or self._default_make_cell

        if cells is None:
            cells = self._generate_cells(x0, y0)

        super().__init__(x0, y0, block_size, cells)

    def _generate_cells(self, x0: int, y0: int) -> dict:
        result = {}
        for dy in range(self.block_size):
            for dx in range(self.block_size):
                x = x0 + dx
                y = y0 + dy
                result[(x, y)] = self.make_cell_func(x, y)
        return result

    def _default_make_cell(self, x: int, y: int) -> GridCell:
        # 기본 지형 랜덤 생성 (외부 지식 없이 내부적으로만 처리)
        terrain = random.choices(
            population=[
                TerrainType.NORMAL,
                TerrainType.FOREST,
                TerrainType.MOUNTAIN,
                TerrainType.WATER,
                TerrainType.FORBIDDEN
            ],
            weights=[0.5, 0.2, 0.1, 0.1, 0.1]
        )[0]
        # return GridCell(x=x, y=y, terrain=terrain)
        return GridCell.random(x=x, y=y, npc_chance=0.05)

    def to_dict(self) -> dict:
        return {
            "x0": self.x0,
            "y0": self.y0,
            "block_size": self.block_size,
            "cells": [cell.to_dict() for cell in self.cells.values()]
        }

    def to_json(self, folder: str):
        Path(folder).mkdir(parents=True, exist_ok=True)
        data = self.to_dict()
        path = Path(folder) / f"block_{self.x0}_{self.y0}.json"
        with open(path, "w", encoding="utf-8") as f:
            json.dump(data, f, indent=4, ensure_ascii=False)

    @classmethod
    def from_dict(cls, data: dict):
        x0 = data["x0"]
        y0 = data["y0"]
        block_size = data.get("block_size", 100)
        raw_cells = data["cells"]

        cell_dict = {}
        for raw in raw_cells:
            cell = GridCell.from_dict(raw)
            cell_dict[(cell.x, cell.y)] = cell

        return cls(
            x0=x0,
            y0=y0,
            block_size=block_size,
            cells=cell_dict
        )

class DummyBlockThread(QThread):
    succeeded = Signal(tuple, DummyBlock)
    failed = Signal(tuple)
    loading_block_started = Signal(float)

    def __init__(self,
                 x0: int, y0: int,
                 block_size: int = 100,
                 make_cell_func: Optional[Callable[[int, int], GridCell]] = None):
        super().__init__()
        self.x0 = x0
        self.y0 = y0
        self.block_size = block_size
        self.make_cell_func = make_cell_func
        self.result: DummyBlock | None = None

    def run(self):
        try:
            block = DummyBlock(
                x0=self.x0,
                y0=self.y0,
                block_size=self.block_size,
                make_cell_func=self.make_cell_func
            )
            self.result = block
            self.succeeded.emit((self.x0, self.y0), block)
        except Exception as e:
            g_logger.log_debug_threadsafe(
                f"[❌ DummyBlockThread 실패] ({self.x0},{self.y0}): {e}"
            )
            self.failed.emit((self.x0, self.y0))

class DummyBlockLoaderThread(BlockLoaderThread):
    def run(self):
        try:
            with open(self.path, "r", encoding="utf-8") as f:
                data = json.load(f)
                block = DummyBlock.from_dict(data)
                self.succeeded.emit(block)
        except Exception as e:
            g_logger.log_debug_threadsafe(
                f"[\u274c 더미 블럭 로딩 실패] {self.path}: {e}")
            self.failed.emit(str(self.path))
