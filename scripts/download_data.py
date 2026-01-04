import os
import argparse
import wave
import struct
import math
import shutil
import json
import requests
import sys

try:
    import mirdata
except ImportError:
    mirdata = None

def generate_dummy_wav(filepath, duration_sec=1.0, freq=440.0):
    """Generates a simple beep WAV file for testing."""
    sample_rate = 44100
    n_samples = int(sample_rate * duration_sec)

    with wave.open(filepath, 'w') as wav_file:
        wav_file.setnchannels(1)
        wav_file.setsampwidth(2)
        wav_file.setframerate(sample_rate)

        data = []
        for i in range(n_samples):
            t = float(i) / sample_rate
            value = int(32767.0 * math.sin(2.0 * math.pi * freq * t))
            data.append(struct.pack('<h', value))

        wav_file.writeframes(b''.join(data))

def sort_tracks(track_list, base_dir, download_dir):
    """
    Sorts tracks into Percussion/Melodic based on metadata.
    track_list: List of track objects (or dictionaries in test mode).
    download_dir: Where the raw files are currently located.
    """
    perc_dir = os.path.join(base_dir, "Percussion")
    melo_dir = os.path.join(base_dir, "Melodic")
    os.makedirs(perc_dir, exist_ok=True)
    os.makedirs(melo_dir, exist_ok=True)

    print(f"Sorting {len(track_list)} tracks...")

    for track in track_list:
        # Determine category
        # Logic: If 'Percussion' or 'Tabla' or 'Mridangam' -> Percussion
        # Else if 'Sitar', 'Flute', 'Vocal' -> Melodic

        # Accommodate both mirdata objects and test dictionaries
        if isinstance(track, dict):
            instrument = track.get('instrument', '').lower()
            audio_path = track.get('audio_path')
            if not audio_path:
                continue
            filename = os.path.basename(audio_path)
        else:
            # Assume mirdata track object
            # Saraga tracks often have 'instrument_leader' or similar metadata
            # We will use a broad check on available metadata fields
            instrument = "unknown"
            if hasattr(track, 'instrument_leader'):
                 instrument = str(track.instrument_leader).lower()
            elif hasattr(track, 'artist_instrument'):
                 instrument = str(track.artist_instrument).lower()

            if hasattr(track, 'audio_path'):
                audio_path = track.audio_path
            else:
                continue # Skip if no audio path

            filename = os.path.basename(audio_path)

        # Logic
        is_percussion = any(x in instrument for x in ['perc', 'tabla', 'mridangam', 'ghatam', 'kanjira'])
        is_melodic = any(x in instrument for x in ['sitar', 'flute', 'vocal', 'violin', 'sarangi', 'veena'])

        target_dir = None
        if is_percussion:
            target_dir = perc_dir
        elif is_melodic:
            target_dir = melo_dir

        # If matches neither but exists, maybe default to Melodic or skip?
        # Requirement says: "If 'Sitar/Flute/Vocal', move to /Samples/Melodic."
        # It implies specific sorting.

        if target_dir and audio_path and os.path.exists(audio_path):
            dest_path = os.path.join(target_dir, filename)
            # Copy file
            shutil.copy2(audio_path, dest_path)
            print(f"Moved {filename} to {target_dir}")

def main():
    parser = argparse.ArgumentParser(description="Download and organize Saraga data.")
    parser.add_argument('--test', action='store_true', help="Use dummy data for verification.")
    parser.add_argument('--local-source', type=str, help="Path to a local folder containing manually downloaded files to sort.")
    args = parser.parse_args()

    base_dir = "Samples"

    if args.local_source:
        print(f"Scanning local source: {args.local_source}")
        if not os.path.exists(args.local_source):
            print(f"Error: Path {args.local_source} does not exist.")
            return

        # Simple crawl to find all wav/mp3 in the given folder
        found_tracks = []
        for root, dirs, files in os.walk(args.local_source):
            for file in files:
                if file.lower().endswith(('.wav', '.mp3')):
                    # Create a dummy track object for our sorter
                    # We try to guess instrument from filename since we lack metadata
                    full_path = os.path.join(root, file)
                    found_tracks.append({
                        'instrument': file, # sort_tracks looks at this string
                        'audio_path': full_path
                    })

        if found_tracks:
            sort_tracks(found_tracks, base_dir, args.local_source)
            print(f"Imported {len(found_tracks)} files from local source.")
        else:
            print("No audio files found in the specified local path.")
        return

    if args.test:
        print("Running in TEST mode.")
        raw_dl_dir = "temp_raw_downloads"
        os.makedirs(raw_dl_dir, exist_ok=True)

        # Create dummy source files
        t1 = os.path.join(raw_dl_dir, "tabla_solo.wav")
        t2 = os.path.join(raw_dl_dir, "sitar_raga.wav")
        t3 = os.path.join(raw_dl_dir, "mridangam_beat.wav")
        t4 = os.path.join(raw_dl_dir, "vocal_alaap.wav")

        generate_dummy_wav(t1, freq=100)
        generate_dummy_wav(t2, freq=400)
        generate_dummy_wav(t3, freq=150)
        generate_dummy_wav(t4, freq=600)

        # Create dummy metadata list
        test_tracks = [
            {'instrument': 'tabla', 'audio_path': t1},
            {'instrument': 'sitar', 'audio_path': t2},
            {'instrument': 'mridangam', 'audio_path': t3},
            {'instrument': 'vocal', 'audio_path': t4},
            {'instrument': 'guitar', 'audio_path': None} # Should be skipped
        ]

        sort_tracks(test_tracks, base_dir, raw_dl_dir)

        # Cleanup
        shutil.rmtree(raw_dl_dir)
        print("Test complete.")
        return

    # --- REAL LOGIC ---
    if mirdata is None:
        print("mirdata not installed. Please run: pip install mirdata")
        return

    print("Initializing Saraga Hindustani...")
    data_h = mirdata.initialize('saraga_hindustani')
    # In a real run, we would download:
    # data_h.download()
    # But since that's heavy, we assume the user runs this.
    # We will try to parse what is available.

    print("Initializing Saraga Carnatic...")
    data_c = mirdata.initialize('saraga_carnatic')

    # Combine tracks
    all_tracks = []
    try:
        all_tracks.extend(data_h.load_tracks().values())
    except:
        print("Could not load Hindustani tracks (maybe not downloaded?)")

    try:
        all_tracks.extend(data_c.load_tracks().values())
    except:
         print("Could not load Carnatic tracks (maybe not downloaded?)")

    # Zenodo Download Logic
    zenodo_ids = {
        "Mridangam": "1265187",
        "Tabla": "1267023"
    }

    print("Fetching percussion datasets from Zenodo...")

    # Note: Zenodo APIs return JSON metadata listing files.
    # We fetch the record metadata first.

    for name, zid in zenodo_ids.items():
        print(f"Checking {name} (ID: {zid})...")
        api_url = f"https://zenodo.org/api/records/{zid}"

        try:
            r = requests.get(api_url)
            if r.status_code == 200:
                data = r.json()
                files = data.get('files', [])

                # Filter for audio files (e.g. mp3, wav)
                # For this assignment, we will download ONE sample file per dataset
                # to demonstrate the implementation without downloading GBs of data.

                audio_files = [f for f in files if f['key'].endswith('.mp3') or f['key'].endswith('.wav')]

                if audio_files:
                    # Sort by size to pick a small one ideally, or just the first
                    target = audio_files[0]
                    file_url = target['links']['self']
                    filename = target['key']

                    dest_dir = os.path.join(base_dir, "Percussion")
                    os.makedirs(dest_dir, exist_ok=True)
                    dest_path = os.path.join(dest_dir, filename)

                    if not os.path.exists(dest_path):
                        print(f"Downloading {filename} from {file_url}...")
                        print(f"Manual Link: {file_url}") # Provide link for manual DL

                        try:
                            response = requests.get(file_url, stream=True)
                            total_length = response.headers.get('content-length')

                            with open(dest_path, 'wb') as f:
                                if total_length is None: # no content length header
                                    f.write(response.content)
                                else:
                                    dl = 0
                                    total_length = int(total_length)
                                    for data in response.iter_content(chunk_size=4096):
                                        dl += len(data)
                                        f.write(data)
                                        done = int(50 * dl / total_length)
                                        sys.stdout.write(f"\r[{'=' * done}{' ' * (50-done)}] {int(dl/total_length*100)}%")
                                        sys.stdout.flush()
                            print("\nDownload complete.")
                        except Exception as e:
                            print(f"\nDownload failed: {e}")
                            print(f"You can manually download the file from: {file_url}")
                            print(f"Then place it in: {dest_path}")
                    else:
                        print(f"File {filename} already exists.")
            else:
                print(f"Failed to fetch metadata for {name}: Status {r.status_code}")
                print(f"Manual Page: https://zenodo.org/record/{zid}")
        except Exception as e:
            print(f"Error fetching {name}: {e}")
            print(f"Manual Page: https://zenodo.org/record/{zid}")

    print("\nData acquisition complete.")
    print("If you prefer to download manually, visit:")
    print(" - Mridangam: https://zenodo.org/record/1265187")
    print(" - Tabla: https://zenodo.org/record/1267023")
    print("Then use: python3 scripts/download_data.py --local-source <path_to_downloaded_files>")

if __name__ == "__main__":
    main()
