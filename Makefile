PORT ?= /dev/ttyUSB0
FQBN ?= arduino:avr:nano
SKETCH := sketches/IR_Turret_Custom

MONITOR_CMD := CrunchLabs-Monitor
MONITOR_FALLBACK := picocom -b 9600 $(PORT)

.PHONY: build upload upload-old monitor monitor-raw board-list

build:
	arduino-cli compile --fqbn $(FQBN) $(SKETCH)

upload: build
	arduino-cli upload -p $(PORT) --fqbn $(FQBN) $(SKETCH)

upload-old: build
	arduino-cli upload -p $(PORT) --fqbn arduino:avr:nano:cpu=atmega328old $(SKETCH)

monitor:
	@if command -v $(MONITOR_CMD) >/dev/null 2>&1; then \
	  $(MONITOR_CMD); \
	else \
	  $(MONITOR_FALLBACK); \
	fi

monitor-raw:
	picocom -b 9600 $(PORT)

board-list:
	arduino-cli board list
