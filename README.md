# TempTake Manager

## About

This is the manager module for the TempTake project. The manager module is responsible for receiving data from the worker modules, validating the data and publishing it to the MQTT broker.

## Installation and setup

### 1. Make sure you have platformio installed

**Ubuntu:**
```bash
sudo apt install platformio
```

**Arch:**
```bash
sudo pacman -S platformio-core
```

### 2. Configure the src/config.h file

Configure the `src/config.h` file with the correct values for your setup following the `src/config.h.example` file (copy, paste and rename the file if using the same ports).

### 3. Connect the MCU and flash the firmware

(replace `/dev/ttyUSB0` with the correct port)
```bash
pio run -t upload --upload-port /dev/ttyUSB0
```

### 4. Send the WiFi credentials to the MCU using Bluetooth Serial

Format: `ssid:password` or `ssid` for open networks.
