# CrunchLabs IR Turret — VS Code + Arduino CLI

Local workflow to build, upload, and monitor the CrunchLabs IR Turret using VS Code and Arduino CLI.

## Prerequisites
- Linux with serial permissions (user in `dialout`)
- Arduino CLI (snap)
- Libraries: IRremote, Servo
- Reliable serial monitor (picocom)

### 1) Serial permissions (one-time)
```bash
# Check membership
groups | grep dialout || sudo usermod -aG dialout "$USER"
# Log out/in or run: exec bash -l
```

### 2) Install Arduino CLI (snap)
```bash
sudo snap install arduino-cli
arduino-cli config init
arduino-cli core update-index
arduino-cli core install arduino:avr
arduino-cli lib install "IRremote" "Servo"
# Allow USB access for snap
sudo snap connect arduino-cli:raw-usb
```

### 3) Install picocom (recommended monitor)
```bash
sudo apt update && sudo apt install -y picocom
```

## Repository Layout
```
Crunch-Labs-IR-Turret/
  README.md
  sketches/
    IR_Turret_Custom/
      IR_Turret_Custom.ino
```
- Arduino expects the folder and `.ino` file names to match.

## Build and Upload
From the repo root:

```bash
# Compile (Arduino Nano, new bootloader)
arduino-cli compile --fqbn arduino:avr:nano sketches/IR_Turret_Custom

# Upload (adjust port if not /dev/ttyUSB0)
arduino-cli upload -p /dev/ttyUSB0 --fqbn arduino:avr:nano sketches/IR_Turret_Custom
```

If you see sync errors, try the old bootloader variant:
```bash
arduino-cli upload -p /dev/ttyUSB0 --fqbn arduino:avr:nano:cpu=atmega328old sketches/IR_Turret_Custom
```

### Makefile shortcuts
```bash
# Default port/fqbn
make build
make upload

# Old bootloader
make upload-old

# Monitor (uses CrunchLabs-Monitor if available, else picocom)
make monitor
make monitor-raw

# Override port/fqbn on the fly
make upload PORT=/dev/ttyUSB1 FQBN=arduino:avr:nano:cpu=atmega328old
```

## Serial Monitoring
Preferred: use the CrunchLabs wrapper around `picocom`.

- Wrapper script location: `~/projects/CrunchLabs/scripts/CrunchLabs-Monitor`
- Ensure it’s on your PATH (added in `~/.profile`):
  ```bash
  # CrunchLabs scripts
  if [ -d "$HOME/projects/CrunchLabs/scripts" ] ; then
      PATH="$HOME/projects/CrunchLabs/scripts:$PATH"
  fi
  ```

Usage:
```bash
CrunchLabs-Monitor
# Exit with: Ctrl+A, then Ctrl+X
```

Alternative (without wrapper):
```bash
picocom -b 9600 /dev/ttyUSB0
```

## VS Code Notes
- Workspace-level IntelliSense is configured under `~/projects/CrunchLabs/.vscode/` so `.ino` files resolve `Arduino.h`, `Servo.h`, and `IRremote.hpp`.
- Open the top-level `CrunchLabs.code-workspace` for consistent settings across CrunchLabs projects.
- Arduino extension: workspace settings point to snap `arduino-cli`; this repo’s `.vscode/arduino.json` carries the sketch/board/port specifics.

## Troubleshooting
- Port busy (/dev/ttyUSB0):
  - Close Arduino Cloud Agent/browser IDE or run `fuser -v /dev/ttyUSB0` to identify the holder.
- Permissions denied:
  - Ensure `dialout` membership and re-login (`exec bash -l`).
- Board not in sync:
  - Switch FQBN to `arduino:avr:nano:cpu=atmega328old`.
- Arduino CLI monitor errors (snap sandboxing):
  - Prefer `picocom` or the `CrunchLabs-Monitor` wrapper.

## Definition of Done
- `arduino-cli compile` and `arduino-cli upload` succeed
- Serial monitor shows expected startup logs (9600 baud)
- Repo structure remains minimal and reproducible
