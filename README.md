# DesiSamplerPro

DesiSamplerPro is a VST3/Standalone Sampler designed for Indian Music production, featuring a "Rajasthani Palace" aesthetic and specialized mapping for Sitar and Percussion.

## Prerequisites

1.  **Python 3.9+** (for data acquisition)
2.  **CMake 3.22+**
3.  **Visual Studio 2022** (Windows) or **GCC/X11** (Linux)
4.  **JUCE 8** (Fetched automatically via CMake)

### Python Dependencies
Install the required python libraries to fetch the sample data:

```bash
pip install mirdata requests pandas
```

## Data Acquisition

Before building, you must download the sample data using the provided script.
The script sorts samples into `Samples/Percussion` and `Samples/Melodic`.

```bash
# To test with dummy data (fast):
python3 scripts/download_data.py --test

# To download real Saraga/Zenodo data:
python3 scripts/download_data.py
```

## Building on Windows (Visual Studio 2022)

1.  Open Visual Studio 2022.
2.  Select **File > Open > Folder...** and choose the `DesiSamplerPro` folder.
3.  Visual Studio will detect `CMakeLists.txt` and begin configuration.
    *   Wait for "CMake generation finished".
4.  Select the startup item **DesiSamplerPro_Standalone.exe** from the top toolbar.
5.  Press **F5** to Build and Run.

## Building on Linux

```bash
cmake -B build
cmake --build build --target DesiSamplerPro_Standalone
./build/DesiSamplerPro_artefacts/Debug/Standalone/DesiSamplerPro
```

## Usage

1.  **Launch the Standalone App.**
2.  **Configure Audio:** Use the `Options > Audio/MIDI Settings` menu (in Standalone) to select your audio interface and MIDI input.
3.  **Browser:** Double-click a file in the list to **Preview** it.
4.  **Load:** Select a file, choose a Target Octave and Mode, and click "Load Sample".
    *   *Melodic Mode:* Pitches the sample across the octave (good for Sitar).
    *   *Percussive Mode:* Maps the sample to the Root Key of the octave (good for drum kits).
5.  **Play:** Use your MIDI keyboard (or Virtual Keyboard in Settings) to play.
6.  **Indicators:** The Red/Gold LED in the top right flashes on MIDI activity.
