SMOKE DENSITY PANEL - WINDOWS PACKAGE (v1.0.0, 2026-11-26)
==========================================================

Contents
--------
  Smoke Density Panel-Setup-1.0.0-win-x64.exe   Desktop app installer (Windows 10/11, 64-bit)
  firmware\                                     ESP32 controller firmware + HOW_TO_FLASH.txt
  Modbus_Register_Datasheet.pdf                 Register map, data formats, test procedures

1. Install the app
------------------
  Run the Setup .exe. The app is not code-signed, so Windows SmartScreen may warn:
  click "More info" -> "Run anyway". A desktop shortcut is created.
  If the controller's COM port does not appear, install the Silicon Labs CP210x driver:
  https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers

2. Connect
----------
  USB:   Connection -> "USB (COM port)" -> Add port... -> pick the board -> Connect
         (115200 baud, slave ID 1)
  Wi-Fi: join the controller's Wi-Fi network ESP32_rehan, then
         Connection -> "Wi-Fi" -> IP 192.168.4.1, port 502, slave ID 1 -> Connect

3. Use
------
  Test type:    Absorption (4 min, SDR) or Transmission (40 min, optical density)
  Test button:  START TEST -> STOP TEST -> RESET
  Calibration:  lamp on, clear path -> Set 0 %; beam blocked -> Set 100 %
  Relays:       lamp, exhaust fan, exit

4. Firmware
-----------
  The app's 40-minute Transmission test needs this firmware (register 20 = test duration).
  See firmware\HOW_TO_FLASH.txt.
