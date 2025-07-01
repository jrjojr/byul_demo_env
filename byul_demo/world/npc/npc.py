# npc.py
#
# Copyright (c) 2025 별이아빠 (byuldev@outlook.kr)
# This file is part of the Byul World project.
# Licensed under the Byul World 공개 라이선스 v1.0.
# See LICENSE file for details.

from ffi_core import ffi, C
from coord import c_coord
from map import c_map
from dstar_lite import c_dstar_lite, MOVE_TO, CHANGE_COORDS
from typing import List, Optional

from pathlib import Path

from PySide6.QtGui import QPixmap, QPainter, QColor
from PySide6.QtCore import QPoint, QRect, Qt, QTimer, QObject, Signal, Slot

from route import c_route, RouteDir, calc_direction
from list import c_list

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

class NPC(QObject):
    anim_to_started_sig = Signal(tuple)
    anim_to_arrived_sig = Signal(tuple)
    start_changed_sig = Signal(tuple)
    goal_changed_sig = Signal(tuple)

    speed_kmh_changed = Signal(float)
    start_delay_sec_changed = Signal(float)
    compute_max_retry_changed = Signal(int)
    route_capacity_changed = Signal(int)

    disp_dx_changed = Signal(float)
    disp_dy_changed = Signal(float)

    def __init__(self, npc_id: str, world, start:tuple=None, 
                 speed_kmh:float=4.0, start_delay_sec=0.5, 
                 route_capacity=100, 
                 compute_max_retry = 1000, 
                 image_path:Path=None, route_image_path:Path=None, 
                 parent=None):
        
        '''start_delay_sec는 0.5 밑으로는 설정하지 마라.
        여러번 클릭시에 경로 찾기가 잠깐 멈춘다 다시 클릭해야 npc가 움직인다.
        '''
        super().__init__()
        self.parent = parent
        self.world = world
        self.id = npc_id
        self.native_terrain = TerrainType.NORMAL
        self.influence_range = 0

        if start:
            self.finder = c_dstar_lite.from_values(self.world.map, start)
        else:
            self.finder = c_dstar_lite.from_map(self.world.map)
        
        self.compute_max_retry = compute_max_retry
        self.set_compute_max_retry(compute_max_retry)

        self.route_capacity = route_capacity
        self.set_route_capacity(route_capacity)

        self.loop_once = False

        self.movable_terrain = [TerrainType.NORMAL]

        # 경로 및 이미지 캐시에서 로딩
        self.image_paths = ImageManager.get_npc_image_paths(image_path)
        self.images = ImageManager.get_npc_image_set(image_path)        

        self.route_images = ImageManager.get_route_image_set(route_image_path)

        self.direction = random.randint(
            RouteDir.RIGHT.value, RouteDir.DOWN_RIGHT.value)
        
        self._changed_q = Queue()

        self.disp_dx = 0.0
        self.set_disp_dx(0.0)
        self.disp_dy = 0.0
        self.set_disp_dy(0.0)

        self.draw_offset_x = 0
        self.draw_offset_y = 0
        self.anim_dx_arrived = False
        self.anim_dy_arrived = False

        self._goal_q = Queue()
        self.goal_list:list[tuple[int,int]] = list()

        self.real_route = c_route()
        self.proto_route = c_route()

        self.phantom_start = self.start
        self.anim_started = False

        self.next = None
        self._next_q = Queue()
        
        self.speed_kmh = speed_kmh  # default speed
        self.start_delay_sec = start_delay_sec
        self.set_start_delay_sec(start_delay_sec)

        self.total_elapsed_sec = 0.0

        # self.finder.move_func = MOVE_TO
        self._move_cb_c = ffi.callback(
            "void(const coord, void*)", self._move_cb)
        self.finder.move_func = self._move_cb_c

        # self.finder.changed_coords_func = CHANGE_COORDS
        self._changed_coords_cb_c = ffi.callback(
            "GList*(void*)", self._changed_coords_cb)
        self.finder.changed_coords_func = self._changed_coords_cb_c

        self._cost_cb_c = ffi.callback(
            "gfloat(const map, const coord, const coord, void*)",
            self._cost_cb
        )
        self.finder.cost_func = self._cost_cb_c
        # 반드시 C 함수로 등록
        # C.dstar_lite_set_cost_func(self.finder.ptr(), self._cost_cb_c, ffi.NULL)        

        self._is_blocked_cb_c = ffi.callback(
            "gboolean(const map, gint, gint, void*)",
            self._is_blocked_cb
        )
        # self.finder.is_blocked_func = self._is_blocked_cb_c

        self.finding_thread = None
        
        self.finding_active = False

        self._real_q = Queue()
        self._proto_q = Queue()

    @Slot(float)
    def set_disp_dx(self, dx:float):
        self.disp_dx = dx
        self.disp_dx_changed.emit(dx)

    @Slot(float)
    def set_disp_dy(self, dy:float):
        self.disp_dy = dy
        self.disp_dy_changed.emit(dy)

    @Slot(int)
    def set_route_capacity(self, capacity:int):
        self.route_capacity = capacity
        self.route_capacity_changed.emit(capacity)

    @Slot(int)
    def set_compute_max_retry(self, max_retry:int):
        self.compute_max_retry = max_retry
        self.finder.compute_max_retry = max_retry
        self.compute_max_retry_changed.emit(max_retry)

    @Slot(float)
    def set_speed_kmh(self, speed_kmh:float):
        self.speed_kmh = speed_kmh

    @Slot(float)
    def set_start_delay_sec(self, delay_sec:float):
        self.start_delay_sec = delay_sec
        self.start_delay_sec_changed.emit(delay_sec)

    def reset(self):
        """NPC 상태를 초기화한다. (경로, 애니메이션, 큐 등)"""
        # 탐색기 재초기화
        if self.start:
            self.finder.close()
            self.finder = c_dstar_lite.from_values(self.world.map, self.start)
        else:
            self.finder.close()
            self.finder = c_dstar_lite.from_map(self.world.map)

        self.finder.max_range = self.max_range
        self.finder.compute_max_retry = self.finder.compute_max_retry
        self.finder.move_func = self._move_cb_c
        self.finder.changed_coords_func = self._changed_coords_cb_c
        self.finder.cost_func = self._cost_cb_c
        # self.finder.is_blocked_func = self._is_blocked_cb_c

        # 경로 초기화
        self.real_route.clear_coords()
        self.proto_route.clear_coords()
        self.proto_route.clear_visited()
        self.real_route.clear_visited()
        self.goal_list.clear()

        self._changed_q.shutdown()
        self._goal_q.shutdown()
        self._next_q.shutdown()

        self._real_q.shutdown()
        while not self._real_q.empty():
            route = self._real_q.get()
            route.close()  # 무조건 해제

        # self._proto_q.shutdown()
        while not self._proto_q.empty():
            route = self._proto_q.get()
            route.close()  # 무조건 해제

        self.next = None

        # 애니메이션 상태
        self.anim_started = False
        self.anim_dx_arrived = False
        self.anim_dy_arrived = False
        self.disp_dx = 0.0
        self.disp_dy = 0.0
        self.disp_dx_changed.emit(0.0)
        self.disp_dy_changed.emit(0.0)

        # 위치 추적 및 타이머
        self.phantom_start = self.start
        self.total_elapsed_sec = 0.0

        # 스레드 상태
        self.finding_thread = None
        self.finding_active = False

    def close(self):
        '''NPC 종료 시 리소스를 정리한다'''
        # 🔸 탐색 쓰레드 정지
        self.stop_finding()

        self.phantom_start = None

        self._changed_q.shutdown()
        self._goal_q.shutdown()
        self._next_q.shutdown()

        while not self._real_q.empty():
            route = self._real_q.get()
            route.close()  # 무조건 해제

        # self._proto_q.shutdown()
        while not self._proto_q.empty():
            route = self._proto_q.get()
            route.close()  # 무조건 해제        

        self.finder.close()
        self.proto_route.close()
        self.real_route.close()

        # 🔸 로깅
        g_logger.log_debug(f"[NPC.close] npc({self.id}) 종료 완료")

    def __del__(self):
        self.close()

    @property
    def start(self):
        return self.finder.start.to_tuple()
    
    @start.setter
    def start(self, coord:tuple):
        self.finder.start = c_coord.from_tuple(coord)
        self.start_changed_sig.emit(coord)

    @property
    def goal(self):
        return self.finder.goal.to_tuple()
    
    @goal.setter
    def goal(self, coord: tuple):
        self.finder.goal = c_coord.from_tuple(coord)
        self.goal_changed_sig.emit(coord)

    @property
    def speed_kmh(self):
        return self.m_speed_kmh
    
    @speed_kmh.setter
    def speed_kmh(self, kmh: float):
        self.m_speed_kmh = kmh
        self.finder.interval_msec = self.interval_msec
        self.speed_kmh_changed.emit(kmh)

    @property
    def interval_msec(self):
        """속도(km/h)와 셀 단위(m)를 기준으로 한 이동 간격(ms) 반환"""
        speed_mps = self.m_speed_kmh * 1000 / 3600.0
        if speed_mps == 0:
            return float('inf')  # 속도가 0이면 무한대 시간 필요
        return int((self.world.grid_unit_m / speed_mps) * 1000)

    @Slot(int, int)
    def set_start_from_int(self, x:int, y:int):
        s = (x, y)
        self.start = s

    def append_goal(self, coord:tuple):
        self._goal_q.put(coord)
        
    def move_to(self, coord: tuple):
        if self.finding_thread and self.finding_thread.is_alive():
            g_logger.log_debug("🔁 현재 find 루프 종료 요청 중...")
            # 바로 요청하면 마우스 클릭으로 여러번 함수 호출할때
            # force_quit중에 또 force_quit이 실행되어서 
            # 다음 finder 루프를 돌수 없다.
            self.finder.force_quit()
            # 이제 요청했다. 루프 멈추라고...
            # 즉각적으로 멈추는게 아니다.
            # 아마도 시속에 따라 계산된 interval_msec 이후에 멈춘다.
            self.finding_active = False

        self.goal = coord
        c = c_coord.from_tuple(coord)
        self.finder.update_vertex(c)

        # 목표 모드가 한번만 설정하는것이다.
        # append_goal은 기본적으로 클릭할때마다 목표를 추가한다
        # 하지만 모드가 loop_once 이므로 가장 마지막에 설정된 목표만 
        # 실제 이동할때 사용된다.
        self.loop_once = True

        # 바로 목표로 이동하는게 아니다.
        # 시작 지연 msec에 따라 약간 지연 후에 on_tick에서 이동 시작한다.        
        self.append_goal(coord)

    def anim_moving_to(self, next: tuple, elapsed_sec: float, cell_size:int):
        speed_mps = self.speed_kmh * 1000 / 3600.0
        speed_pixel_per_sec = speed_mps * (cell_size / self.world.grid_unit_m)
        delta = speed_pixel_per_sec * elapsed_sec
        epsilon = 1e-3

        target_dx = (next[0] - self.phantom_start[0]) * cell_size
        target_dy = (next[1] - self.phantom_start[1]) * cell_size

        delta_x = target_dx - self.disp_dx
        if abs(delta_x) <= delta + epsilon:
            self.disp_dx = target_dx
            self.anim_dx_arrived = True
            self.disp_dx_changed.emit(target_dx)
        else:
            self.disp_dx += delta if delta_x > 0 else -delta
            self.anim_dx_arrived = False
            self.disp_dx_changed.emit(self.disp_dx)

        delta_y = target_dy - self.disp_dy
        if abs(delta_y) <= delta + epsilon:
            self.disp_dy = target_dy
            self.anim_dy_arrived = True
            self.disp_dy_changed.emit(target_dy)
        else:
            self.disp_dy += delta if delta_y > 0 else -delta
            self.anim_dy_arrived = False
            self.disp_dy_changed.emit(self.disp_dy)

    def is_anim_arrived(self) -> bool:
        return self.anim_dx_arrived and self.anim_dy_arrived
    
    def on_tick(self, elapsed_sec: float, cell_size:int):
        if self.total_elapsed_sec >= self.start_delay_sec:
            if not self._goal_q.empty():
                if not self.finding_thread or not self.finding_thread.is_alive():
                    g_logger.log_debug(f'''지금 find()가 실행되었다
가장 중요한 finder.is_quit_forced는 {self.finder.is_quit_forced()}
elapsed_sec : {elapsed_sec}, 
self.total_elapsed_sec : {self.total_elapsed_sec},
start_delay_sec : {self.start_delay_sec}''')
                    self.find()
            self.total_elapsed_sec = 0.0
        else:
            self.total_elapsed_sec += elapsed_sec

        # 현재 목표가 없으면 큐에서 꺼내서 대기 목표 설정
        if not self.next and not self.anim_started:
            try:
                next = self._next_q.get_nowait()
            except Empty:
                next = None

            if next is not None:
                self.next = next
                self.anim_started = True
                self.anim_to_started_sig.emit(self.next)

        if self.next:
            ps = c_coord.from_tuple(self.phantom_start)
            sn = c_coord.from_tuple(self.next)
            new_dir = calc_direction(ps, sn)
            if new_dir != RouteDir.UNKNOWN:
                self.direction = new_dir

            self.anim_moving_to(self.next, elapsed_sec, cell_size)

            if self.is_anim_arrived():
                self.anim_to_arrived_sig.emit(self.next)
                self.start = self.next
                self.phantom_start = self.start

                self.disp_dx = 0
                self.disp_dy = 0
                self.disp_dx_changed.emit(0)
                self.disp_dy_changed.emit(0)
                self.anim_dx_arrived = False
                self.anim_dy_arrived = False
                self.anim_started = False
                self.next = None  # 다음 tick에서 새 목표 pop


    def find_loop(self):
        '''쓰레드에서 실행된다.'''
        try:
            prev_goal = None
            while self.finding_active:
                try:
                    if self.loop_once:
                        # 가장 마지막에 추가된 목표만 사용한다.
                        while not self._goal_q.empty():
                            g = self._goal_q.get_nowait()
                        
                        self.goal = g
                        self.loop_once = False
                    else:
                        if prev_goal is None:
                            prev_goal = self.start

                        if prev_goal == self.start:
                            # g = self._goal_q.get(timeout=1)  # 최대 1초 대기
                            g = self._goal_q.get_nowait()
                            self.goal = g
                            self.start = prev_goal
                        else:
                            if prev_goal != self.goal:
                                prev_goal = self.goal

                    self.finder.find_proto()
                    route = self.finder.get_proto_route()
                    self._proto_q.put(route.copy())
                    if route.success:
                        g_logger.log_debug_threadsafe(f'proto route 찾기가 성공했다')
                    else:
                        g_logger.log_debug_threadsafe(f'proto route 찾기가 실패했다')
                        '''실패시 행동요령
                        1. 실패하면 목표를 찾아야 한다.
                        2. 장애물을 제거해야 한다.
                        3. 가만 있는다.
                        '''

                    g_logger.log_debug_threadsafe(f'''초기 경로 찾기 로그:
        self.finder.proto_compute_retry_count : {self.finder.proto_compute_retry_count}, 
        self.finder.reconstruct_retry_count : {self.finder.reconstruct_retry_count}, 
        ''')

                    self.finder.find_loop()
                    route = self.finder.get_proto_route()
                    self._real_q.put(route.copy())
                    if route.success:
                        g_logger.log_debug_threadsafe(f'real route 찾기가 성공했다')
                    else:
                        g_logger.log_debug_threadsafe(f'real route 찾기가 실패했다')

                    g_logger.log_debug_threadsafe(f'''실제 경로 찾기 로그
        self.finder.real_compute_retry_count : {self.finder.real_compute_retry_count}, 
        self.finder.real_loop_retry_count : {self.finder.real_loop_retry_count}
        ''')
                    g_logger.log_debug_threadsafe(
                                '✅ find_loop가 정상 종료되었습니다')
                except Empty:
                    g_logger.log_debug_threadsafe(
                        '✅ find_loop가 self._goal_q.get_nowait()가 '
                        'Empty라서 종료되었습니다')
                    break

        except Exception as e:
            g_logger.log_debug_threadsafe(f'🚨 경로 탐색 중 예외 발생: {e}')

        finally:
            # 🔸 종료 처리 (외부에서 self.finding_active = False 호출할 수 
            # 있으므로 여기선 굳이 다시 False 안 해도 됨)
            self.finding_thread = None

    def find(self):
        if self.finding_thread and self.finding_thread.is_alive():
            return  # 이미 실행 중이면 무시
        
        self.finding_active = True
        self.finding_thread = Thread(
            target=self.find_loop, daemon=True)
        
        self.finding_thread.start()

    def stop_finding(self):
        self.finding_active = False

        if self.finding_thread:
            if self.finding_thread.is_alive():
                # finder() 내부에서 while루프에 forece_quite = True해서 루프 종료
                # finder()가 종료되면 자연스럽게 쓰레드 종료
                self.finder.force_quit()

                self.finding_thread.join(timeout=1.0)
                if self.finding_thread and self.finding_thread.is_alive():
                    g_logger.log_debug_threadsafe(
                        "⏰ 타임아웃: 쓰레드 아직 종료되지 않음!")
                    # 필요하면 여기서 강제종료 로직 또는 경고 처리
                    return  # 또는 그냥 남겨둠
            self.finding_thread = None

    def _move_cb(self, coord_c, userdata):
        try:
            c = c_coord(raw_ptr=coord_c).to_tuple()

            g_logger.log_debug_threadsafe(f"[MOVE_CB] 받은 이동 좌표: {c}")
            # print(f"[MOVE_CB] 받은 이동 좌표: {c}")

            # 🔹 이동 큐에 좌표 추가 (thread-safe) 복사해서 추가해야 한다.
            # a = copy.deepcopy(c)
            self._next_q.put( c)

        except Exception as e:
            g_logger.log_debug_threadsafe(f"[MOVE_CB] 예외 발생: {e}")

    def add_changed_coord(self, coord_c: tuple):
        self._changed_q.put(coord_c)

    def clear_changed_coords(self):
        try:
            while not self._changed_q.empty():
                dummy = self._changed_q.get_nowait()
        except Empty:
            g_logger.log_debug('모두 비웠다 changed_coords를...')
            pass

    def _changed_coords_cb(self, userdata):
        g_logger.log_debug_threadsafe('_changed_coords_cb 호출됨')

        c_list_obj = c_list()

        while not self._changed_q.empty():
            tu = self._changed_q.get(1)
            c = c_coord.from_tuple(tu)
            c_list_obj.append(c)

        return c_list_obj.ptr()

    def _cost_cb(self, map_ptr, start_ptr, goal_ptr, userdata):
        if not map_ptr or not start_ptr or not goal_ptr:
            return ffi.cast("gfloat", float("inf"))

        map = c_map(raw_ptr=map_ptr)
        start = c_coord(raw_ptr=start_ptr)
        goal = c_coord(raw_ptr=goal_ptr)

        tg = goal.to_tuple()
        cell = self.world.block_mgr.get_cell(tg)
        if self.is_obstacle(cell):
            return ffi.cast("gfloat", float("inf"))

        dx = start.x - goal.x
        dy = start.y - goal.y
        return ffi.cast("gfloat", math.hypot(dx, dy))

    def _is_blocked_cb(self, map:c_map, x, y, userdata):
        c = (x, y)
        cell = self.world.block_mgr.get_cell(c)
        return self.is_obstacle(cell)

    def draw(self, painter: QPainter,
                 start_win_pos_x:int, start_win_pos_y:int, cell_size):
        '''실제 디바이스에 이미지를 그린다.
        '''
        x = start_win_pos_x + self.draw_offset_x + int(self.disp_dx)
        y = start_win_pos_y + self.draw_offset_y + int(self.disp_dy)

        # 배경: 반투명 검정
        # rect = QRect(x, y, cell_size, cell_size)
        # painter.setBrush(QColor(0, 0, 0, 127))
        # painter.setPen(Qt.NoPen)
        # painter.drawRect(rect)

        image = self.get_image()

        painter.drawPixmap(
                x, y, cell_size, cell_size, image)
        pass

    def get_image(self):
        return self.images[self.direction]
    
    def get_image_path(self):
        return self.image_paths[self.direction]
    
    def get_proto_route_image(self, coord):
        c = c_coord.from_tuple(coord)
        cur_idx = self.proto_route.find(c)
        direction = self.proto_route.get_direction_by_index(cur_idx)
        return self.route_images[direction]
    
    def get_real_route_image(self, coord):
        c = c_coord.from_tuple(coord)
        cur_idx = self.real_route.find(c)
        direction = self.real_route.get_direction_by_index(cur_idx)
        return self.route_images[direction]    

    def load_image_paths(self, image_path:Path):
        self.image_paths = ImageManager.get_npc_image_paths(image_path)

    def load_images(self, image_path:Path):
        self.images = ImageManager.get_npc_image_set(image_path)                

    def on_proto_route_found(self):
        try:
            p: c_route = self._proto_q.get_nowait()
        except Empty:
            g_logger.log_debug('텅 비었다 self._proto_q.get_nowait()')
            return

        try:
            self.proto_route.append_nodup(p)
            len_full = len(self.proto_route)
            if len_full > self.route_capacity:
                self.proto_route.slice(
                    len_full - self.route_capacity,
                    len_full)
            g_logger.log_debug(
                f'len(self.proto_route): {len(self.proto_route)}')

        finally:
            pass

    def on_real_route_found(self):
        try:
            p: c_route = self._real_q.get_nowait()
        except Empty:
            g_logger.log_debug('텅 비었다 self._real_q.get_nowait()')
            return

        try:
            self.real_route.append_nodup(p)
            len_full = len(self.real_route)
            if len_full > self.route_capacity:
                self.real_route.slice(
                    len_full - self.route_capacity,
                    len_full)
            g_logger.log_debug(
                f'len(self.real_coord_list): {len(self.real_route)}')                

        finally:

            pass

    def clear_proto_route(self):
        self.proto_route.clear_coords()

    def clear_real_route(self):
        self.real_route.clear_coords()

    def flush_goal_q(self):
        while not self._goal_q.empty():
            ct = self._goal_q.get(1)
            self.goal_list.append(ct)
        return self.goal_list
    
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

