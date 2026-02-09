import pandas as pd
import os
import sys

def convert_timestamps_in_folder(folder_path):
    folder_path = os.path.abspath(folder_path)

    if not os.path.isdir(folder_path):
        print(f"Path not exist or is not a folder:\t{folder_path}")
        return

    output_folder = folder_path + "_converted"
    os.makedirs(output_folder, exist_ok=True)

    csv_files = [f for f in os.listdir(folder_path) if f.endswith(".csv")]

    if not csv_files:
        print("No .csv files in folder:\t{folder_path}")
        return

    for file in csv_files:
        file_path = os.path.join(folder_path, file)
        try:
            df = pd.read_csv(file_path)
            if "timestamp" in df.columns:
                df["timestamp"] = pd.to_datetime(df["timestamp"], unit='us')
            else:
                print(f"File {file} has not column named 'timestamp'.")
            output_path = os.path.join(output_folder, file)
            df.to_csv(output_path, index=False)
            print(f"Saved: {output_path}")
        except Exception as e:
            print(f"Error in processing file -> {file}: {e}")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: convert_timestamps.py <path_to_log_csv_folder>")
    else:
        convert_timestamps_in_folder(sys.argv[1])
