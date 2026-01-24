/*
 * AUTHOR: @emensta
 *
 * ESP32 RECEIVER - OLED Display + RYLR998 LoRa Module
 * Receives distance data via REYAX RYLR998 and displays on OLED
 * 
 * Components:
 * - ESP32 WROOM 32
 * - 0.96" I2C OLED Display (SSD1306)
 * - REYAX RYLR998 LoRa Module (868/915MHz)
 *
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1  // Reset pin (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C  // Common I2C address (try 0x3D if this doesn't work)

// I2C pins
#define SDA_PIN 33
#define SCL_PIN 32

// REYAX RYLR998 LoRa Module Pins (using Serial2)
#define LORA_RX 27  // Connect to TX of RYLR998 (GPIO 27 = RX)
#define LORA_TX 14  // Connect to RX of RYLR998 (GPIO 14 = TX)

// LoRa config
#define LORA_ADDRESS 2     // This device address
#define NETWORK_ID 6       // Network ID (0-255, must match transmitter)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// var
float lastDistance = -1;
unsigned long lastReceivedTime = 0;
int rssi = 0;
int snr = 0;

void setup() {
  // Initialize serial
  Serial.begin(115200);
  delay(1000);
  Serial.println("ESP32 RYLR998 Receiver Starting...");
  
  Wire.begin(SDA_PIN, SCL_PIN);
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  
  // Clear display and startup msg
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("RYLR998 LoRa Receiver");
  display.println("Initializing...");
  display.display();
  delay(2000);
  
  // Initialize LoRa serial com
  Serial2.begin(115200, SERIAL_8N1, LORA_RX, LORA_TX);
  delay(1000);
  
  configureLora();
  
  Serial.println("RYLR998 Receiver ready!");
  
  displayReady();
}

void loop() {
  // Check for incoming LoRa messages
  if (Serial2.available()) {
    String message = Serial2.readStringUntil('\n');
    Serial.print("Received: ");
    Serial.println(message);
    
    parseLoraMessage(message);
  }
  
  if (lastDistance >= 0 && (millis() - lastReceivedTime > 1500)) {
    displayNoSignal();
    lastDistance = -1;
  }
}

// Parse
void parseLoraMessage(String message) {
  
  if (message.startsWith("+RCV=")) {
    message = message.substring(5);
    
    int firstComma = message.indexOf(',');
    int secondComma = message.indexOf(',', firstComma + 1);
    int thirdComma = message.indexOf(',', secondComma + 1);
    int fourthComma = message.indexOf(',', thirdComma + 1);
    
    if (firstComma > 0 && secondComma > 0 && thirdComma > 0) {
      String dataStr = message.substring(secondComma + 1, thirdComma);
      
      if (fourthComma > 0) {
        rssi = message.substring(thirdComma + 1, fourthComma).toInt();
        snr = message.substring(fourthComma + 1).toInt();
      }
      
      float distance = dataStr.toFloat();
      
      // Update display
      lastDistance = distance;
      lastReceivedTime = millis();
      displayDistance(distance);
      
      Serial.print("Distance: ");
      Serial.print(distance);
      Serial.print(" cm, RSSI: ");
      Serial.print(rssi);
      Serial.print(" dBm, SNR: ");
      Serial.println(snr);
    }
  }
}

// Display distance on OLED
void displayDistance(float distance) {
  display.clearDisplay();
  
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("RYLR998 LoRa Distance");
  display.drawLine(0, 10, SCREEN_WIDTH, 10, SSD1306_WHITE);
  
  if (distance >= 0) {
    display.setTextSize(3);
    display.setCursor(5, 17);
    display.print(distance, 1);
    display.setTextSize(1);
    display.setCursor(90, 28);
    display.print("cm");
    
    // Visual bar (0-100cm)
    int barWidth = map(constrain(distance, 0, 100), 0, 100, 0, SCREEN_WIDTH - 4);
    display.drawRect(0, 45, SCREEN_WIDTH, 10, SSD1306_WHITE);
    display.fillRect(2, 47, barWidth, 6, SSD1306_WHITE);
  } else {
    display.setTextSize(2);
    display.setCursor(10, 25);
    display.println("No Data");
  }
  
  // Signal info
  display.setTextSize(1);
  display.setCursor(0, 56);
  display.print("RSSI:");
  display.print(rssi);
  display.print(" SNR:");
  display.print(snr);
  
  display.display();
}

void displayNoSignal() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("LoRa Distance");
  display.drawLine(0, 10, SCREEN_WIDTH, 10, SSD1306_WHITE);
  
  display.setTextSize(2);
  display.setCursor(5, 25);
  display.println("Waiting for");
  display.setCursor(5, 43);
  display.println("Signal..");
  
  display.display();
}

void displayReady() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("RYLR998 LoRa Receiver");
  display.drawLine(0, 10, SCREEN_WIDTH, 10, SSD1306_WHITE);
  
  display.setTextSize(2);
  display.setCursor(20, 25);
  display.println("READY");
  
  display.setTextSize(1);
  display.setCursor(0, 50);
  display.print("Addr: ");
  display.println(LORA_ADDRESS);
  
  display.display();
}

// Configure LoRa module
void configureLora() {
  Serial.println("Configuring RYLR998 LoRa module...");
  
  // Test connection
  sendLoraCommand("AT");
  delay(100);
  
  // Set device address
  sendLoraCommand("AT+ADDRESS=" + String(LORA_ADDRESS));
  delay(100);
  
  // Set network ID
  sendLoraCommand("AT+NETWORKID=" + String(NETWORK_ID));
  delay(100);
  
  // IMPORTANT: Match transmitter power settings
  // Set to 10 dBm (lower power = less current draw)
  sendLoraCommand("AT+CRFOP=10");
  delay(100);
  
  // Optional: Match transmitter bandwidth settings
  // Bandwidth=125kHz(7), SF=7, CR=1, Preamble=12
  sendLoraCommand("AT+CBRR=7,7,1,12");
  delay(100);
  
  Serial.println("RYLR998 LoRa configuration complete!");
}

// Send AT command to LoRa module
void sendLoraCommand(String command) {
  Serial2.print(command + "\r\n");
  Serial.print("CMD: ");
  Serial.println(command);
  delay(100);
  
  // Read
  if (Serial2.available()) {
    String response = Serial2.readStringUntil('\n');
    Serial.print("Response: ");
    Serial.println(response);
  }
}
