# Ma Drive

**Ma Drive** is a Windows VST3 and Standalone saturation / clipper developed by **WADIDAW**. It combines a soft clip stage with three characterful drive modes in an industrial rack-style interface.

## Project Status

First runnable implementation: JUCE VST3/Standalone target with real-time soft clipping, GROWL/ODD/TAPE drive, HPF/LPF, 1x/2x/4x/8x JUCE oversampling, Stereo/M-S modes, parameter state, A/B snapshots, bypass, trim, and live input/output/GR waveform metering.

The project specification, visual direction, and core design concepts are documented in:

- WADIDAW_CODEX_MASTER_SPEC.txt
- ssets/DESAIN UTAMA.png

## Development

Requires CMake 3.22+, a C++17 Windows toolchain, and the vendored JUCE submodule-style source at `external/JUCE`.

From a Visual Studio Developer PowerShell (or a MinGW environment):

```powershell
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

For Visual Studio generators, use `cmake -S . -B build` then `cmake --build build --config Release`.

Expected artifacts are `build/MaDrive_artefacts/Release/VST3/Ma Drive.vst3` and `build/MaDrive_artefacts/Release/Standalone/Ma Drive.exe` (generator layout can vary).

The UI has functional parameter bindings, live visual history, dynamic threshold, and meter readouts. Spectrum/Transfer tabs and a full three-band split are planned refinements; the current Multi Band selection follows the stable shared processing path rather than presenting a fake per-band display.

## License

TBD
