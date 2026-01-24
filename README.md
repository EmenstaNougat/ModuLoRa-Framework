<h1 align="center">ModuLoRa-Framework - by @emensta</h1>
<div align="center">
  <img width="2560" height="1440" alt="image" src="https://github.com/user-attachments/assets/0a949301-2898-4d91-903a-2b24ef5db38d" />
</div>

An open-source, modular ESP32 framework for LoRa wireless communication using the REYAX RYLR998 module (feel free to use any LoRa module you'd like). This project provides a complete transmitter and receiver solution, perfect for IoT prototyping, remote monitoring, and custom wireless applications.

The transmitter uses an HC-SR04 ultrasonic sensor to measure distance and sends the data via LoRa. The receiver captures this data, displaying it in real-time on an OLED screen along with signal strength information (RSSI and SNR).

<img width="30%" alt="image" src="https://github.com/user-attachments/assets/74b9b62f-0fca-4538-8141-e1b112813662" />

## Features

*   **Modular Design:** Separate, easy-to-understand Arduino sketches for the transmitter and receiver units.
*   **LoRa Communication:** Utilizes the REYAX RYLR998 module, configured via AT commands for reliable long-range communication.
*   **Sensor Integration:** The transmitter is pre-configured to use an HC-SR04 ultrasonic distance sensor.
*   **OLED Data Visualization:** The receiver displays distance, a visual proximity bar, and signal quality (RSSI/SNR) on a 0.96" SSD1306 OLED display.
*   **Configurable Parameters:** Easily modify network ID, device addresses, and transmission intervals directly in the code.
*   **Robust Error Handling:** The receiver includes a timeout to display a "No Signal" message if data is not received, and the transmitter handles out-of-range sensor readings.

## Hardware Requirements

### Common Components
*   2x ESP32 WROOM 32 Development Boards
*   2x REYAX RYLR998 LoRa Modules
<img width="30%" alt="image" src="https://github.com/user-attachments/assets/e1926f5d-3fd6-4e7f-961f-77f06494746d" />


### Transmitter
*   1x HC-SR04 Ultrasonic Sensor
<img width="30%" alt="image" src="https://github.com/user-attachments/assets/584af6fd-6788-4b15-89ba-7f0b5cb2b440" />

### Receiver
*   1x 0.96" I2C OLED Display (SSD1306)
<img width="30%" alt="image" src="https://github.com/user-attachments/assets/a87082b9-9903-41e5-bbbf-c03afd2f5b36" />

## Software & Libraries

This project is built using the Arduino framework for ESP32. You will need the following libraries installed in your Arduino IDE:
*   [Adafruit GFX Library](https://github.com/adafruit/Adafruit-GFX-Library)
*   [Adafruit SSD1306 Library](https://github.com/adafruit/Adafruit_SSD1306)

You can install these via the Arduino Library Manager (`Sketch` > `Include Library` > `Manage Libraries...`).

## Wiring

### Transmitter Wiring

| ESP32 Pin | Component Pin       |
| :-------- | :------------------ |
| GPIO 25   | HC-SR04 `Trig`      |
| GPIO 26   | HC-SR04 `Echo`      |
| GPIO 14   | RYLR998 `RXD`       |
| GPIO 27   | RYLR998 `TXD`       |
| 5V        | HC-SR04 `VCC`       |
| 3.3V      | RYLR998 `VCC`       |
| GND       | HC-SR04 `GND`, RYLR998 `GND` |

### Receiver Wiring

| ESP32 Pin | Component Pin       |
| :-------- | :------------------ |
| GPIO 33   | OLED `SDA`          |
| GPIO 32   | OLED `SCL`          |
| GPIO 14   | RYLR998 `RXD`       |
| GPIO 27   | RYLR998 `TXD`       |
| 3.3V      | OLED `VCC`, RYLR998 `VCC` |
| GND       | OLED `GND`, RYLR998 `GND` |

## Getting Started

1.  **Hardware Setup:** Wire the transmitter and receiver components as shown in the wiring diagrams above.
2.  **IDE Setup:**
    *   Install the ESP32 board manager in your Arduino IDE.
    *   Install the required libraries listed in the [Software & Libraries](#software--libraries) section.
3.  **Configuration:**
    *   Open `ModuLoRa_TX-RX/ModuLoRa_transmitter/ModuLoRa_transmitter.ino`.
    *   Open `ModuLoRa_TX-RX/ModuLoRa_reciever/ModuLoRa_reciever.ino`.
    *   Ensure the `NETWORK_ID` is the same in both files (default is `6`).
    *   Ensure that the `DEST_ADDRESS` in the transmitter file matches the `LORA_ADDRESS` in the receiver file (default is `2`).
4.  **Upload Code:**
    *   Connect the transmitter ESP32 to your computer. Select your ESP32 board and port, then upload `ModuLoRa_transmitter.ino`.
    *   Connect the receiver ESP32 to your computer. Select your ESP32 board and port, then upload `ModuLoRa_reciever.ino`.
5.  **Operation:**
    *   Power on both devices.
    *   The transmitter will begin measuring distance every 500ms and broadcasting it via LoRa.
    *   The receiver's OLED screen will display "READY" and then show the distance data as it arrives.

## Code Configuration

You can easily customize the behavior by changing the `#define` values at the top of each `.ino` file.

### `ModuLoRa_transmitter.ino`

*   `MEASURE_INTERVAL`: The time in milliseconds between each distance measurement and transmission (default: `500`).
*   `LORA_ADDRESS`: The unique address of the transmitter module (default: `1`).
*   `DEST_ADDRESS`: The address of the target receiver module (default: `2`).
*   `NETWORK_ID`: The LoRa network ID. Must match the receiver's ID (default: `6`).

### `ModuLoRa_reciever.ino`

*   `LORA_ADDRESS`: The unique address of the receiver module (default: `2`).
*   `NETWORK_ID`: The LoRa network ID. Must match the transmitter's ID (default: `6`).
*   `SDA_PIN`/`SCL_PIN`: I2C pins for the OLED display if you need to change them.

## Special Thanks to REYAX!

A heartfelt thank you to **[REYAX Technology](https://reyax.com/)** for providing the **[RYLR998 LoRa modules](https://reyax.com/products/RYLR998)** used in this project. Their support made this development possible and allowed for real-world testing of the long-range communication capabilities showcased in ModuLoRa-Framework.

## License

This project is licensed under the Apache License, Version 2.0. See the [LICENSE](LICENSE) file for the full license text.
