#!/usr/bin/env python3
"""
IMU and GPS Data Collector for AirSim

This script connects to AirSim and collects IMU and GPS data during flight.
Data is stored in a CSV file with synchronized timestamps.
"""

import os
import time
import csv
import argparse
import numpy as np
import airsim
from datetime import datetime


class IMUGPSCollector:
    def __init__(self, output_dir, frequency=100, duration=None):
        """
        Initialize the IMU and GPS data collector.
        
        Args:
            output_dir (str): Directory to save collected data
            frequency (int): Sampling frequency in Hz
            duration (float, optional): Duration of data collection in seconds
        """
        self.output_dir = output_dir
        self.frequency = frequency
        self.duration = duration
        self.interval = 1.0 / frequency
        
        # Connect to AirSim
        self.client = airsim.MultirotorClient()
        self.client.confirmConnection()
        
        # Create output directory if it doesn't exist
        os.makedirs(output_dir, exist_ok=True)
        
        # Define file paths
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        self.imu_gps_file = os.path.join(output_dir, f"imu_gps_data_{timestamp}.csv")
        
        # Initialize CSV writer
        self.csv_file = open(self.imu_gps_file, 'w', newline='')
        self.csv_writer = csv.writer(self.csv_file)
        
        # Write header
        self.csv_writer.writerow([
            'timestamp', 
            'imu_angular_velocity_x', 'imu_angular_velocity_y', 'imu_angular_velocity_z',
            'imu_linear_acceleration_x', 'imu_linear_acceleration_y', 'imu_linear_acceleration_z',
            'imu_orientation_w', 'imu_orientation_x', 'imu_orientation_y', 'imu_orientation_z',
            'gps_latitude', 'gps_longitude', 'gps_altitude',
            'gps_velocity_x', 'gps_velocity_y', 'gps_velocity_z'
        ])
        
        self.start_time = None
        self.samples_collected = 0
    
    def start_collection(self):
        """Start collecting data from IMU and GPS."""
        print(f"Starting data collection at {self.frequency} Hz...")
        print(f"Data will be saved to {self.imu_gps_file}")
        
        self.start_time = time.time()
        next_sample_time = self.start_time
        
        try:
            while True:
                current_time = time.time()
                
                # Check if we've reached the duration limit
                if self.duration and (current_time - self.start_time) >= self.duration:
                    print(f"Reached duration limit of {self.duration} seconds")
                    break
                
                # Wait until next sample time
                if current_time < next_sample_time:
                    time.sleep(0.001)  # Small sleep to avoid CPU spinning
                    continue
                
                # Get IMU data
                imu_data = self.client.getImuData()
                
                # Get GPS data
                gps_data = self.client.getGpsData()
                
                # Get orientation from state
                state = self.client.getMultirotorState()
                orientation = state.kinematics_estimated.orientation
                
                # Record data
                self.csv_writer.writerow([
                    current_time - self.start_time,  # timestamp (seconds since start)
                    imu_data.angular_velocity.x_val, imu_data.angular_velocity.y_val, imu_data.angular_velocity.z_val,
                    imu_data.linear_acceleration.x_val, imu_data.linear_acceleration.y_val, imu_data.linear_acceleration.z_val,
                    orientation.w_val, orientation.x_val, orientation.y_val, orientation.z_val,
                    gps_data.gnss.geo_point.latitude, gps_data.gnss.geo_point.longitude, gps_data.gnss.geo_point.altitude,
                    gps_data.gnss.velocity.x_val, gps_data.gnss.velocity.y_val, gps_data.gnss.velocity.z_val
                ])
                
                self.samples_collected += 1
                if self.samples_collected % self.frequency == 0:
                    elapsed = current_time - self.start_time
                    print(f"Collected {self.samples_collected} samples ({elapsed:.2f} seconds)")
                
                # Schedule next sample
                next_sample_time += self.interval
                
        except KeyboardInterrupt:
            print("\nData collection interrupted by user")
        finally:
            elapsed_time = time.time() - self.start_time
            self.csv_file.close()
            print(f"Data collection finished. Collected {self.samples_collected} samples over {elapsed_time:.2f} seconds")
            print(f"Average sampling rate: {self.samples_collected / elapsed_time:.2f} Hz")
            print(f"Data saved to {self.imu_gps_file}")
    
    def simulate_gps_outage(self, duration=5.0):
        """
        Simulate a GPS outage by inserting NaN values for GPS data.
        
        Args:
            duration (float): Duration of the outage in seconds
        """
        print(f"Simulating GPS outage for {duration} seconds...")
        
        outage_end_time = time.time() + duration
        while time.time() < outage_end_time:
            current_time = time.time()
            
            # Get IMU data
            imu_data = self.client.getImuData()
            
            # Get orientation from state
            state = self.client.getMultirotorState()
            orientation = state.kinematics_estimated.orientation
            
            # Record data with NaN for GPS
            self.csv_writer.writerow([
                current_time - self.start_time,  # timestamp (seconds since start)
                imu_data.angular_velocity.x_val, imu_data.angular_velocity.y_val, imu_data.angular_velocity.z_val,
                imu_data.linear_acceleration.x_val, imu_data.linear_acceleration.y_val, imu_data.linear_acceleration.z_val,
                orientation.w_val, orientation.x_val, orientation.y_val, orientation.z_val,
                np.nan, np.nan, np.nan,  # GPS latitude, longitude, altitude as NaN
                np.nan, np.nan, np.nan   # GPS velocity as NaN
            ])
            
            self.samples_collected += 1
            time.sleep(self.interval)
        
        print("GPS outage simulation finished")


def main():
    parser = argparse.ArgumentParser(description='Collect IMU and GPS data from AirSim')
    parser.add_argument('--output', type=str, required=True, help='Output directory for collected data')
    parser.add_argument('--frequency', type=int, default=100, help='Sampling frequency in Hz (default: 100)')
    parser.add_argument('--duration', type=float, help='Duration of data collection in seconds (default: until interrupted)')
    parser.add_argument('--gps-outage', type=float, help='Simulate GPS outage after this many seconds')
    parser.add_argument('--outage-duration', type=float, default=5.0, help='Duration of GPS outage in seconds (default: 5.0)')
    
    args = parser.parse_args()
    
    collector = IMUGPSCollector(args.output, args.frequency, args.duration)
    
    if args.gps_outage:
        # Start a timer to simulate GPS outage
        def trigger_outage():
            time.sleep(args.gps_outage)
            collector.simulate_gps_outage(args.outage_duration)
        
        import threading
        threading.Thread(target=trigger_outage, daemon=True).start()
    
    collector.start_collection()


if __name__ == "__main__":
    main()