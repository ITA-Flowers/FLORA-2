#!/usr/bin/env python3
"""
Unified Flight Controller for AirSim
Supports: square, straight, and circle flight modes
"""

import os
import sys
import math
import time
import signal
import argparse
import subprocess
import airsim

class FlightController:
    def __init__(self, mode, size=50, altitude=20, speed=5, mute_collectors=False):
        self.mode = mode
        self.size = size
        self.altitude = altitude
        self.speed = speed
        self.mute_collectors = mute_collectors
        self.client = None
        self.data_processes = []

        self.connect_to_airsim()

    def connect_to_airsim(self):
        print("Connecting to AirSim...")
        self.client = airsim.MultirotorClient()
        self.client.confirmConnection()

        vehicles = self.client.listVehicles()
        if not vehicles:
            raise RuntimeError("No vehicles found in AirSim")

        print("Connection established.")

    def configure_camera(self):
        pose = airsim.Pose(airsim.Vector3r(0, 0, 0), airsim.to_quaternion(-math.pi/2, 0, 0))
        self.client.simSetCameraPose("0", pose)
        print("Camera set to downward orientation.")

    def start_data_collection(self, output_dir, duration):
        os.makedirs(output_dir, exist_ok=True)

        current_dir = os.path.dirname(os.path.abspath(__file__))
        venv_dir = os.path.join(current_dir, ".venv")
        python_exe = sys.executable

        if os.name == 'nt':
            alt_python = os.path.join(venv_dir, "Scripts", "python.exe")
        else:
            alt_python = os.path.join(venv_dir, "bin", "python")
        if os.path.exists(alt_python):
            python_exe = alt_python

        utils_dir = os.path.join(current_dir, "utils")
        imu_script = os.path.join(utils_dir, "imu_gps_collector.py")
        cam_script = os.path.join(utils_dir, "camera_collector.py")

        imu_cmd = [python_exe, imu_script, "--output", output_dir, "--frequency", "100", "--duration", str(duration + 30)]
        cam_cmd = [python_exe, cam_script, "--output", output_dir, "--fps", "30", "--width", "1280", "--height", "720", "--fov", "90", "--duration", str(duration + 30)]

        self.data_processes.append(subprocess.Popen(imu_cmd))
        self.data_processes.append(subprocess.Popen(cam_cmd))
        print("Data collection started.")

    def stop_data_collection(self):
        for proc in self.data_processes:
            proc.terminate()
            proc.wait()
        self.data_processes = []
        print("Data collection stopped.")

    def take_off(self):
        print(f"   - Taking off to {self.altitude}m...")
        self.client.takeoffAsync().join()
        self.client.moveToZAsync(-self.altitude, self.speed).join()
        time.sleep(5)

    def land(self):
        print("   - Starting landing procedure...")
        
        current_pos = self.client.getMultirotorState().kinematics_estimated.position
        
        print("   - Descending to 2m altitude...")
        self.client.moveToPositionAsync(
            current_pos.x_val, 
            current_pos.y_val, 
            -2.0,
            self.speed
        ).join()
        
        height = -self.client.getMultirotorState().kinematics_estimated.position.z_val
        print(f"   - Reached altitude of {height:.1f}m")
        
        print("   - Executing final landing...")
        self.client.landAsync().join()
        
        landed_state = self.client.getMultirotorState().landed_state
        print(f"   - Landing completed, state: {landed_state}")
        
        print("   - Disarming drone...")
        self.client.armDisarm(False)
        self.client.enableApiControl(False)
        print("   - Landing procedure completed")

    def fly_straight(self):
        state = self.client.getMultirotorState().kinematics_estimated.position
        target = airsim.Vector3r(state.x_val + self.size, state.y_val, -self.altitude)
        
        print(f"   - Flying straight to {target}...")
        self.client.moveToPositionAsync(target.x_val, target.y_val, target.z_val, self.speed).join()
        
        print("   - Hovering...")
        time.sleep(3)
        
        print(f"   - Flying straight to {state}...")
        self.client.moveToPositionAsync(state.x_val, state.y_val, -self.altitude, self.speed).join()
        
        print("   - Hovering...")
        time.sleep(3)

    def fly_square(self):
        state = self.client.getMultirotorState().kinematics_estimated.position
        s = self.size / 2
        corners = [
            airsim.Vector3r(state.x_val + s, state.y_val + s, -self.altitude),
            airsim.Vector3r(state.x_val + s, state.y_val - s, -self.altitude),
            airsim.Vector3r(state.x_val - s, state.y_val - s, -self.altitude),
            airsim.Vector3r(state.x_val - s, state.y_val + s, -self.altitude),
            airsim.Vector3r(state.x_val + s, state.y_val + s, -self.altitude)
        ]
        for corner in corners:
            self.client.moveToPositionAsync(corner.x_val, corner.y_val, corner.z_val, self.speed).join()
            time.sleep(3)

    def fly_circle(self):
        state = self.client.getMultirotorState().kinematics_estimated.position
        steps = 24
        for i in range(steps + 1):
            angle = 2 * math.pi * i / steps
            x = state.x_val + self.size * math.cos(angle)
            y = state.y_val + self.size * math.sin(angle)
            self.client.moveToPositionAsync(x, y, -self.altitude, self.speed).join()
        time.sleep(3)

    def execute(self, output_dir):
        duration = (4 * self.size / self.speed) + 30
        self.client.reset()
        time.sleep(2)
        
        print("   - Arming...")
        self.client.enableApiControl(True)
        self.client.armDisarm(True)
        time.sleep(2)
        print("   - Armed.")
        
        self.configure_camera()
        
        self.start_data_collection(output_dir, duration)
        time.sleep(3)
        self.take_off()

        if self.mode == "square":
            self.fly_square()
        elif self.mode == "straight":
            self.fly_straight()
        elif self.mode == "circle":
            self.fly_circle()
        else:
            raise ValueError(f"Unknown flight mode: {self.mode}")

        self.land()
        time.sleep(5)
        self.stop_data_collection()


def signal_handler(sig, frame):
    print("Ctrl+C detected, shutting down...")
    sys.exit(0)

def main():
    signal.signal(signal.SIGINT, signal_handler)
    parser = argparse.ArgumentParser(description="AirSim Unified Flight Script")
    parser.add_argument("--output", required=True)
    parser.add_argument("--mode", required=True, choices=["square", "straight", "circle"])
    parser.add_argument("--size", type=float, default=50)
    parser.add_argument("--altitude", type=float, default=20)
    parser.add_argument("--speed", type=float, default=5)
    args = parser.parse_args()

    controller = FlightController(mode=args.mode, size=args.size, altitude=args.altitude, speed=args.speed)
    controller.execute(args.output)

if __name__ == "__main__":
    main()
