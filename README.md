# 🌟 Byul's World DEMO 0.1 – Pathfinding Simulator

**Byul's World DEMO 0.1** is a lightweight real-time simulator  
where autonomous NPCs navigate a 2D grid toward their goals.

Each NPC plans its own path, avoids obstacles, and moves independently.  
This project was built to visualize pathfinding in action  
and to lay the foundation for a living simulation world.

> 💖 If you enjoy this project, consider supporting development at [paypal.me/jrjojr](https://paypal.me/jrjojr)

---

## 🧍 What is an NPC?

In this simulation, an **NPC (Non-Player Character)** is an autonomous agent  
that follows user-defined or system-assigned goals.

Each NPC can:

- Hold multiple goals in a queue  
- Calculate and follow paths on its own  
- React to obstacles placed on the map

---

## ✅ Key Features

| Feature            | Description |
|--------------------|-------------|
| D\* Lite Algorithm | Lightweight real-time pathfinding core |
| GridMap System     | Dynamic 100x100 cell-based grid structure |
| Multiple NPCs      | Create, switch, and control several NPCs |
| Goal Queueing      | Add multiple goals with Shift + Right Click |
| Obstacle Toggling  | Toggle obstacles at the cursor with Spacebar |
| Intuitive Controls | Fully mouse-driven interface |

---

## 🎮 Controls

| Action              | Description |
|---------------------|-------------|
| Left Click          | Select an NPC (green glow indicates selection) |
| Right Click         | Set a new goal (clears previous ones) |
| Shift + Right Click | Add a goal to the queue |
| Spacebar            | Toggle obstacle at the current cursor position |
| ESC                 | Exit fullscreen |
| Mouse Wheel         | Zoom in/out on grid cell size |
| Middle Click        | Center view on cursor |
| Arrow Keys          | Move the entire map |
| F11                 | Enter fullscreen mode |

---

## 🧠 Technical Philosophy

This project follows a clear principle:

> **“Core logic in C, container structures in C++ STL.”**

- All main logic is written in **pure C style**, using `struct`s and functions  
- There are no classes, inheritance, or RAII  
- Only the data containers (`std::map`, `std::vector`, `unordered_map`)  
  are borrowed from C++ STL for safety and clarity
- The C interface is designed to be easily accessible from Python

---

## 🧩 Architecture Overview

| Component            | Role |
|----------------------|------|
| `GridViewer`         | Top-level UI container |
| `GridCanvas`         | Renders the grid and handles mouse events |
| `MouseInputHandler`  | Dedicated mouse event processor |
| `GridMap`            | Terrain and cell state manager |
| `NPC`                | Goal logic and pathfinding per entity |
| `BottomDockingPanel` | Logs and performance metrics |
| `Toolbar`, `MenuBar` | Reset, configuration, fullscreen toggle |

---

## 🛠 Tech Stack

| Stack              | Description |
|--------------------|-------------|
| **C (with C++ STL)** | Core logic in C, with STL containers used only for data management |
| **Python Integration** | Exposes C logic to Python directly |
| **PySide6 (Qt)**    | GUI framework |
| **Multithreading**  | Separate UI and pathfinding execution |

---

## 🔮 Planned Features

- Better visual path tracing  
- NPC collision / cooperation logic  
- Smoother movement animations  
- Memory-based AI routines  
- In-editor map creation and persistence

---

## 💬 Developer Note

This project began as a visual demo of pathfinding,  
but it's gradually evolving into a small, living simulation.  
NPCs are not just dots on a screen –  
they are entities with purpose, logic, and reactivity.

If you'd like to ask a question or leave feedback,  
please open an Issue or start a Discussion.

---

## ▶️ Run the Demo

python byul_demo.py

## 📄 License
This project is part of Byul's World,
released for educational and research use only.
Commercial use or redistribution is not allowed.

See the LICENSE file for full terms.

© 2025 ByulPapa (byuldev@outlook.kr)
All rights reserved.