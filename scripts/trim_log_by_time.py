import pandas as pd
import os
import sys
from datetime import datetime, time

def trim_logs(folder_path, time_cutoff_str):
    folder_path = os.path.abspath(folder_path)
    output_folder = folder_path + "_trimmed"
    os.makedirs(output_folder, exist_ok=True)

    try:
        cutoff_time = datetime.strptime(time_cutoff_str, "%H:%M:%S").time()
    except ValueError:
        print("❌ Podaj poprawny czas w formacie HH:MM:SS")
        return

    for filename in os.listdir(folder_path):
        if not filename.endswith(".csv"):
            continue

        file_path = os.path.join(folder_path, filename)
        try:
            df = pd.read_csv(file_path)

            if "timestamp" not in df.columns:
                print(f"⚠️ Plik {filename} nie ma kolumny 'timestamp', pomijam.")
                continue

            # Zamień timestamp na datetime
            df["timestamp"] = pd.to_datetime(df["timestamp"])

            # Filtrowanie po czasie
            df = df[df["timestamp"].dt.time >= cutoff_time]

            output_path = os.path.join(output_folder, filename)
            df.to_csv(output_path, index=False)
            print(f"✅ Przycięto i zapisano: {output_path}")

        except Exception as e:
            print(f"❌ Błąd przy pliku {filename}: {e}")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Użycie: python trim_logs_by_time.py <ścieżka_do_folderu> <czas_HH:MM:SS>")
    else:
        folder = sys.argv[1]
        time_str = sys.argv[2]
        trim_logs(folder, time_str)
