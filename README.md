# Smoke Density Panel – Windows package

Desktop app, ESP32 firmware and Modbus datasheet for the smoke density controller.

## Download

**[Download everything (zip, 83 MB)](https://github.com/syedaburehan/smoke-density-panel-windows/releases/latest/download/Smoke_Density_Panel_Windows.zip)**

Or just the [Windows installer (.exe)](https://github.com/syedaburehan/smoke-density-panel-windows/releases/latest/download/Smoke.Density.Panel-Setup-1.0.0-win-x64.exe) · [all releases](https://github.com/syedaburehan/smoke-density-panel-windows/releases)

## Contents

| File | What it is |
| --- | --- |
| `Smoke Density Panel-Setup-1.0.0-win-x64.exe` | Desktop app installer, Windows 10/11 64-bit (in the release / zip) |
| [`firmware/`](firmware) | ESP32 firmware: one combined file (`smoke_density_modbus_full_0x0.bin`) or the four separate parts, plus [HOW_TO_FLASH.txt](firmware/HOW_TO_FLASH.txt) |
| [`Modbus_Register_Datasheet.pdf`](Modbus_Register_Datasheet.pdf) | Register map, data formats, test procedures |
| [`README.txt`](README.txt) | Install, connect and use |

## Quick start

1. Run the installer. It is not code-signed: if SmartScreen warns, click **More info → Run anyway**.
2. Connect over **USB** (Add port… → pick the board → Connect) or **Wi-Fi** (join `ESP32_rehan`, IP 192.168.4.1, port 502).
3. Choose the test type (Absorption 4 min / Transmission 40 min) and press **START TEST**.

The 40-minute Transmission test needs the firmware in `firmware/` (register 20 = test duration).
