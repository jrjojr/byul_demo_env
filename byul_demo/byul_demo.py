import sys

from PySide6.QtWidgets import QApplication

from config import BYUL_DEMO_ENV_PATH, BYUL_DEMO_PATH, WRAPPER_PATH
from world.world import World
from gui.grid_viewer import GridViewer

if __name__ == "__main__":
    # config 로딩용 로딩을 해야 필요한 디렉토리가 추가된다 sys.path에...
    print(f'BYUL_DEMO_ENV_PATH : {BYUL_DEMO_ENV_PATH}')
    print(f'BYUL_DEMO_PATH : {BYUL_DEMO_PATH}')
    print(f'WRAPPER_PATH : {WRAPPER_PATH}')    

    app = QApplication(sys.argv)
    world = World(block_size=100)
    viewer = GridViewer(world)
    viewer.resize(1000, 900)
    viewer.show()
    sys.exit(app.exec())
