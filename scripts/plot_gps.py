# -*- coding: utf-8 -*-
import pandas as pd
import matplotlib.pyplot as plt
import sys
import os
import numpy as np

from math import radians, sin, cos, sqrt, atan2

def haversine(lat1, lon1, lat2, lon2):
    # Ziemia w metrach
    R = 6371000.0
    dlat = radians(lat2 - lat1)
    dlon = radians(lon2 - lon1)
    a = sin(dlat / 2)**2 + cos(radians(lat1)) * cos(radians(lat2)) * sin(dlon / 2)**2
    c = 2 * atan2(sqrt(a), sqrt(1 - a))
    return R * c

if len(sys.argv) < 2:
    print("Usage: python plot_gps_heading.py <csv_filepath>")
    sys.exit(1)

csv_path = sys.argv[1]
df = pd.read_csv(csv_path)

required_cols = [
    "dr_lat", "dr_lon", "gps_lat", "gps_lon",
    "frame_number", "heading",
    "speed_mps",      # Optical Flow speed
    "gps_vel"         # GPS speed
]

missing = [col for col in required_cols if col not in df.columns]
if missing:
    raise ValueError(f"Missing columns: {', '.join(missing)}")

# Add time index in units of 100ms (1 tick = 3 frames)
df["time_100ms"] = (df["frame_number"] // 3).astype(int)

# Aggregate
agg = df.groupby("time_100ms").agg({
    "dr_lat": "last",
    "dr_lon": "last",
    "gps_lat": "last",
    "gps_lon": "last",
    "heading": "mean",
    "speed_mps": "mean",
    "gps_vel": "mean"
}).reset_index()

# --- Error metrics: position ---
position_errors = agg.apply(lambda row: haversine(row["gps_lat"], row["gps_lon"], row["dr_lat"], row["dr_lon"]), axis=1)
pos_mae = position_errors.mean()
pos_rmse = np.sqrt((position_errors ** 2).mean())
pos_max = position_errors.max()

# --- Error metrics: velocity ---
velocity_errors = (agg["gps_vel"] - agg["speed_mps"]).abs()
vel_mae = velocity_errors.mean()
vel_rmse = np.sqrt(((agg["gps_vel"] - agg["speed_mps"]) ** 2).mean())
vel_max = velocity_errors.max()

# Print metrics
print("\n=== Error Summary ===")
print(f"Position error [meters]:")
print(f"  MAE   = {pos_mae:.2f} m")
print(f"  RMSE  = {pos_rmse:.2f} m")
print(f"  Max   = {pos_max:.2f} m")

print(f"\nVelocity error [m/s]:")
print(f"  MAE   = {vel_mae:.2f} m/s")
print(f"  RMSE  = {vel_rmse:.2f} m/s")
print(f"  Max   = {vel_max:.2f} m/s\n")

# Save to file
basename = os.path.splitext(os.path.basename(csv_path))[0]
out_dir = os.path.dirname(csv_path)

report_path = os.path.join(out_dir, f"{basename}_error_report.txt")
with open(report_path, "w") as f:
    f.write("=== Error Summary ===\n")
    f.write("Position error [meters]:\n")
    f.write(f"  MAE   = {pos_mae:.2f} m\n")
    f.write(f"  RMSE  = {pos_rmse:.2f} m\n")
    f.write(f"  Max   = {pos_max:.2f} m\n\n")
    f.write("Velocity error [m/s]:\n")
    f.write(f"  MAE   = {vel_mae:.2f} m/s\n")
    f.write(f"  RMSE  = {vel_rmse:.2f} m/s\n")
    f.write(f"  Max   = {vel_max:.2f} m/s\n")

print(f"[INFO] Error report saved as: {report_path}")

# --- Plotting ---
# 1. Trajectory plot
plt.figure(figsize=(10, 6))
plt.plot(agg["dr_lon"], agg["dr_lat"], label="Estimated trajectory (GNSS-free navigation)", linestyle='--', linewidth=1)
plt.plot(agg["gps_lon"], agg["gps_lat"], label="GPS trajectory", linewidth=1, alpha=0.4)
plt.xlabel("Longitude")
plt.ylabel("Latitude")
plt.title("Trajectory Comparison: GPS vs GNSS-free navigation")
plt.legend()
plt.grid(True)
plt.axis("equal")
plt.tight_layout()
traj_path = os.path.join(out_dir, f"{basename}_position.png")
plt.savefig(traj_path, dpi=300)
print(f"[INFO] Trajectory plot saved as: {traj_path}")
plt.close()

# 2. Heading + OF speed
fig, ax1 = plt.subplots(figsize=(10, 5))
ax1.plot(agg["time_100ms"], agg["heading"], label="Heading [deg]", color="tab:orange")
ax1.set_ylabel("Heading [°]", color="tab:orange")
ax1.tick_params(axis='y', labelcolor="tab:orange")

ax2 = ax1.twinx()
ax2.plot(agg["time_100ms"], agg["speed_mps"], label="Speed OF [m/s]", color="tab:blue")
ax2.set_ylabel("Speed [m/s]", color="tab:blue")
ax2.tick_params(axis='y', labelcolor="tab:blue")

ax1.set_xlabel("Time [×100ms]")
ax1.set_title("Heading and Speed (OF) over Time")
ax1.grid(True)
plt.tight_layout()
hs_path = os.path.join(out_dir, f"{basename}_heading_speed.png")
plt.savefig(hs_path, dpi=300)
print(f"[INFO] Heading + OF speed plot saved as: {hs_path}")
plt.close()

# 3. GPS vs OF speed
plt.figure(figsize=(10, 5))
plt.plot(agg["time_100ms"], agg["gps_vel"], label="GPS velocity [m/s]", color="tab:green", alpha=0.8)
plt.plot(agg["time_100ms"], agg["speed_mps"], label="Optical Flow estimated velocity [m/s]", color="tab:red", linestyle='--')
plt.xlabel("Time [×100ms]")
plt.ylabel("Velocity [m/s]")
plt.title("Velocity Comparison: GPS vs Optical Flow")
plt.legend()
plt.grid(True)
plt.tight_layout()
vel_path = os.path.join(out_dir, f"{basename}_velocity.png")
plt.savefig(vel_path, dpi=300)
print(f"[INFO] Velocity comparison plot saved as: {vel_path}")
plt.close()
