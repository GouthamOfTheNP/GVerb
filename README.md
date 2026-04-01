# GVerb

A factory reverb VST3/AU plugin built with JUCE. Wraps JUCE's built-in `juce::Reverb` engine in a clean dark UI with five smoothed parameters and ships as VST3, AU, CLAP, and macOS Standalone.

---

## Parameters

| Parameter | Range | Default | Description |
|-----------|-------|---------|-------------|
| Room Size | 0.0 – 1.0 | 0.50 | Controls the size of the simulated room |
| Damping | 0.0 – 1.0 | 0.30 | High-frequency absorption of the reverb tail |
| Width | 0.0 – 1.0 | 1.00 | Stereo spread of the wet signal |
| Wet Level | 0.0 – 1.0 | 0.30 | Mix level of the processed (reverb) signal |
| Dry Level | 0.0 – 1.0 | 1.00 | Mix level of the unprocessed signal |

All parameters are smoothed over 50 ms to prevent zipper noise during automation.

---

## Building

### Requirements

- CMake ≥ 3.22
- C++23 compiler (Clang recommended on macOS)
- JUCE as a submodule at `libs/JUCE`
- macOS (build scripts use `codesign` and `pkgbuild`)

### Clone

```bash
git clone --recurse-submodules https://github.com/GouthamOfTheNP/GVerb.git
cd GVerb
```

### Configure & Build

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

### Outputs

After a successful build, the following is produced (if on macOS):

| Target | Output |
|--------|--------|
| `GVerb_VST3` | `build/GVerb.pkg` — installer for `/Library/Audio/Plug-Ins/VST3/` |
| `GVerb_AU` | `build/GVerbAU.pkg` — installer for `/Library/Audio/Plug-Ins/Components/` |
| `GVerb_CLAP` | `build/GverbCLAP.pkg` —  installer for `/Library/Audio/Plug-Ins/CLAP/` |
| `GVerb_Standalone` | Ad-hoc signed `.app` bundle |

Install a `.pkg` by double-clicking it, then rescan plugins in your DAW.

---

## Project Structure

```
GVerb/
├── assets/
│   ├── icon_big.png        # Plugin icon
│   ├── bottom_pic.png      # UI decorative image
│   ├── Neuropol.otf        # Title font (embedded via BinaryData)
│   └── Fragile Bombers.otf # Unused textfont (embedded via BinaryData)
├── libs/
│   └── JUCE/               # JUCE submodule
├── clap-juce-extensions    # CLAP format compilation
├── src/                    # Source files
│   ├── GVerbProcessor.hpp  # Plugin backend class
│   ├── GVerbEditor.hpp     # Plugin interface class
│   ├── GVerbProcessor.cpp  # Implementation of GVerbProcessor
│   ├── GVerbEditor.cpp     # Implementation of GVerbEditor
│   └── binary_data/        # Binary data getters folder
│       ├── Fonts.hpp       # Font data getters
│       └── Fonts.cpp       # Font data getters implementation
├── main.cpp                # Runner class
└── CMakeLists.txt
```

All assets under `assets/` are compiled into a `BinaryData` target and linked into the plugin binary — no external resource files are needed at runtime.

---

## Architecture

`main.cpp` contains both the processor and editor in a single translation unit.

**`GVerbProcessor`** (`juce::AudioProcessor`) handles all DSP. Parameter values are exposed as `std::atomic<float>*` via the APVTS, read on the audio thread inside `syncReverbParams()`, and fed through `juce::SmoothedValue` before being applied to `juce::Reverb`. Stereo and mono signal paths are handled separately in `processBlock`.

**`GVerbEditor`** (`juce::AudioProcessorEditor`) lays out three vertical linear sliders (Room Size, Damping, Width) on the left and two rotary knobs (Wet, Dry) on the right. The color scheme uses Ableton-style orange (`#FF8800`) accents on a dark background (`#1A1A1A`). The title is rendered in the embedded Neuropol typeface.

Plugin state is serialized to XML via `getStateInformation` / `setStateInformation` for DAW preset recall.

---

## Notes

- The AU and VST3 installers use ad-hoc code signing (`-s -`). For distribution outside your own machine, replace it with a Developer ID certificate.
- `VST3_AUTO_MANIFEST` is disabled; VST3 module info is not auto-generated.
- Tail length reported to the DAW is `roomSize × 5.0` seconds.
