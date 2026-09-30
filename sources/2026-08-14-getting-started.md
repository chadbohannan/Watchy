# Getting Started — Watchy (official docs)

Source: https://watchy.sqfmi.com/docs/getting-started (fetched 2026-08-14, Docusaurus site, copyright Squarofumi).

## Watchy Kit w/ Case Assembly

Before you begin, review the instructions and inspect for missing or damaged components.

- Remove the components carefully (tightly packed). **The display is made of glass and will crack if you bend it.**
- Insert & remove the battery carefully to avoid damaging PCB components.
- Test the PCB to make sure the board is functional.
- **Do not apply the included tape** unless using Watchy without the enclosure, and the screen is perfectly aligned.

1. Connect the screen to the FPC connector: ribbon cable goes through the PCB slot; shiny gold pins must face up; pull the black tabs out to insert, push tabs back to secure.
2. Connect the battery.
3. Connect USB to start charging.
4. Press SW1 to bring up menu/confirm.
5. Press SW3 & SW4 to navigate up/down.
6. Press SW2 to exit/cancel.
7. Wait 1 minute to check the clock is updating correctly.

Tips: screen must be aligned (not above/below the strap holes); tuck the battery cable in; do not force the case closed (can break screen & buttons); do not overtighten screws in the aluminum case (cracks screen).

## Arduino Setup

Watchy comes pre-loaded with firmware. To try other faces/examples:

1. Install the latest Arduino IDE.
2. File > Preferences > Additional Board Manager URLs: `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
3. Tools > Board > Boards Manager: install latest **esp32** by Espressif (not "Arduino ESP32 Boards").
4. Sketch > Include Library > Manage Libraries: search **Watchy**, install latest; ensure dependencies (GxEPD2, WiFiManager, rtc_pcf8563, etc.) are installed.

### Enter Bootloader Mode

1. Plug in USB.
2. Press and hold the top 2 buttons (Back & Up) for > 4 seconds, then release **Back first**, before releasing Up.
3. An ESP32S3 device should enumerate a serial port (COM, /dev/cu.*).

### Uploading New Watchfaces/Firmware

1. Select the serial port for Watchy.
2. If you can't find the port, repeat bootloader steps; use a data (not charge-only) USB cable; try different ports.
3. Tools > Board > ESP32 Arduino > **ESP32S3 Dev Module**.
4. Tools > Flash Size > **8MB (64Mb)**.
5. Tools > Partition Scheme > **8M with spiffs...**
6. Leave everything else default; choose an example and click upload.
7. After upload, reset Watchy to run the new firmware.

### How to reset Watchy

1. Press and hold the top 2 buttons (Back & Up) for > 4 seconds, release the **Up button first**, before releasing Back.
2. Wait a few seconds for it to boot and refresh.

### WiFi Setup

1. Click 'Setup Wifi' from the main menu.
2. Connect to 'Watchy Ap' from another device.
3. Open 192.168.4.1 in a browser (iPhone opens automatically).
4. Click 'Configure WiFi', enter SSID and password.
5. Hit 'Save'; if connection fails the watch displays 'Setup failed & timed out!'; otherwise displays the local IP and SSID.

Troubleshoot WiFi setup:
1. Click 'Info' from 192.168.4.1.
2. Check router settings allow the listed MAC address.
3. If still failing, click 'Erase Wifi Config' from the 'Info' page, wait for restart, then retry.
