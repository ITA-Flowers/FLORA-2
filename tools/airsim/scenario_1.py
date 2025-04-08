import airsim
import time
import sys

"""
INSTRUKCJA KONFIGURACJI KAMERY:

Aby skonfigurować kamerę skierowaną w dół (widok z gimbala), należy ręcznie utworzyć lub edytować
plik settings.json, który znajduje się w:
- Windows: C:/Users/<twoja_nazwa_użytkownika>/Documents/AirSim/settings.json

Zawartość pliku powinna wyglądać tak:
{
    "SettingsVersion": 1.2,
    "SimMode": "Multirotor",
    "Vehicles": {
        "Drone1": {
            "VehicleType": "SimpleFlight",
            "Cameras": {
                "bottom_center": {
                    "CaptureSettings": [
                        {
                            "ImageType": 0,
                            "Width": 1280,
                            "Height": 720,
                            "FOV_Degrees": 90
                        }
                    ],
                    "X": 0, "Y": 0, "Z": -0.2,
                    "Pitch": 90, "Roll": 0, "Yaw": 0
                }
            }
        }
    }
}

Po utworzeniu/edycji pliku należy zrestartować symulator AirSim.
"""

def fly_square_mission():
    """Wykonuje misję lotu po kwadracie"""
    print("Próba połączenia z AirSim...")
    try:
        # Połącz z symulatorem
        client = airsim.MultirotorClient()
        client.confirmConnection()
    except Exception as e:
        print(f"Błąd podczas łączenia z AirSim: {e}")
        print("Sprawdź, czy symulator jest uruchomiony.")
        return
    
    print("Połączono z AirSim!")
    
    try:
        # Przejmij kontrolę
        client.enableApiControl(True)
        
        # Sprawdź stan drona
        state = client.getMultirotorState()
        if state.landed_state == airsim.LandedState.Landed:
            print("Dron jest na ziemi.")
        else:
            print("Uwaga: Dron już jest w powietrzu.")
            
        # Uzbrojenie silników
        armed = client.armDisarm(True)
        if not armed:
            print("Nie udało się uzbroić silników. Sprawdź stan drona.")
            client.enableApiControl(False)
            return
        
        print("Silniki uzbrojone.")
        
        # Startowanie i wznoszenie na pułap 30m
        print("Startowanie i wznoszenie na pułap 30m...")
        client.takeoffAsync().join()
        time.sleep(2)  # Poczekaj na stabilizację po starcie
        
        client.moveToZAsync(-30, 5).join()  # Uwaga: w AirSim oś Z jest skierowana w dół, stąd -30
        time.sleep(2)  # Stabilizacja przez 2 sekundy
        
        # Wykonanie kwadratu o boku 25m
        print("Lot po kwadracie o boku 25m...")
        
        # Pierwszy odcinek - leć 25m do przodu
        print("Bok 1: Lecę do przodu...")
        client.moveToPositionAsync(25, 0, -30, 5).join()
        time.sleep(1)
        
        # Drugi odcinek - leć 25m w prawo
        print("Bok 2: Lecę w prawo...")
        client.moveToPositionAsync(25, 25, -30, 5).join()
        time.sleep(1)
        
        # Trzeci odcinek - leć 25m do tyłu
        print("Bok 3: Lecę do tyłu...")
        client.moveToPositionAsync(0, 25, -30, 5).join()
        time.sleep(1)
        
        # Czwarty odcinek - leć 25m w lewo, wracając do punktu startowego
        print("Bok 4: Lecę w lewo (powrót do punktu startowego)...")
        client.moveToPositionAsync(0, 0, -30, 5).join()
        time.sleep(1)
        
        # Lądowanie
        print("Misja zakończona. Lądowanie...")
        client.landAsync().join()
        
        # Czekaj na zakończenie lądowania
        landed = False
        retry_count = 0
        max_retries = 10
        while not landed and retry_count < max_retries:
            state = client.getMultirotorState()
            landed = state.landed_state == airsim.LandedState.Landed
            if not landed:
                time.sleep(1)
                retry_count += 1
        
        # Rozbrojenie silników
        client.armDisarm(False)
        print("Silniki rozbrojone.")

    except KeyboardInterrupt:
        # W przypadku przerwania przez użytkownika, spróbuj bezpiecznie wylądować
        print("\nPrzerwano przez użytkownika. Wykonuję awaryjne lądowanie...")
        try:
            client.landAsync().join()
            client.armDisarm(False)
        except:
            print("Błąd podczas awaryjnego lądowania.")
    
    except Exception as e:
        # Obsługa innych wyjątków
        print(f"Wystąpił błąd podczas misji: {e}")
        print("Próba awaryjnego lądowania...")
        try:
            client.landAsync().join()
            client.armDisarm(False)
        except:
            print("Błąd podczas awaryjnego lądowania.")
    
    finally:
        # Zwolnij kontrolę niezależnie od tego, co się stało
        try:
            client.enableApiControl(False)
            print("Kontrola API wyłączona.")
        except:
            print("Nie udało się wyłączyć kontroli API.")
        
        print("Misja zakończona.")

if __name__ == "__main__":
    # Pokaż instrukcję konfiguracji
    print(__doc__)
    print("\nRozpoczynam misję...\n")
    fly_square_mission()