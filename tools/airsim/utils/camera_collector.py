#!/usr/bin/env python3
"""
Camera Data Collector for AirSim - Highly Optimized Video Stream Version

This script connects to AirSim and collects camera images during flight.
Images are saved directly as a video file with maximum performance optimizations.
"""

import os
import time
import csv
import argparse
import numpy as np
import airsim
import cv2 # type: ignore
import threading
import queue
from datetime import datetime


class CameraCollector:
    def __init__(self, output_dir, camera_name="0", fps=30, width=640, height=480, fov=90, duration=None, 
                 metadata_interval=3, format="avi"):
        """
        Initialize the camera data collector.
        
        Args:
            output_dir (str): Directory to save collected data
            camera_name (str): Camera name to use in AirSim
            fps (int): Frame rate in frames per second
            width (int): Image width in pixels
            height (int): Image height in pixels
            fov (float): Field of view in degrees
            duration (float, optional): Duration of data collection in seconds
            metadata_interval (int): How often to record metadata (every N frames)
            format (str): Video format ('avi', 'mp4', etc.)
        """
        self.output_dir = output_dir
        self.camera_name = camera_name
        self.fps = fps
        self.width = width
        self.height = height
        self.fov = fov
        self.duration = duration
        self.metadata_interval = metadata_interval
        self.video_format = format
        
        # Statistics
        self.frame_count = 0
        self.last_fps_check = time.time()
        self.fps_frame_count = 0
        self.actual_fps = 0
        
        # Performance monitoring
        self.enable_perf_monitoring = True
        self.perf_stats = {
            'fetch': [], 'process': [], 'write': [], 'total': []
        }
        
        # Initialize queue for video processing thread
        self.frame_queue = queue.Queue(maxsize=30)  # Buffer for 30 frames
        self.stop_event = threading.Event()
        self.video_thread = None
        
        # Connect to AirSim
        self.client = airsim.MultirotorClient()
        self.client.confirmConnection()
        print(f"[Collector][CAM/REC] Connected to AirSim! Client Ver:{self.client.getClientVersion()}")
        
        # Create output directory if it doesn't exist
        os.makedirs(output_dir, exist_ok=True)
        
        # Define file paths
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        if self.video_format.lower() == 'avi':
            self.video_file = os.path.join(output_dir, f"flight_video_{timestamp}.avi")
            self.fourcc = cv2.VideoWriter_fourcc(*'XVID')
        else:
            self.video_file = os.path.join(output_dir, f"flight_video_{timestamp}.mp4")
            self.fourcc = cv2.VideoWriter_fourcc(*'avc1')  # H.264 encoding
            
        self.metadata_file = os.path.join(output_dir, f"camera_metadata_{timestamp}.csv")
        
        # Initialize VideoWriter (will be done in the video thread)
        self.video_writer = None
        
        # Initialize CSV writer for metadata with buffered I/O
        self.csv_file = open(self.metadata_file, 'w', newline='', buffering=16384)
        self.csv_writer = csv.writer(self.csv_file)
        
        # Write header
        self.csv_writer.writerow([
            'timestamp', 
            'frame_number',
            'video_timestamp',
            'position_x', 'position_y', 'position_z',
            'orientation_w', 'orientation_x', 'orientation_y', 'orientation_z'
        ])
        
        self.start_time = None
        
        # Cache for drone state to minimize API calls
        self.last_drone_state = None
        self.last_state_time = 0
        self.state_cache_timeout = 0.1  # 100ms cache
        
        # Save camera parameters to a file
        self.camera_params_file = os.path.join(output_dir, f"camera_parameters_{timestamp}.json")
        self.save_camera_parameters()
    
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
        
        print(f"[Collector][CAM/REC] Camera parameters saved to {self.camera_params_file}")
    
    def get_drone_state(self):
        """Get drone state with caching to reduce API calls."""
        current_time = time.time()
        
        # If cache is valid, return cached state
        if self.last_drone_state is not None and (current_time - self.last_state_time) < self.state_cache_timeout:
            return self.last_drone_state
        
        # Otherwise, get fresh state and update cache
        try:
            self.last_drone_state = self.client.getMultirotorState()
            self.last_state_time = current_time
            return self.last_drone_state
        except Exception as e:
            print(f"[Collector][CAM/REC] Error getting drone state: {e}")
            # Return last known state if available, or None
            return self.last_drone_state
    
    def video_processing_thread(self):
        """Thread for processing and writing video frames."""
        try:
            # Initialize VideoWriter
            self.video_writer = cv2.VideoWriter(
                self.video_file, 
                self.fourcc, 
                self.fps, 
                (self.width, self.height)
            )
            
            if not self.video_writer.isOpened():
                print(f"[VideoThread] ERROR: Could not open video file for writing: {self.video_file}")
                return
                
            print(f"[VideoThread] Video writer initialized for {self.video_file}")
            
            frames_processed = 0
            last_status_time = time.time()
            
            while not self.stop_event.is_set() or not self.frame_queue.empty():
                try:
                    # Get frame from queue with timeout
                    frame_data = self.frame_queue.get(timeout=0.5)
                    img_bgr, timestamp, frame_number = frame_data
                    
                    # Start timing write operation
                    t_start = time.time()
                    
                    # Write frame to video
                    self.video_writer.write(img_bgr)
                    
                    # End timing write operation
                    t_end = time.time()
                    write_time = (t_end - t_start) * 1000  # ms
                    
                    if self.enable_perf_monitoring:
                        if len(self.perf_stats['write']) < 1000:  # Limit stats collection
                            self.perf_stats['write'].append(write_time)
                    
                    # Mark as done in queue
                    self.frame_queue.task_done()
                    frames_processed += 1
                    
                    # Log status periodically
                    current_time = time.time()
                    if current_time - last_status_time >= 5.0:
                        print(f"[VideoThread] Processed {frames_processed} frames, queue size: {self.frame_queue.qsize()}")
                        last_status_time = current_time
                    
                except queue.Empty:
                    # Check if we should exit
                    if self.stop_event.is_set() and self.frame_queue.empty():
                        break
                    continue
                    
                except Exception as e:
                    print(f"[VideoThread] Error processing frame: {e}")
                    import traceback
                    traceback.print_exc()
            
            print(f"[VideoThread] Video processing complete. Processed {frames_processed} frames.")
            
        except Exception as e:
            print(f"[VideoThread] Unhandled error in video thread: {e}")
            import traceback
            traceback.print_exc()
            
        finally:
            # Release video writer
            if self.video_writer:
                self.video_writer.release()
                print(f"[VideoThread] Video file closed: {self.video_file}")
    
    def start_collection(self):
        """Start collecting images from the camera."""
        print(f"[Collector][CAM/REC] Starting camera collection at {self.fps} FPS...")
        print(f"[Collector][CAM/REC] Video will be saved to {self.video_file}")
        print(f"[Collector][CAM/REC] Using resolution {self.width}x{self.height}")
        print(f"[Collector][CAM/REC] Recording metadata every {self.metadata_interval} frames")
        
        # Get first image to verify dimensions
        first_response = self.client.simGetImages([
            airsim.ImageRequest(self.camera_name, airsim.ImageType.Scene, False, False)
        ])
        
        if first_response:
            print(f"[Collector][CAM/REC] First image received: {len(first_response[0].image_data_uint8)} bytes")
            print(f"[Collector][CAM/REC] Image dimensions: {first_response[0].width}x{first_response[0].height}")
            
            # Update dimensions if they differ from requested
            if first_response[0].width != self.width or first_response[0].height != self.height:
                print(f"[Collector][CAM/REC] [WARNING]: Actual image dimensions ({first_response[0].width}x{first_response[0].height}) " + 
                      f"differ from requested ({self.width}x{self.height})")
                print("[Collector][CAM/REC] Using actual dimensions for processing")
                self.width = first_response[0].width
                self.height = first_response[0].height
        else:
            print("[Collector][CAM/REC] ERROR: Could not get first image from camera")
            return
        
        # Start video processing thread
        self.stop_event.clear()
        self.video_thread = threading.Thread(target=self.video_processing_thread)
        self.video_thread.daemon = True
        self.video_thread.start()
        
        self.start_time = time.time()
        self.last_fps_check = self.start_time
        frame_interval = 1.0 / self.fps
        next_frame_time = self.start_time
        
        try:
            while True:
                current_time = time.time()
                
                # Check if we've reached the duration limit
                if self.duration and (current_time - self.start_time) >= self.duration:
                    print(f"[Collector][CAM/REC] Reached duration limit of {self.duration} seconds")
                    break
                
                # Simple timer for next frame
                sleep_time = next_frame_time - current_time
                if sleep_time > 0:
                    time.sleep(sleep_time)
                
                # Capture image
                t1 = time.time()
                responses = self.client.simGetImages([
                    airsim.ImageRequest(self.camera_name, airsim.ImageType.Scene, False, False)
                ])
                t2 = time.time()
                
                if responses:
                    try:
                        # Convert image data to numpy array
                        img1d = np.frombuffer(responses[0].image_data_uint8, dtype=np.uint8)
                        
                        # Reshape to expected dimensions
                        img_rgb = img1d.reshape(self.height, self.width, 3)
                        
                        # Convert from RGB to BGR (for OpenCV)
                        img_bgr = cv2.cvtColor(img_rgb, cv2.COLOR_RGB2BGR)
                        t3 = time.time()
                        
                        # Add to video processing queue
                        self.frame_queue.put((img_bgr, t2, self.frame_count))
                        
                        # Collect metadata less frequently to reduce overhead
                        if self.frame_count % self.metadata_interval == 0:
                            # Get drone position and orientation (with caching)
                            state = self.get_drone_state()
                            
                            if state:
                                position = state.kinematics_estimated.position
                                orientation = state.kinematics_estimated.orientation
                                
                                # Record elapsed time and frame number
                                elapsed = t2 - self.start_time
                                
                                # Record metadata
                                self.csv_writer.writerow([
                                    elapsed,              # timestamp (seconds since start)
                                    self.frame_count,         # frame number
                                    format(elapsed, '.6f'),  # video timestamp (for sync with video)
                                    position.x_val, position.y_val, position.z_val,
                                    orientation.w_val, orientation.x_val, orientation.y_val, orientation.z_val
                                ])
                        
                        # Update counters
                        self.frame_count += 1
                        self.fps_frame_count += 1
                        
                        # Performance monitoring
                        t4 = time.time()
                        fetch_time = (t2 - t1) * 1000  # ms
                        process_time = (t3 - t2) * 1000  # ms
                        total_time = (t4 - t1) * 1000  # ms
                        
                        if self.enable_perf_monitoring:
                            if len(self.perf_stats['fetch']) < 1000:  # Limit stats collection
                                self.perf_stats['fetch'].append(fetch_time)
                                self.perf_stats['process'].append(process_time)
                                self.perf_stats['total'].append(total_time)
                        
                        # Display performance stats every 30 frames
                        if self.frame_count % 30 == 0 and self.enable_perf_monitoring:
                            print(f"[Performance] Fetch: {fetch_time:.1f}ms, Process: {process_time:.1f}ms, Total: {total_time:.1f}ms")
                        
                        # Calculate and display actual FPS every second
                        if current_time - self.last_fps_check >= 1.0:
                            time_diff = current_time - self.last_fps_check
                            self.actual_fps = self.fps_frame_count / time_diff
                            elapsed = current_time - self.start_time
                            print(f"[Collector][CAM/REC] FPS: {self.actual_fps:.2f}, Frames: {self.frame_count}, Time: {elapsed:.2f}s, Queue: {self.frame_queue.qsize()}")
                            self.last_fps_check = current_time
                            self.fps_frame_count = 0
                        
                    except Exception as e:
                        print(f"[Collector][CAM/REC] Error processing image: {e}")
                        import traceback
                        traceback.print_exc()
                
                # Schedule next frame
                next_frame_time += frame_interval
                
                # If we're far behind schedule, reset the timing
                if current_time - next_frame_time > 5 * frame_interval:
                    print(f"[Collector][CAM/REC] Resetting schedule, behind by {current_time - next_frame_time:.3f}s")
                    next_frame_time = current_time + frame_interval
                
        except KeyboardInterrupt:
            print("\n[Collector][CAM/REC] Collection interrupted by user")
        
        finally:
            # Signal video thread to stop
            self.stop_event.set()
            
            # Wait for video thread to finish
            print("[Collector][CAM/REC] Waiting for video processing to complete...")
            self.video_thread.join(timeout=30.0)
            
            # Close CSV file
            self.csv_file.close()
            
            # Final statistics
            elapsed_time = time.time() - self.start_time
            
            print(f"[Collector][CAM/REC] Video recording finished")
            print(f"[Collector][CAM/REC] Recorded {self.frame_count} frames over {elapsed_time:.2f} seconds")
            print(f"[Collector][CAM/REC] Average frame rate: {self.frame_count / elapsed_time:.2f} FPS")
            print(f"[Collector][CAM/REC] Target frame rate: {self.fps} FPS")
            print(f"[Collector][CAM/REC] Video saved to: {self.video_file}")
            print(f"[Collector][CAM/REC] Metadata saved to: {self.metadata_file}")
            
            # Show performance statistics summary
            if self.enable_perf_monitoring and self.perf_stats['fetch']:
                print("\nPerformance Statistics (ms):")
                for metric in ['fetch', 'process', 'write', 'total']:
                    if self.perf_stats[metric]:
                        avg = sum(self.perf_stats[metric]) / len(self.perf_stats[metric])
                        max_val = max(self.perf_stats[metric])
                        min_val = min(self.perf_stats[metric]) if self.perf_stats[metric] else 0
                        print(f"  {metric.capitalize()}: Avg={avg:.1f}, Min={min_val:.1f}, Max={max_val:.1f}")


def main():
    parser = argparse.ArgumentParser(description='Collect camera video from AirSim')
    parser.add_argument('--output', type=str, required=True, help='Output directory for video and metadata')
    parser.add_argument('--camera', type=str, default="0", help='Camera name in AirSim (default: 0)')
    parser.add_argument('--fps', type=int, default=30, help='Frame rate in FPS (default: 30)')
    parser.add_argument('--width', type=int, default=640, help='Image width in pixels (default: 640)')
    parser.add_argument('--height', type=int, default=480, help='Image height in pixels (default: 480)')
    parser.add_argument('--fov', type=float, default=90, help='Field of view in degrees (default: 90)')
    parser.add_argument('--duration', type=float, help='Duration of collection in seconds (default: until interrupted)')
    parser.add_argument('--metadata-interval', type=int, default=3, help='Record metadata every N frames (default: 3)')
    parser.add_argument('--format', type=str, default='avi', choices=['avi', 'mp4'], help='Video format (default: avi)')
    
    args = parser.parse_args()
    
    collector = CameraCollector(
        args.output, 
        args.camera, 
        args.fps, 
        args.width, 
        args.height,
        args.fov,
        args.duration,
        args.metadata_interval,
        args.format
    )
    
    collector.start_collection()


if __name__ == "__main__":
    main()