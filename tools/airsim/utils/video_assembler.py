#!/usr/bin/env python3
"""
Video Assembler for AirSim

This script assembles collected camera frames into a video file.
It can also overlay additional information such as IMU data if available.
"""

import os
import csv
import argparse
import numpy as np
import cv2
import json
from glob import glob
from tqdm import tqdm


class VideoAssembler:
    def __init__(self, images_dir, metadata_file, output_file, fps=30, overlay_data=None):
        """
        Initialize the video assembler.
        
        Args:
            images_dir (str): Directory containing the captured images
            metadata_file (str): Path to the camera metadata CSV file
            output_file (str): Path to the output video file
            fps (int): Frames per second for the output video
            overlay_data (str, optional): Path to IMU/GPS data to overlay
        """
        self.images_dir = images_dir
        self.metadata_file = metadata_file
        self.output_file = output_file
        self.fps = fps
        self.overlay_data = overlay_data
        
        # Load metadata
        self.metadata = self._load_metadata()
        
        # Load IMU/GPS data if provided
        self.sensor_data = self._load_sensor_data() if overlay_data else None
    
    def _load_metadata(self):
        """Load camera metadata from CSV file."""
        metadata = []
        with open(self.metadata_file, 'r') as f:
            reader = csv.DictReader(f)
            for row in reader:
                metadata.append(row)
        
        print(f"Loaded metadata for {len(metadata)} frames")
        return metadata
    
    def _load_sensor_data(self):
        """Load IMU/GPS data for overlay."""
        sensor_data = []
        with open(self.overlay_data, 'r') as f:
            reader = csv.DictReader(f)
            for row in reader:
                sensor_data.append(row)
        
        print(f"Loaded sensor data with {len(sensor_data)} entries")
        return sensor_data
    
    def _find_nearest_sensor_data(self, timestamp):
        """Find the sensor data entry closest to the given timestamp."""
        if not self.sensor_data:
            return None
        
        # Convert timestamp to float for comparison
        target_time = float(timestamp)
        
        # Find entry with closest timestamp
        closest_entry = min(self.sensor_data, 
                           key=lambda x: abs(float(x['timestamp']) - target_time))
        
        return closest_entry
    
    def _overlay_sensor_data(self, image, sensor_data):
        """Overlay sensor data on the image."""
        if not sensor_data:
            return image
        
        # Create a copy of the image
        result = image.copy()
        
        # Extract relevant data
        try:
            accel_x = float(sensor_data.get('imu_linear_acceleration_x', 0))
            accel_y = float(sensor_data.get('imu_linear_acceleration_y', 0))
            accel_z = float(sensor_data.get('imu_linear_acceleration_z', 0))
            
            gyro_x = float(sensor_data.get('imu_angular_velocity_x', 0))
            gyro_y = float(sensor_data.get('imu_angular_velocity_y', 0))
            gyro_z = float(sensor_data.get('imu_angular_velocity_z', 0))
            
            # Check if GPS data is available (not NaN)
            gps_lat = sensor_data.get('gps_latitude', 'N/A')
            gps_lon = sensor_data.get('gps_longitude', 'N/A')
            gps_alt = sensor_data.get('gps_altitude', 'N/A')
            
            has_gps = (gps_lat != 'N/A' and gps_lon != 'N/A' and 
                      not (isinstance(gps_lat, float) and np.isnan(gps_lat)) and
                      not (isinstance(gps_lon, float) and np.isnan(gps_lon)))
        except (ValueError, TypeError):
            return result
        
        # Add text overlay
        font = cv2.FONT_HERSHEY_SIMPLEX
        font_scale = 0.6
        color = (255, 255, 255)
        thickness = 1
        line_spacing = 25
        
        # Background for text
        overlay = result.copy()
        cv2.rectangle(overlay, (10, 10), (350, 160), (0, 0, 0), -1)
        cv2.addWeighted(overlay, 0.6, result, 0.4, 0, result)
        
        # Add IMU data
        y_pos = 35
        cv2.putText(result, f"IMU Data:", (20, y_pos), font, font_scale, color, thickness)
        y_pos += line_spacing
        cv2.putText(result, f"Accel: {accel_x:.2f}, {accel_y:.2f}, {accel_z:.2f} m/s²", 
                   (20, y_pos), font, font_scale, color, thickness)
        y_pos += line_spacing
        cv2.putText(result, f"Gyro: {gyro_x:.2f}, {gyro_y:.2f}, {gyro_z:.2f} rad/s", 
                   (20, y_pos), font, font_scale, color, thickness)
        
        # Add GPS data if available
        y_pos += line_spacing
        if has_gps:
            cv2.putText(result, f"GPS:", (20, y_pos), font, font_scale, color, thickness)
            y_pos += line_spacing
            cv2.putText(result, f"Lat: {float(gps_lat):.6f}, Lon: {float(gps_lon):.6f}", 
                       (20, y_pos), font, font_scale, color, thickness)
        else:
            cv2.putText(result, f"GPS: Signal Lost", (20, y_pos), font, font_scale, (0, 0, 255), thickness)
        
        return result
    
    def assemble_video(self):
        """Assemble the video from collected frames."""
        if not self.metadata:
            print("No metadata found. Cannot assemble video.")
            return False
        
        # Get first image to determine dimensions
        first_frame_path = os.path.join(self.images_dir, self.metadata[0]['image_filename'])
        if not os.path.exists(first_frame_path):
            print(f"Error: First frame not found at {first_frame_path}")
            return False
        
        first_frame = cv2.imread(first_frame_path)
        if first_frame is None:
            print(f"Error: Could not read first frame at {first_frame_path}")
            return False
        
        height, width, _ = first_frame.shape
        
        # Create video writer
        fourcc = cv2.VideoWriter_fourcc(*'mp4v')  # Use 'avc1' for h264 codec if available
        video_writer = cv2.VideoWriter(self.output_file, fourcc, self.fps, (width, height))
        
        print(f"Creating video: {self.output_file}")
        print(f"Resolution: {width}x{height}, FPS: {self.fps}")
        
        for entry in tqdm(self.metadata, desc="Assembling video"):
            # Get the frame path
            frame_path = os.path.join(self.images_dir, entry['image_filename'])
            
            if not os.path.exists(frame_path):
                print(f"Warning: Frame not found: {frame_path}")
                continue
            
            # Read the frame
            frame = cv2.imread(frame_path)
            if frame is None:
                print(f"Warning: Could not read frame: {frame_path}")
                continue
            
            # Find corresponding sensor data for overlay
            if self.sensor_data:
                sensor_entry = self._find_nearest_sensor_data(entry['timestamp'])
                frame = self._overlay_sensor_data(frame, sensor_entry)
            
            # Add timestamp
            timestamp = float(entry['timestamp'])
            minutes = int(timestamp // 60)
            seconds = timestamp % 60
            timestamp_str = f"{minutes:02d}:{seconds:06.3f}"
            
            # Add timestamp to bottom right
            cv2.putText(frame, timestamp_str, (width - 150, height - 20), 
                       cv2.FONT_HERSHEY_SIMPLEX, 0.6, (255, 255, 255), 1)
            
            # Write the frame to video
            video_writer.write(frame)
        
        # Release resources
        video_writer.release()
        print(f"Video assembled successfully: {self.output_file}")
        
        return True
    
    def create_synchronized_data(self, output_csv=None):
        """
        Create a synchronized dataset with camera and sensor data.
        
        This is useful for algorithm development and testing, as it provides
        a single CSV file with all data aligned by timestamp.
        """
        if not self.metadata or not self.sensor_data:
            print("Both metadata and sensor data are required for synchronization.")
            return False
        
        if output_csv is None:
            base_name = os.path.splitext(self.output_file)[0]
            output_csv = f"{base_name}_synchronized.csv"
        
        print(f"Creating synchronized dataset: {output_csv}")
        
        # Collect headers from both datasets
        meta_headers = list(self.metadata[0].keys())
        sensor_headers = list(self.sensor_data[0].keys())
        
        # Create combined headers, avoiding duplicates
        combined_headers = meta_headers.copy()
        for header in sensor_headers:
            if header not in combined_headers and header != 'timestamp':
                combined_headers.append(header)
        
        with open(output_csv, 'w', newline='') as f:
            writer = csv.DictWriter(f, fieldnames=combined_headers)
            writer.writeheader()
            
            for entry in tqdm(self.metadata, desc="Synchronizing data"):
                # Find nearest sensor data
                sensor_entry = self._find_nearest_sensor_data(entry['timestamp'])
                
                # Create combined entry
                combined_entry = entry.copy()
                for key, value in sensor_entry.items():
                    if key not in combined_entry and key != 'timestamp':
                        combined_entry[key] = value
                
                # Write to CSV
                writer.writerow(combined_entry)
        
        print(f"Synchronized dataset created: {output_csv}")
        return True


def main():
    parser = argparse.ArgumentParser(description='Assemble video from collected frames')
    parser.add_argument('--images', type=str, required=True, help='Directory containing captured images')
    parser.add_argument('--metadata', type=str, required=True, help='Path to camera metadata CSV file')
    parser.add_argument('--output', type=str, required=True, help='Output video file path')
    parser.add_argument('--fps', type=int, default=30, help='Frames per second (default: 30)')
    parser.add_argument('--overlay', type=str, help='Path to IMU/GPS data CSV to overlay on video')
    parser.add_argument('--sync-data', action='store_true', help='Create synchronized dataset CSV')
    
    args = parser.parse_args()
    
    assembler = VideoAssembler(
        args.images,
        args.metadata,
        args.output,
        args.fps,
        args.overlay
    )
    
    success = assembler.assemble_video()
    
    if success and args.sync_data:
        assembler.create_synchronized_data()


if __name__ == "__main__":
    main()