#!/usr/bin/env python3
"""
Square Flight Controller for AirSim

This script performs a square flight pattern in AirSim while
configuring a downward-facing camera for data collection.
"""

import os
import time
import argparse
import math
import airsim
import subprocess
import threading
import signal
import sys


class SquareFlightController:
    def __init__(self, square_size=50, altitude=20, speed=5):
        """
        Initialize the square flight controller.
        
        Args:
            square_size (float): Size of the square in meters
            altitude (float): Flight altitude in meters
            speed (float): Flight speed in m/s
        """
        self.square_size = square_size
        self.altitude = altitude
        self.speed = speed
        self.client = None
        self.data_processes = []
        
        # Connect to AirSim
        self.connect_to_airsim()
    
    def connect_to_airsim(self):
        """Connect to AirSim and prepare the drone."""
        print("[*] Connecting to AirSim...")
        
        try:
            self.client = airsim.MultirotorClient()
            self.client.confirmConnection()
            
            # Check if the vehicle is present
            vehicle_names = self.client.listVehicles()
            print(f"  * Available vehicles: {vehicle_names}")
            
            if not vehicle_names:
                print("[!] No vehicles found in the simulation. Make sure a drone exists in the scene.")
                print("[?] You may need to reset the simulation or check your AirSim settings.")
                raise Exception("No vehicles available in AirSim")
            
            # Default to the first vehicle if multiple exist
            if len(vehicle_names) > 1:
                print(f"  * Multiple vehicles found. Using the first one: {vehicle_names[0]}")
            
            # Take control of the drone
            self.client.enableApiControl(True)
            self.client.armDisarm(True)  # Arm the drone
            
            # Check if the drone is in a valid state
            state = self.client.getMultirotorState()
            print(f"  * Current drone state: {state.landed_state}")
            
            # Configure camera to face downward
            self.configure_camera()
            
            print("[*] Successfully connected to AirSim and prepared the drone")
        
        except Exception as e:
            print(f"[!] Error connecting to AirSim: {e}")
            print("\n[?] Troubleshooting tips:")
            print("    1. Make sure AirSim is running and fully loaded")
            print("    2. Check if the AirSim settings.json is configured correctly")
            print("    3. Try resetting the simulation from the AirSim menu")
            print("    4. Restart AirSim if the issue persists\n")
            raise
    
    def configure_camera(self):
        """Configure the camera to face downward."""
        # Get the default camera pose
        camera_info = self.client.simGetCameraInfo("0")
        
        # Create a camera pose with downward orientation
        # Rotate 90 degrees around x-axis to point downward
        camera_pose = airsim.Pose(
            airsim.Vector3r(0, 0, 0),  # No position offset
            airsim.to_quaternion(math.pi/2, 0, 0)  # 90 degrees around x-axis
        )
        
        # Set the camera pose
        self.client.simSetCameraPose("0", camera_pose)
        print("[*] Camera configured to face downward")
    
    def start_data_collection(self, output_dir, duration):
        """
        Start data collection processes.
        
        Args:
            output_dir (str): Directory to save collected data
            duration (float): Duration of the flight in seconds
        """
        # Create output directory
        os.makedirs(output_dir, exist_ok=True)
        
        # Determine paths to scripts and Python interpreter in virtual environment
        current_dir = os.path.dirname(os.path.abspath(__file__))
        project_root = os.path.abspath(os.path.join(current_dir, ".."))
        venv_dir = os.path.join(project_root, ".venv")
        
        # Find Python executable in the virtual environment
        if os.name == 'nt':  # Windows
            python_exe = os.path.join(venv_dir, "Scripts", "python.exe")
        else:  # Linux/Mac
            python_exe = os.path.join(venv_dir, "bin", "python")
            
        # Check if the virtual environment exists
        if not os.path.exists(python_exe):
            print(f"[Warning]: Virtual environment Python not found at {python_exe}")
            print("  * Falling back to system Python")
            python_exe = sys.executable
        else:
            print(f"  [*] Using Python from virtual environment: {python_exe}")
        
        # Define script paths
        utils_dir = os.path.join(project_root, "utils")
        imu_gps_script = os.path.join(utils_dir, "imu_gps_collector.py")
        camera_script = os.path.join(utils_dir, "camera_collector.py")
        
        # Verify script paths
        if not os.path.exists(imu_gps_script):
            print(f"[Warning]: IMU/GPS collector script not found at {imu_gps_script}")
            print(f"  * Looking in current directory...")
            imu_gps_script = os.path.join(current_dir, "imu_gps_collector.py")
        
        if not os.path.exists(camera_script):
            print(f"[Warning]: Camera collector script not found at {camera_script}")
            print(f"  * Looking in current directory...")
            camera_script = os.path.join(current_dir, "camera_collector.py")
        
        # Start IMU/GPS data collection
        imu_gps_cmd = [
            python_exe, imu_gps_script,
            "--output", output_dir,
            "--frequency", "100",
            "--duration", str(duration + 30)  # Add buffer time
        ]
        
        print(f" * Starting IMU/GPS collector...")
        imu_gps_process = subprocess.Popen(imu_gps_cmd)
        self.data_processes.append(imu_gps_process)
        
        # Start camera data collection
        camera_cmd = [
            python_exe, camera_script,
            "--output", output_dir,
            "--fps", "30",
            "--width", "1280",
            "--height", "720",
            "--fov", "90",
            "--duration", str(duration + 30)  # Add buffer time
        ]
        
        print(f" * Starting camera collector...")
        camera_process = subprocess.Popen(camera_cmd)
        self.data_processes.append(camera_process)
        
        print("[*] Data collection started")
    
    def stop_data_collection(self):
        """Stop all data collection processes."""
        for process in self.data_processes:
            process.terminate()
            process.wait()
        
        self.data_processes = []
        print("[*] Data collection stopped")
    
    def take_off(self):
        """Take off and hover at the specified altitude."""
        print(f"[*] Taking off to {self.altitude} meters...")
        self.client.takeoffAsync().join()
        self.client.moveToZAsync(-self.altitude, self.speed).join()
        
        # Hover a bit to stabilize
        print("[*] Stabilizing...")
        time.sleep(10)
    
    def fly_square(self):
        """Fly in a square pattern at the specified altitude."""
        print(f"[*] Flying a square with side length {self.square_size} meters...")
        
        # Get starting position
        state = self.client.getMultirotorState()
        start_pos = state.kinematics_estimated.position
        
        # Calculate the four corners of the square relative to starting position
        half_size = self.square_size / 2
        corners = [
            airsim.Vector3r(start_pos.x_val + half_size, start_pos.y_val - half_size, -self.altitude),
            airsim.Vector3r(start_pos.x_val + half_size, start_pos.y_val + half_size, -self.altitude),
            airsim.Vector3r(start_pos.x_val - half_size, start_pos.y_val + half_size, -self.altitude),
            airsim.Vector3r(start_pos.x_val - half_size, start_pos.y_val - half_size, -self.altitude),
            airsim.Vector3r(start_pos.x_val, start_pos.y_val, -self.altitude)  # Back to start
        ]
        
        # Fly to each corner
        for i, corner in enumerate(corners):
            print(f"    - Flying to corner {i+1}/5...")
            self.client.moveToPositionAsync(
                corner.x_val, corner.y_val, corner.z_val, self.speed
            ).join()
            
            # Hover briefly at each corner
            print("     - Hovering at corner...")
            time.sleep(10)
    
    def land(self):
        """Land the drone."""
        print("[*] Landing...")
        self.client.landAsync().join()
        
        # Disarm
        self.client.armDisarm(False)
        self.client.enableApiControl(False)
        print("[*] Flight complete")
    
    def execute_flight(self, output_dir):
        """
        Execute the complete square flight mission.
        
        Args:
            output_dir (str): Directory to save collected data
        """
        # Calculate flight duration based on square size and speed
        perimeter = 4 * self.square_size
        flight_time = perimeter / self.speed
        hover_time = 4 * 3  # 3 seconds hover at each corner
        total_duration = flight_time + hover_time + 10  # +10 seconds buffer
        
        print(f"[*] Estimated flight duration: {total_duration:.1f} seconds")
        
        try:
            # Check if client is connected
            if self.client is None:
                print("[*] Reconnecting to AirSim...")
                self.connect_to_airsim()
            
            # Reset the drone to starting position
            print("[*] Resetting drone position...")
            self.client.reset()
            time.sleep(2)  # Wait for reset to complete
            
            # Start data collection
            self.start_data_collection(output_dir, total_duration)
            
            # Give data collection processes time to initialize
            time.sleep(3)
            
            # Check drone readiness
            is_armed = self.client.isApiControlEnabled()
            if not is_armed:
                print("[*] Enabling API control and arming drone...")
                self.client.enableApiControl(True)
                self.client.armDisarm(True)
                time.sleep(1)
            
            # Execute flight
            self.take_off()
            self.fly_square()
            self.land()
            
            # Let data collection finish
            print("[***] Waiting for data collection to complete...")
            time.sleep(5)
            
        except Exception as e:
            print(f"[!] Error during flight: {e}")
            import traceback
            traceback.print_exc()
        finally:
            # Stop data collection
            self.stop_data_collection()
            
            # Create video from collected images
            self.assemble_video(output_dir)
    
    def assemble_video(self, output_dir):
        """
        Assemble video from collected images.
        
        Args:
            output_dir (str): Directory containing collected data
        """
        print("[*] Assembling video from collected images...")
        
        # Find the metadata file
        import glob
        metadata_files = glob.glob(os.path.join(output_dir, "camera_metadata_*.csv"))
        imu_gps_files = glob.glob(os.path.join(output_dir, "imu_gps_data_*.csv"))
        
        if not metadata_files or not imu_gps_files:
            print("[!] Error: Could not find metadata or IMU/GPS files")
            return
        
        # Determine paths to scripts and Python interpreter in virtual environment
        current_dir = os.path.dirname(os.path.abspath(__file__))
        project_root = os.path.abspath(os.path.join(current_dir, ".."))
        venv_dir = os.path.join(project_root, ".venv")
        
        # Find Python executable in the virtual environment
        if os.name == 'nt':  # Windows
            python_exe = os.path.join(venv_dir, "Scripts", "python.exe")
        else:  # Linux/Mac
            python_exe = os.path.join(venv_dir, "bin", "python")
            
        # Check if the virtual environment exists
        if not os.path.exists(python_exe):
            print(f"[Warning]: Virtual environment Python not found at {python_exe}")
            print("  * Falling back to system Python")
            python_exe = sys.executable
        
        # Define script path
        utils_dir = os.path.join(project_root, "utils")
        video_script = os.path.join(utils_dir, "video_assembler.py")
        
        # Verify script path
        if not os.path.exists(video_script):
            print(f"[Warning]: Video assembler script not found at {video_script}")
            print(f"  * Looking in current directory...")
            video_script = os.path.join(current_dir, "video_assembler.py")
        
        # Assemble the video
        video_cmd = [
            python_exe, video_script,
            "--images", os.path.join(output_dir, "images"),
            "--metadata", metadata_files[0],
            "--output", os.path.join(output_dir, "square_flight.mp4"),
            "--fps", "28",
            "--overlay", imu_gps_files[0],
            "--sync-data"
        ]
        
        print(f"  * Running video assembler...")
        subprocess.run(video_cmd)
        print(f"[*] Video assembled: {os.path.join(output_dir, 'square_flight.mp4')}")


def signal_handler(sig, frame):
    print("[!] Ctrl+C detected, stopping flight...")
    sys.exit(0)

def main():
    parser = argparse.ArgumentParser(description='Perform a square flight pattern in AirSim')
    parser.add_argument('--output', type=str, required=True, help='Output directory for collected data')
    parser.add_argument('--size', type=float, default=50, help='Size of the square in meters (default: 50)')
    parser.add_argument('--altitude', type=float, default=20, help='Flight altitude in meters (default: 20)')
    parser.add_argument('--speed', type=float, default=5, help='Flight speed in m/s (default: 5)')
    parser.add_argument('--no-flight', action='store_true', help='Skip the actual flight and only collect data from an existing simulation')
    
    args = parser.parse_args()
    
    # Register signal handler for Ctrl+C
    signal.signal(signal.SIGINT, signal_handler)
    
    try:
        controller = SquareFlightController(args.size, args.altitude, args.speed)
        
        if args.no_flight:
            print("[*] Skipping flight execution, only collecting data from simulation...")
            # Still need to collect data and assemble video
            controller.start_data_collection(args.output, 60)  # Collect for 60 seconds
            print("[?] Press Ctrl+C to stop data collection...")
            try:
                while True:
                    time.sleep(1)
            except KeyboardInterrupt:
                print("\n  * Stopping data collection...")
            finally:
                controller.stop_data_collection()
                controller.assemble_video(args.output)
        else:
            controller.execute_flight(args.output)
    
    except Exception as e:
        print(f"\n[!] Error in main function: {e}")
        print("[?] See the troubleshooting guide for solutions to common problems.")
        if input("Show detailed error? (y/n): ").lower() == 'y':
            import traceback
            traceback.print_exc()


if __name__ == "__main__":
    main()