# animator_engine.py

from concurrent.futures import ThreadPoolExecutor
from queue import Queue
import threading

class AnimatorTask:
    def __init__(self, npc, world, elapsed_sec):
        self.npc = npc
        self.world = world
        self.elapsed_sec = elapsed_sec

class AnimatorEngine:
    def __init__(self, max_workers=8):
        self.executor = ThreadPoolExecutor(max_workers=max_workers)
        self.queue = Queue()
        self.running = True
        self.thread = threading.Thread(
            target=self._dispatcher_loop, daemon=True)
        self.thread.start()

    def submit(self, npc, world, elapsed_sec):
        task = AnimatorTask(npc, world, elapsed_sec)
        self.queue.put(task)

    def _dispatcher_loop(self):
        while self.running:
            task = self.queue.get()
            if task is None:
                break
            self.executor.submit(self._process_task, task)

    def _process_task(self, task):
        task.npc.animator.step(task.elapsed_sec)

    def shutdown(self):
        self.running = False
        self.queue.put(None)
        self.executor.shutdown(wait=True)
