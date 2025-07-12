# npc.py
#
# Copyright (c) 2025 별이아빠 (byuldev@outlook.kr)
# This file is part of the Byul World project.
# Licensed under the Byul World 공개 라이선스 v1.0.
# See LICENSE file for details.

from ffi_core import ffi, C
from coord import c_coord
from map import c_map
from dstar_lite import c_dstar_lite

from pathlib import Path

from PySide6.QtGui import QPixmap, QPainter, QColor
from PySide6.QtCore import QPoint, QRect, Qt, QTimer, QObject, Signal, Slot

from route import c_route, RouteDir
from coord_list import c_coord_list

from world.village.village import Village
from grid.grid_cell import TerrainType, GridCell, CellStatus

from utils.log_to_panel import g_logger

from queue import Queue, Empty

import random

import uuid
import math

from utils.image_manager import ImageManager

from threading import Thread

import copy

from world.npc.npc_animator import DirectionalAnimator
from world.npc.npc_pos import NpcPos

from world.route_engine.common import RouteResult
from algo import RouteAlgotype

from typing import TYPE_CHECKING
if TYPE_CHECKING:
    from world.world import World  # 순환 참조 방지용 타입 힌트

class NPC(QObject):
    anim_to_started_sig = Signal(tuple)
    anim_to_arrived_sig = Signal(tuple)
    start_changed_sig = Signal(tuple)
    goal_changed_sig = Signal(tuple)

    speed_kmh_changed = Signal(float)
    start_delay_sec_changed = Signal(float)
    max_retry_changed = Signal(int)
    route_capacity_changed = Signal(int)

    disp_dx_changed = Signal(float)
    disp_dy_changed = Signal(float)

    def __init__(self, npc_id: str, world:'World', start=(0,0), 
                 speed_kmh:float=4.0, start_delay_sec=0.5, 
                 route_capacity=100, 
                 algotype = RouteAlgotype.ASTAR,
                 max_retry = 1000, 
                 influence_range = 0,
                 max_range = 10,
                 image_path:Path=None, route_image_path:Path=None, 
                 parent=None):
        
        '''start_delay_sec는 dstar_lite에서는 0.5 밑으로는 설정하지 마라.
        여러번 클릭시에 경로 찾기가 잠깐 멈춘다 다시 클릭해야 npc가 움직인다.
        '''
        super().__init__()
        self.parent = parent
        self.world = world
        self.id = npc_id
        self.native_terrain = TerrainType.NORMAL
        self.movable_terrain = [TerrainType.NORMAL]
        self.influence_range = influence_range
        self.max_range = max_range

        self.pos = NpcPos(start)
        self.m_goal = start

        self.algotype = algotype
        self.max_retry = max_retry
        self.set_compute_max_retry(max_retry)

        self.route_capacity = route_capacity
        self.set_route_capacity(route_capacity)

        self.loop_once = False

        # 경로 및 이미지 캐시에서 로딩
        self.image_paths = ImageManager.get_npc_image_paths(image_path)
        self.images = ImageManager.get_npc_image_set(image_path)        

        self.route_images = ImageManager.get_route_image_set(route_image_path)

        self.direction = random.randint(
            RouteDir.RIGHT.value, RouteDir.DOWN_RIGHT.value)
        
        self.reset()

        self.animator = DirectionalAnimator(self,
            on_complete_cb=self.on_anim_complete_cb,
            on_tick_cb=self.on_anim_tick_cb,
            on_start_cb=self.on_anim_start_cb)

    def reset(self):
        """NPC 상태를 초기화한다. (경로, 애니메이션, 큐 등)"""

        # self.goal_list.clear()

        self.goal_list:list[tuple[int,int]] = list()

        self.real_list = list()
        
        self.proto_list = list()
        self.proto_route_index = 0
        self.proto = c_route()

        self.next_history = list()
        self.cur_index = 0
       
        self.speed_kmh = 4  # default speed
        self.start_delay_sec = 0.5
        self.set_start_delay_sec(0.5)

        self.start = (0,0)        
        self.goal = self.start
        self.cur_index_changed = False

    @Slot(int)
    def set_route_capacity(self, capacity:int):
        self.route_capacity = capacity
        self.route_capacity_changed.emit(capacity)

    @Slot(int)
    def set_compute_max_retry(self, max_retry:int):
        self.max_retry = max_retry
        self.compute_max_retry_changed.emit(max_retry)

    @Slot(float)
    def set_speed_kmh(self, speed_kmh:float):
        self.speed_kmh = speed_kmh

    @Slot(float)
    def set_start_delay_sec(self, delay_sec:float):
        self.start_delay_sec = delay_sec
        self.start_delay_sec_changed.emit(delay_sec)

    def close(self):
        '''NPC 종료 시 리소스를 정리한다'''

        # 🔸 로깅
        g_logger.log_debug(f"[NPC.close] npc({self.id}) 종료 완료")

    def __del__(self):
        self.close()

    @property
    def start(self):
        return self.pos.cur_coord
        
    @start.setter
    def start(self, coord:tuple):
        # self.pos.cur_coord = coord
        self.pos.move_to(coord)
        if self.proto:
            self.direction = self.proto.get_direction_by_index(self.cur_index)
        self.start_changed_sig.emit(coord)

    @property
    def goal(self):
        return self.m_goal
    
    @goal.setter
    def goal(self, coord: tuple):
        self.m_goal = coord
        self.goal_changed_sig.emit(coord)

    def append_goal(self, coord:tuple):
        self.goal_list.append(coord)

    def move_to(self, coord: tuple):
        self.loop_once = True
        self.goal_list.clear()
        self.append_goal(coord)
        self.goal = coord

    @property
    def speed_kmh(self):
        return self.m_speed_kmh
    
    @speed_kmh.setter
    def speed_kmh(self, kmh: float):
        self.m_speed_kmh = kmh
        self.speed_kmh_changed.emit(kmh)

    @property
    def interval_msec(self):
        """속도(km/h)와 셀 단위(m)를 기준으로 한 이동 간격(ms) 반환"""
        speed_mps = self.m_speed_kmh * 1000 / 3600.0
        if speed_mps == 0:
            return float('inf')  # 속도가 0이면 무한대 시간 필요
        return int((self.world.grid_unit_m / speed_mps) * 1000)

    def on_tick(self, elapsed_sec: float):
        # 길찾기 요청 조건 (목표 존재 + 시작 ≠ 목표)
        if len(self.goal_list) > 0 and self.start != self.goal:
            g_logger.log_debug(f'지금 find()가 실행되었다 '
                f'elapsed_sec : {elapsed_sec}, '
                f'start_delay_sec : {self.start_delay_sec}')
            self.find()

        # 경로가 존재하고, 아직 도달하지 않았는가?
        if len(self.proto) > 0 and self.cur_index < len(self.proto):
            if self.cur_index_changed and not self.animator.is_anim_started():
                if self.start == self.goal:
                    return  # 목적지 = 시작점이면 애니메이션 불필요

                self.direction = self.proto.get_direction_by_index(self.cur_index)
                self.animator.start(self.direction)

                self.cur_index_changed = False

            # animator가 실행 중이면 tick을 제출
            if self.animator.is_anim_started():
                self.world.animator_engine.submit(self.animator, elapsed_sec)

    def on_anim_tick_cb(self):
        # 필요 시 디버깅/로깅 가능
        pass

    def on_anim_complete_cb(self):
        self.cur_index += 1
        self.cur_index_changed = True
        next = self.proto.coord_at(self.cur_index).to_tuple()
        self.pos.move_to(next)
        self.world.set_start(self, next)
        if self.start == self.goal:
            self.cur_index = 0

        self.world.add_changed_coord(self.goal)

    def on_anim_start_cb(self):
        # 필요 시 효과음, 감정 연출 등 후처리
        # grid_canvas에서 그림을 제거해야한다.
        pass

    def find(self):
        if len(self.goal_list) > 0:
            goal = self.goal_list.pop(0)
        else:
            if self.start == self.goal:
                return
            goal = self.goal
            
        map = self.world.map
        map.set_is_coord_blocked_fn(self._is_blocked_cb)
        self.world.algo_engine.submit(map, self.id,
            self.algotype, self.start, goal, self.on_proto_found,
            self.max_retry)

    def on_proto_found(self, result:RouteResult):
        id = result.npc_id
        route:c_route = result.route
        self.proto.append(route, nodup=True)

        a_list = route.coords().to_list()

        for i in range(len(a_list)):
            self.proto_list.append(a_list[i])

        # 길이 초과 시 마지막 N개만 유지
        if len(self.proto_list) > self.route_capacity:
            self.proto_list[:] = self.proto_list[-self.route_capacity:]

        end  = len(self.proto)
        if end > self.route_capacity:
            odd = end - self.route_capacity
            start = odd

            sliced = self.proto.slice(start, end)
            self.proto = sliced

        g_logger.log_debug_threadsafe(
            f'npc_id : {id}, len(proto_list): {len(self.proto_list)}')
        g_logger.log_debug_threadsafe(
            f'route.to_string() : {route.to_string()}')

        # proto를 생성했으니 self.cur_index_changed = True 발생
        self.cur_index_changed = True

        # coord =  self.proto_list[-1].to_tuple()
        # self.world.set_start(self, coord)

    def _move_cb(self, coord_c:'c_coord', userdata):
        try:
            g_logger.log_debug_threadsafe(f"[MOVE_CB] 받은 이동 좌표: {coord_c}")
            c = coord_c.to_tuple()
            self.next_history.append(c)

        except Exception as e:
            g_logger.log_debug_threadsafe(f"[MOVE_CB] 예외 발생: {e}")

    def _cost_cb(self, map_obj, start, goal, userdata):
        """
        기본 비용 함수 예시. set_cost_func()에 전달 가능한 형태.
        
        - 장애물 셀은 inf 반환
        - 아닌 경우 유클리드 거리 반환
        """
        tg = goal.to_tuple()
        cell = self.world.block_mgr.get_cell(tg)

        if cell is None or self.is_obstacle(cell):
            return float('inf')

        dx = start.x - goal.x
        dy = start.y - goal.y
        return math.hypot(dx, dy)

    def _is_blocked_cb(self, map:c_map, x, y, userdata):
        c = (x, y)
        cell = self.world.block_mgr.get_cell(c)
        return self.is_obstacle(cell)

    def draw(self, painter: QPainter, 
            start_win_pos_x: int, start_win_pos_y: int, 
            cell_size: int):
        """
        NPC를 현재 위치에 그린다.
        disp_dx, disp_dy는 -1.0 ~ 1.0 사이의 상대 위치 비율이다.
        """
        # cur_pos = self.pos.get_pos()
        cur_pos_x = self.pos.disp_dx
        cur_pos_y = self.pos.disp_dy

        # 실제 렌더링 좌표 계산 (비율 적용 + cell_size 보간)
        draw_x = start_win_pos_x + cur_pos_x * cell_size
        draw_y = start_win_pos_y + cur_pos_y * cell_size

        image = self.get_image()

        # 이미지 크기 = cell_size × cell_size 로 고정 렌더링
        painter.drawPixmap(int(draw_x), int(draw_y), 
                           cell_size, cell_size, image)

    def get_image(self):
        return self.images[self.direction]
    
    def get_image_path(self):
        return self.image_paths[self.direction]
    
    def get_proto_image(self, index:int):
        if len(self.proto_list) <= 0:
            return None

        # 마지막 지점 → 이전 방향
        if index >= len(self.proto) - 1:
            dir = self.proto.get_direction_by_index(len(self.proto) - 2)
        else:
            # 시작지점이거나 중간 좌표는 다음 좌표와의 관계로 방향을 얻으면 된다.
            dir = self.proto.get_direction_by_index(index)
            
        return self.route_images[dir]

    # def get_proto_image(self):
    #     return self.route_images[self.direction]
        
    def get_real_route_image(self, coord):
        pass

    def load_image_paths(self, image_path:Path):
        self.image_paths = ImageManager.get_npc_image_paths(image_path)

    def load_images(self, image_path:Path):
        self.images = ImageManager.get_npc_image_set(image_path)                

    def on_real_route_found(self):
        pass

    def clear_proto(self):
        self.world.clear_proto_flags(self)
        self.proto_list.clear()

    def clear_real_route(self):
        pass

    def is_movable(self, cell:GridCell):
        return not self.is_obstacle(cell)
    
    def is_obstacle(self, cell:GridCell):
        if cell.status == CellStatus.NPC:
            return True
        
        if cell.terrain == TerrainType.FORBIDDEN:
            return True

        if not cell.terrain in self.movable_terrain:            
            return True
        
        return False

    def get_image(self):
        return self.images[self.direction]
    
    def get_selected_npc_image(self, path:Path=None):
        return ImageManager.get_selected_npc_image(path)
    
    @staticmethod
    def generate_random_npc_id() -> str:
        return str(uuid.uuid4())[:8]

