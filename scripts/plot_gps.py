# -*- coding: utf-8 -*-
import pandas as pd
import matplotlib
matplotlib.use('Agg') 
import matplotlib.pyplot as plt
import sys
import os

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

fig, axs = plt.subplots(3, 1, figsize=(12, 14), sharex=False)

# 1. Trajectory
axs[0].plot(df["dr_lon"], df["dr_lat"], label="Dead Reckoning", linestyle='--', linewidth=1)
axs[0].plot(df["gps_lon"], df["gps_lat"], label="GPS", linewidth=1, alpha=0.4)
axs[0].set_xlabel("Longitude")
axs[0].set_ylabel("Latitude")
axs[0].set_title("Trajektoria")
axs[0].legend()
axs[0].grid(True)
axs[0].axis("equal")

# 2. Heading + Optical Flow speed
axs[1].plot(df["frame_number"], df["heading"], label="Heading [deg]", color="tab:orange")
axs[1].set_ylabel("Heading [°]", color="tab:orange")
axs[1].tick_params(axis='y', labelcolor="tab:orange")

ax2 = axs[1].twinx()
ax2.plot(df["frame_number"], df["speed_mps"], label="Speed OF [m/s]", color="tab:blue")
ax2.set_ylabel("Speed [m/s]", color="tab:blue")
ax2.tick_params(axis='y', labelcolor="tab:blue")
axs[1].set_xlabel("Frame")
axs[1].set_title("Heading i Speed (OF) w czasie")
axs[1].grid(True)

# 3. GPS vs OF speed
axs[2].plot(df["frame_number"], df["gps_vel"], label="GPS speed [m/s]", color="tab:green", alpha=0.8)
axs[2].plot(df["frame_number"], df["speed_mps"], label="Optical Flow speed [m/s]", color="tab:red", linestyle='--')
axs[2].set_xlabel("Frame")
axs[2].set_ylabel("Speed [m/s]")
axs[2].set_title("Porównanie prędkości: GPS vs Optical Flow")
axs[2].legend()
axs[2].grid(True)

basename = os.path.splitext(os.path.basename(csv_path))[0]
plot_path = os.path.join(os.path.dirname(csv_path), f"{basename}_trajectory.png")
plt.tight_layout()
plt.savefig(plot_path, dpi=300)
print(f"[INFO] Figure saved as: {plot_path}")