import sys
import pandas as pd
import matplotlib.pyplot as plt
import os

csv_path = sys.argv[1]

# Read OF velocity
df = pd.read_csv(csv_path)

# Generate plot
plt.figure(figsize=(10, 4))
plt.plot(df['frame_number'], df['speed_mps'], label="Prędkość [m/s]", color='blue')
plt.xlabel("Klatka")
plt.ylabel("Prędkość (m/s)")
plt.title("Prędkość z przepływu optycznego")
plt.grid(True)
plt.legend()

base_path = '/'.join(csv_path.split('/')[:-1])
plot_path = f"{base_path}/{csv_path.split('.')[-2].split('/')[-1]}_plot.png"

print(f"base_path = {base_path}")
print(f"plot_path = {plot_path}")

plt.tight_layout()
plt.savefig(f"{plot_path}")
print(f"[INFO] OF velocity plot saved as: {plot_path}")
