from PySide6.QtWidgets import ( QWidget, 
    QVBoxLayout, QLabel, QFormLayout, QDoubleSpinBox, QSpinBox, QCheckBox
    )

from gui.grid_canvas import GridCanvas
from world.npc.npc import NPC
from grid.grid_cell import TerrainType

class NpcPropertyWidget(QWidget):
    def __init__(self, npc: NPC | None, parent=None):
        super().__init__(parent)
        self.npc = npc

        self.layout = QVBoxLayout()
        self.form = QFormLayout()
        self.layout.addLayout(self.form)
        self.setLayout(self.layout)

        self._build_or_empty()


    
    def _build_or_empty(self):
        while self.form.rowCount():
            self.form.removeRow(0)

        if self.npc is None:
            self.form.addRow(QLabel("⚠️ 선택된 NPC가 없습니다."))
            return
        
        # ── 기본 정보 ──
        self.id_label = QLabel(self.npc.id)
        self.form.addRow("🆔 NPC ID:", self.id_label)

        self.start_label = QLabel(f"({self.npc.start[0]}, {self.npc.start[1]})")
        self.form.addRow("📍 시작 위치:", self.start_label)
        
        self.goal_label = QLabel(f"({self.npc.goal[0]}, {self.npc.goal[1]})")
        self.form.addRow("📍 목표 위치:", self.goal_label)        
        
        self.next_label = QLabel(f"(0, 0)")
        self.form.addRow("📍 다음 위치:", self.next_label)

        self.phantom_start_label = QLabel(f"(0, 0)")
        self.form.addRow("📍 유령 시작 위치:", self.phantom_start_label)

        self.speed_spin = QDoubleSpinBox()
        self.speed_spin.setRange(0.1, 100)
        self.speed_spin.setValue(self.npc.speed_kmh)
        self.speed_spin.setSuffix(" km/h")
        self.form.addRow("🚶 이동 속도:", self.speed_spin)

        self.delay_spin = QDoubleSpinBox()
        self.delay_spin.setRange(0.0, 10.0)
        self.delay_spin.setValue(self.npc.start_delay_sec)
        self.delay_spin.setSuffix(" sec")
        self.form.addRow("⏱️ 시작 지연:", self.delay_spin)

        self.capacity_spin = QSpinBox()
        self.capacity_spin.setRange(10, 10000)
        self.capacity_spin.setValue(self.npc.route_capacity)
        self.form.addRow("🗺️ 경로 용량:", self.capacity_spin)

        self.unit_spin = QDoubleSpinBox()
        self.unit_spin.setRange(0.01, 100.0)
        self.unit_spin.setValue(self.npc.grid_unit_m)
        self.form.addRow("📐 그리드 단위(m):", self.unit_spin)

        self.retry_spin = QSpinBox()
        self.retry_spin.setRange(0, 10000)
        self.retry_spin.setValue(self.npc.finder.compute_max_retry)
        self.form.addRow("🔁 최대 재시도:", self.retry_spin)

        # ── 그래픽 설정 ──
        self.offset_x_spin = QSpinBox()
        self.offset_x_spin.setRange(-500, 500)
        self.offset_x_spin.setValue(self.npc.draw_offset_x)
        self.form.addRow("X 오프셋:", self.offset_x_spin)

        self.offset_y_spin = QSpinBox()
        self.offset_y_spin.setRange(-500, 500)
        self.offset_y_spin.setValue(self.npc.draw_offset_y)
        self.form.addRow("Y 오프셋:", self.offset_y_spin)

        self.form.addRow(QLabel("<b>🏞️ 적합한 지형</b>"), QLabel(""))
        self.native_terrain_label = QLabel(f'{self.npc.native_terrain.name}')
        self.form.addRow(self.native_terrain_label)

        self.form.addRow(QLabel("<b>🏞️ 이동 가능 지형</b>"), QLabel(""))

        self.terrain_checkboxes = {}
        for terrain in TerrainType:
            cb = QCheckBox(terrain.name)
            cb.setChecked(terrain in self.npc.movable_terrain)
            self.form.addRow(cb)
            self.terrain_checkboxes[terrain] = cb

        # self.layout.addLayout(self.form)
        # self.setLayout(self.layout)

    def set_start_label(self):
        self.start_label.setText(f"({self.npc.start[0]}, {self.npc.start[1]})")

    def set_goal_label(self):
        self.goal_label.setText(f"({self.npc.goal[0]}, {self.npc.goal[1]})")
        
    def set_phantom_start_label(self):
        if self.npc.phantom_start:
            self.phantom_start_label.setText(
                f"({self.npc.phantom_start[0]}, {self.npc.phantom_start[1]})")

    def set_next_label(self):
        if self.npc.next:
            self.next_label.setText(
                f"({self.npc.next[0]}, {self.npc.next[1]})")

    def set_npc(self, npc: NPC):
        self.npc = npc
        self._build_or_empty()
        if npc:
            npc.start_changed_sig.connect(self.set_start_label)
            npc.goal_changed_sig.connect(self.set_goal_label)
            npc.anim_to_started_sig.connect(self.set_next_label)            
            npc.anim_to_arrived_sig.connect(self.set_phantom_start_label)

    def bind_canvas(self, canvas: GridCanvas):
        self.set_npc(canvas.world.selected_npc)
