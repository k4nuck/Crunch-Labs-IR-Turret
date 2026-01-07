PORT ?= /dev/ttyUSB0
FQBN ?= arduino:avr:nano
SKETCH := sketches/IR_Turret_Custom

MONITOR_CMD := CrunchLabs-Monitor
MONITOR_FALLBACK := picocom -b 9600 $(PORT)

.PHONY: help build upload upload-old monitor monitor-raw board-list kill-monitor port-list port-check port-status

help:
	@echo "CrunchLabs IR Turret — Makefile commands"
	@echo ""
	@echo "BUILD & UPLOAD:"
	@echo "  make build           Compile sketch (default FQBN: $(FQBN))"
	@echo "  make upload          Kill monitor, compile, upload to $(PORT)"
	@echo "  make upload-old      Upload using old bootloader (atmega328old)"
	@echo ""
	@echo "MONITORING & DEBUGGING:"
	@echo "  make monitor         Start serial monitor (CrunchLabs-Monitor or picocom)"
	@echo "  make monitor-raw     Start picocom directly at 9600 baud"
	@echo "  make kill-monitor    Terminate any process holding $(PORT)"
	@echo ""
	@echo "SERIAL PORT HELPERS:"
	@echo "  make port-list       List all /dev/ttyACM* and /dev/ttyUSB* devices"
	@echo "  make port-status     Check if $(PORT) is FREE or BUSY (no fail)"
	@echo "  make port-check      Check if $(PORT) is FREE (exits non-zero if BUSY)"
	@echo ""
	@echo "BOARD INFO:"
	@echo "  make board-list      List detected Arduino boards"
	@echo ""
	@echo "OVERRIDE DEFAULTS:"
	@echo "  make upload PORT=/dev/ttyACM0"
	@echo "  make upload FQBN=arduino:avr:nano:cpu=atmega328old"
	@echo ""

build:
	arduino-cli compile --fqbn $(FQBN) $(SKETCH)

upload: kill-monitor build
	arduino-cli upload -p $(PORT) --fqbn $(FQBN) $(SKETCH)

upload-old: kill-monitor build
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

port-list:
	@echo "Detecting serial ports (ttyACM*, ttyUSB*)...";
	@ls -1 /dev/ttyACM* /dev/ttyUSB* 2>/dev/null || echo "No serial ports found."

kill-monitor:
	@echo "Checking for processes using $(PORT)...";
	@PIDS=$$(lsof -t $(PORT) 2>/dev/null || true); \
	if [ -n "$$PIDS" ]; then \
	  echo "Terminating holders: $$PIDS"; \
	  kill -TERM $$PIDS 2>/dev/null || true; \
	  sleep 0.5; \
	else \
	  echo "No active monitor on $(PORT)."; \
	fi

port-check:
	@echo "Checking if $(PORT) exists and is free...";
	@if [ ! -e "$(PORT)" ]; then \
	  echo "Port not found: $(PORT)"; \
	  echo "Tip: run 'make port-list' or set PORT=/dev/ttyACM0"; \
	  exit 2; \
	fi
	@if fuser "$(PORT)" >/dev/null 2>&1; then \
	  echo "Port is BUSY: $(PORT)"; \
	  fuser -v "$(PORT)" || true; \
	  exit 1; \
	else \
	  echo "Port is FREE: $(PORT)"; \
	fi

port-status:
	@echo "Status for $(PORT):";
	@if [ ! -e "$(PORT)" ]; then \
	  echo "NOT FOUND"; \
	  exit 0; \
	fi
	@if fuser "$(PORT)" >/dev/null 2>&1; then \
	  echo "BUSY"; \
	  fuser -v "$(PORT)" || true; \
	else \
	  echo "FREE"; \
	fi


