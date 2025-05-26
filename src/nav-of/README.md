# Realtime Optical Flow Estimation

This project provides a real-time optical flow estimation pipeline using the Horn-Schunck algorithm combined with a Kalman filter for velocity smoothing. The goal is to simulate and analyze movement detected between consecutive video frames, particularly from drone footage.

---

## 🚀 Features

- Horn-Schunck optical flow implementation from scratch (no OpenCV dense flow).
- Real-time frame-by-frame velocity estimation.
- Kalman filtering for smoother speed output.
- Modular and clean C++17 design.
- Ready for integration with cameras, simulations, or drone feeds.

---

## 📦 Project Structure

```
realtime-optical-flow/
├── CMakeLists.txt
├── README.md
├── data/                 # Input videos or frames
└── src/
    ├── main.cpp
    ├── optical_flow_realtime.hpp/.cpp
    ├── horn_schunck.hpp/.cpp
    ├── kalman_filter.hpp/.cpp
    ├── utils.hpp/.cpp
```

---

## 🔧 Requirements

- C++17 or higher
- OpenCV (version ≥ 4.0)
- CMake

---

## 💻 Installation & Build

### 🐧 Linux (Ubuntu / Debian-based)

1. **Install OpenCV and build tools:**

   ```bash
   sudo apt update
   sudo apt install build-essential cmake libopencv-dev
   ```

2. **Build the project:**

   ```bash
   mkdir build && cd build
   cmake ..
   make
   ```

3. **Run it:**

   ```bash
   ./realtime_optical_flow
   ```

---

### 🪟 Windows (via vcpkg)

1. **Install [vcpkg](https://github.com/microsoft/vcpkg):**

   ```bash
   git clone https://github.com/microsoft/vcpkg.git
   cd vcpkg
   .\bootstrap-vcpkg.bat
   ```

2. **Integrate with CMake:**

   ```bash
   .\vcpkg integrate install
   ```

3. **Install OpenCV:**

   ```bash
   .\vcpkg install opencv4
   ```

4. **Build the project:**

   In your `CMakeLists.txt`, make sure to include:

   ```cmake
   set(CMAKE_TOOLCHAIN_FILE "C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake")
   find_package(OpenCV REQUIRED)
   ```

   Then run:

   ```bash
   mkdir build && cd build
   cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
   cmake --build .
   ```

5. **Run the app:**

   ```bash
   ./Debug/realtime_optical_flow.exe
   ```

---

## ▶️ Usage

The app reads frames from `data/input.mp4`. You can change the path in `main.cpp`.

It logs estimated speed (in m/s) to the terminal and optionally shows a video preview (`ESC` to exit).

---

## ⚙️ Configuration

Modify directly in `main.cpp`:

```cpp
OpticalFlowRealtimeProcessor processor(
    30.0f,   // FPS
    28.0f,   // Drone altitude (m)
    82.2f,   // Camera FOV (deg)
    1080     // Image height (px)
);
```

---

## 🔮 Future Improvements

- CLI argument parsing (`--input`, `--fps`, etc.)
- Webcam/live stream support
- Save results to CSV
- Visualize optical flow vectors

---

## 📄 License

MIT License — free to use, modify, and share.

---

## 🤝 Contributions

Pull requests are welcome! If you improve this tool or find bugs, feel free to open an issue.
