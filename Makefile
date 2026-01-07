PORT ?= /dev/ttyUSB0
FQBN ?= arduino:avr:nano
SKETCH := sketches/IR_Turret_Custom

MONITOR_CMD := CrunchLabs-Monitor
MONITOR_FALLBACK := picocom -b 9600 $(PORT)

.PHONY: build upload upload-old monitor monitor-raw board-list kill-monitor

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

# Gracefully stop picocom (or any holder) on the serial port before upload
kill-monitor:
	@echo "Checking for processes using $(PORT)...";
	@PIDS=$$(lsof -t $(PORT) 2>/dev/null || true); \
	if [ -n "$$PIDS" ]; then \
	  echo "Closing monitor on $(PORT)..."; \
	  # First try to terminate only picocom bound to this port
	  (lsof -n -F p $(PORT) 2>/dev/null | sed -n 's/^p//p' | while read pid; do \
	    if [ "$$pid" != "" ] && [ "$$(ps -p $$pid -o comm=)" = "picocom" ]; then kill -TERM $$pid 2>/dev/null || true; fi; \
	  done); \
	  # If still busy, force-kill any remaining holders of the port
	  (sleep 0.3; lsof -t $(PORT) 2>/dev/null | xargs -r kill -TERM 2>/dev/null || true); \
	  sleep 0.5; \
	else \
	  echo "No active monitor on $(PORT)."; \
	fi
