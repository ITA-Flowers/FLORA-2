#!/usr/bin/env python3
"""
Camera Data Collector for AirSim

This script connects to AirSim and collects camera images during flight.
Images are saved with synchronized timestamps for later processing.
"""

import os
import time
import csv
import argparse
import numpy as np
import airsim
import cv2
import math
from datetime import datetime


class CameraCollector:
    def __init__(self, output_dir, camera_name="0", fps=30, width=1280, height=720, fov=90, duration=None):
        """
        Initialize the camera data collector.
        
        Args:
            output_dir (str): Directory to save collected images
            camera_name (str): Camera name to use in AirSim
            fps (int): Frame rate in frames per second
            width (int): Image width in pixels
            height (int): Image height in pixels
            fov (float): Field of view in degrees
            duration (float, optional): Duration of data collection in seconds
        """
        self.output_dir = output_dir
        self.camera_name = camera_name
        self.fps = fps
        self.width = width
        self.height = height
        self.fov = fov
        self.duration = duration
        self.interval = 1.0 / fps
        
        # Connect to AirSim
        self.client = airsim.MultirotorClient()
        self.client.confirmConnection()
        print(f"Connected to AirSim! Client Ver:{self.client.getClientVersion()} (Min Req: {self.client.getMinRequiredClientVersion()}), Server Ver:{self.client.getServerVersion()} (Min Req: {self.client.getMinRequiredServerVersion()})")
        
        # Try to set camera parameters (if available)
        self.configure_camera()
        
        # Create output directories if they don't exist
        self.images_dir = os.path.join(output_dir, "images")
        os.makedirs(self.images_dir, exist_ok=True)
        
        # Define file paths
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        self.metadata_file = os.path.join(output_dir, f"camera_metadata_{timestamp}.csv")
        
        # Initialize CSV writer for metadata
        self.csv_file = open(self.metadata_file, 'w', newline='')
        self.csv_writer = csv.writer(self.csv_file)
        
        # Write header
        self.csv_writer.writerow([
            'timestamp', 
            'frame_number',
            'image_filename',
            'position_x', 'position_y', 'position_z',
            'orientation_w', 'orientation_x', 'orientation_y', 'orientation_z'
        ])
        
        self.start_time = None
        self.frame_count = 0
        
        # Save camera parameters to a file
        self.camera_params_file = os.path.join(output_dir, f"camera_parameters_{timestamp}.json")
        self.save_camera_parameters()
    
    def configure_camera(self):
        """Configure the camera parameters in AirSim."""
        try:
            # Ustawienie kamery w dół (pitch = -90 stopni)
            import math
            camera_pose = airsim.Pose(
                airsim.Vector3r(0, 0, 0),  # pozycja (brak offsetu)
                airsim.to_quaternion(math.radians(-90), 0, 0)  # obrót (pitch = -90 stopni)
            )
            
            print("Setting camera pose to look downward (pitch = -90°)")
            self.client.simSetCameraPose(self.camera_name, camera_pose)
            
            # Try to set camera FOV if method is available
            if hasattr(self.client, "simSetCameraFov"):
                self.client.simSetCameraFov(self.camera_name, self.fov)
                print(f"Set camera FOV to {self.fov} degrees")
            else:
                print("Method simSetCameraFov not available. Camera FOV settings should be configured in AirSim settings.json")
            
            # Note: Camera resolution is set in AirSim settings.json
            print(f"Camera resolution should be configured in settings.json (requested: {self.width}x{self.height})")
            
        except Exception as e:
            print(f"Warning: Could not configure camera: {e}")
            print("Continuing with default camera settings")
        
    def save_camera_parameters(self):
        """Save camera intrinsic parameters to a file."""
        import json
        
        # Calculate focal length from FOV and width
        # focal_length = (width / 2) / tan(FOV / 2)
        focal_length_x = (self.width / 2) / np.tan(np.radians(self.fov / 2))
        focal_length_y = (self.height / 2) / np.tan(np.radians(self.fov / 2))
        
        # Principal point is typically at the center of the image
        cx = self.width / 2
        cy = self.height / 2
        
        # Camera matrix (intrinsic parameters)
        camera_matrix = {
            "fx": float(focal_length_x),
            "fy": float(focal_length_y),
            "cx": float(cx),
            "cy": float(cy)
        }
        
        # Additional parameters
        camera_params = {
            "width": self.width,
            "height": self.height,
            "fov_degrees": self.fov,
            "fps": self.fps,
            "camera_name": self.camera_name,
            "camera_matrix": camera_matrix,
            # Assuming no distortion in simulator
            "distortion_coefficients": [0.0, 0.0, 0.0, 0.0, 0.0]
        }
        
        with open(self.camera_params_file, 'w') as f:
            json.dump(camera_params, f, indent=4)
        
        print(f"Camera parameters saved to {self.camera_params_file}")
        
    def start_collection(self):
        """Start collecting images from the camera."""
        print(f"Starting camera collection at {self.fps} FPS...")
        print(f"Images will be saved to {self.images_dir}")
        print(f"Camera parameters: {self.width}x{self.height}, FOV: {self.fov}°")
        
        self.start_time = time.time()
        next_frame_time = self.start_time
        
        # Get first image to verify dimensions
        first_response = self.client.simGetImages([
            airsim.ImageRequest(self.camera_name, airsim.ImageType.Scene, False, False)
        ])
        
        if first_response:
            print(f"First image received: {len(first_response[0].image_data_uint8)} bytes")
            print(f"Image dimensions: {first_response[0].width}x{first_response[0].height}")
            
            # Update dimensions if they differ from requested
            if first_response[0].width != self.width or first_response[0].height != self.height:
                print(f"WARNING: Actual image dimensions ({first_response[0].width}x{first_response[0].height}) " + 
                      f"differ from requested ({self.width}x{self.height})")
                print("Using actual dimensions for processing")
                self.width = first_response[0].width
                self.height = first_response[0].height
        
        try:
            while True:
                current_time = time.time()
                
                # Check if we've reached the duration limit
                if self.duration and (current_time - self.start_time) >= self.duration:
                    print(f"Reached duration limit of {self.duration} seconds")
                    break
                
                # Wait until next frame time
                if current_time < next_frame_time:
                    time.sleep(0.001)  # Small sleep to avoid CPU spinning
                    continue
                
                # Aktualizuj pozycję kamery, aby symulować gimbala
                self.update_camera_gimbal()
                
                # Get camera image
                responses = self.client.simGetImages([
                    airsim.ImageRequest(self.camera_name, airsim.ImageType.Scene, False, False)
                ])
                
                if responses:
                    try:
                        # Convert the image data to numpy array
                        img1d = np.frombuffer(responses[0].image_data_uint8, dtype=np.uint8)
                        
                        # Reshape to expected dimensions (verified from first frame)
                        img_rgb = img1d.reshape(self.height, self.width, 3)
                        
                        # Get drone position and orientation
                        state = self.client.getMultirotorState()
                        position = state.kinematics_estimated.position
                        orientation = state.kinematics_estimated.orientation
                        
                        # Generate filename
                        elapsed = current_time - self.start_time
                        filename = f"frame_{self.frame_count:06d}_{elapsed:.6f}.png"
                        filepath = os.path.join(self.images_dir, filename)
                        
                        # Save the image (convert from RGB to BGR for OpenCV)
                        cv2.imwrite(filepath, cv2.cvtColor(img_rgb, cv2.COLOR_RGB2BGR))
                        
                        # Record metadata
                        self.csv_writer.writerow([
                            elapsed,  # timestamp (seconds since start)
                            self.frame_count,
                            filename,
                            position.x_val, position.y_val, position.z_val,
                            orientation.w_val, orientation.x_val, orientation.y_val, orientation.z_val
                        ])
                        
                        self.frame_count += 1
                        if self.frame_count % self.fps == 0:
                            print(f"Captured {self.frame_count} frames ({elapsed:.2f} seconds)")
                    
                    except Exception as e:
                        print(f"Error processing image: {e}")
                        import traceback
                        traceback.print_exc()
                
                # Schedule next frame
                next_frame_time += self.interval
                
        except KeyboardInterrupt:
            print("\nImage collection interrupted by user")
        finally:
            elapsed_time = time.time() - self.start_time
            self.csv_file.close()
            print(f"Image collection finished. Captured {self.frame_count} frames over {elapsed_time:.2f} seconds")
            print(f"Average frame rate: {self.frame_count / elapsed_time:.2f} FPS")
            print(f"Metadata saved to {self.metadata_file}")
            
    def update_camera_gimbal(self):
        """Aktualizuj pozycję kamery, aby zawsze była skierowana prostopadle do podłoża."""
        try:
            # Pobierz aktualną orientację drona
            drone_state = self.client.getMultirotorState()
            drone_orientation = drone_state.kinematics_estimated.orientation
            
            # Konwersja kwaterniona drona na kąty Eulera
            drone_orientation_euler = airsim.to_eularian_angles(drone_orientation)
            pitch = drone_orientation_euler[0]
            roll = drone_orientation_euler[1]
            yaw = drone_orientation_euler[2]
            
            # Kąt dla kamery jest odwrotnością kąta drona - aby kompensować jego ruch
            # Dla pitch, dodajemy -π/2 (-90°), aby kamera była skierowana w dół
            camera_pitch = -math.pi/2  # Kompensacja pitch + 90° w dół
            camera_roll = 0               # Przeciwny roll
            camera_yaw = 0                  # Zachowanie tego samego yaw
            
            # Konwersja na kwaternion
            camera_orientation = airsim.to_quaternion(camera_pitch, camera_roll, camera_yaw)
            
            # Ustaw pozę kamery
            camera_pose = airsim.Pose(airsim.Vector3r(0, 0, 0), camera_orientation)
            self.client.simSetCameraPose(self.camera_name, camera_pose)
            
            # Debugowanie - wyświetl co 5 klatek
            if self.frame_count % 5 == 0:
                print(f"Drone orientation: pitch={math.degrees(pitch):.1f}°, roll={math.degrees(roll):.1f}°, yaw={math.degrees(yaw):.1f}°")
                print(f"Camera orientation: pitch={math.degrees(camera_pitch):.1f}°, roll={math.degrees(camera_roll):.1f}°, yaw={math.degrees(camera_yaw):.1f}°")
            
        except Exception as e:
            print(f"Warning: Could not update camera gimbal: {e}")

def main():
    parser = argparse.ArgumentParser(description='Collect camera images from AirSim')
    parser.add_argument('--output', type=str, required=True, help='Output directory for collected images')
    parser.add_argument('--camera', type=str, default="0", help='Camera name in AirSim (default: 0)')
    parser.add_argument('--fps', type=int, default=30, help='Frame rate in FPS (default: 30)')
    parser.add_argument('--width', type=int, default=1280, help='Image width in pixels (default: 1280)')
    parser.add_argument('--height', type=int, default=720, help='Image height in pixels (default: 720)')
    parser.add_argument('--fov', type=float, default=90, help='Field of view in degrees (default: 90)')
    parser.add_argument('--duration', type=float, help='Duration of collection in seconds (default: until interrupted)')
    
    args = parser.parse_args()
    
    collector = CameraCollector(
        args.output, 
        args.camera, 
        args.fps, 
        args.width, 
        args.height,
        args.fov,
        args.duration
    )
    
    collector.start_collection()


if __name__ == "__main__":
    main()