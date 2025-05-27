import sys
import pandas as pd
import matplotlib
matplotlib.use('Agg') 
import matplotlib.pyplot as plt
import os


if len(sys.argv) < 3:
    print("Usage: python3 plot_speed.py <of_csv_path> <gps_log_csv_path>")
    sys.exit(1)

of_csv_path = sys.argv[1]
gps_csv_path = sys.argv[2]

# Read optical flow velocity
df_of = pd.read_csv(of_csv_path)

# Read PX4 GPS log
df_gps = pd.read_csv(gps_csv_path)

# Try to find the right GPS column
possible_columns = ['groundspeed_m_s', 'vel_m_s', 'velocity', 'spd']
gps_column = None
for col in possible_columns:
    if col in df_gps.columns:
        gps_column = col
        break

if gps_column is None:
    print(f"[ERROR] Could not find any of these columns in PX4 log: {possible_columns}")
    sys.exit(2)

# Trim or interpolate GPS values to match frame count of OF data
gps_values = df_gps[gps_column].values
if len(gps_values) < len(df_of):
    gps_interp = pd.Series(gps_values).reindex(range(len(df_of))).interpolate(method='linear').fillna(method='bfill').fillna(method='ffill')
else:
    gps_interp = gps_values[:len(df_of)]

# Plotting
plt.figure(figsize=(10, 4))
plt.plot(df_of['frame_number'], df_of['speed_mps'], label="Prędkość z OF [m/s]", color='blue')
plt.plot(df_of['frame_number'], gps_interp, label="Prędkość GPS [m/s]", color='red')
plt.xlabel("Klatka")
plt.ylabel("Prędkość (m/s)")
plt.title("Porównanie prędkości: Optical Flow vs GPS")
plt.grid(True)
plt.legend()

# Save
base_path = '/'.join(of_csv_path.split('/')[:-1])
plot_filename = f"{of_csv_path.split('.')[-2].split('/')[-1]}_plot.png"
plot_path = f"{base_path}/{plot_filename}"

plt.tight_layout()
plt.savefig(plot_path)
print(f"[INFO] OF + GPS velocity plot saved as: {plot_path}")
