/*
 * AUTHOR: @emensta
 *
 * ESP32 TRANSMITTER - Ultrasonic Sensor + RYLR998 LoRa Module
 * Measures distance with HC-SR04 and transmits via REYAX RYLR998
 * 
 * Components:
 * - ESP32 WROOM 32
 * - HC-SR04 Ultrasonic Sensor
 * - REYAX RYLR998 LoRa Module (868/915MHz)
 *
 */

// HC-SR04 Ultrasonic Sensor pins
#define TRIG_PIN 25
#define ECHO_PIN 26

// REYAX RYLR998 LoRa module pins (using Serial2)
#define LORA_RX 27  // Connect to TX of RYLR998 (GPIO 27 = RX)
#define LORA_TX 14  // Connect to RX of RYLR998 (GPIO 14 = TX)

// Measurement interval
#define MEASURE_INTERVAL 500  // Send data every 500ms (2Hz for reliable transmission)

// LoRa config
#define LORA_ADDRESS 1     // This device address
#define DEST_ADDRESS 2     // Receiver address
#define NETWORK_ID 6       // Network ID (0-255)

unsigned long lastMeasurement = 0;

void setup() {
  // Initialize serial
  Serial.begin(115200);
  delay(1000);
  Serial.println("ESP32 Transmitter Starting...");
  
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  // Initialize LoRa Serial com
  Serial2.begin(115200, SERIAL_8N1, LORA_RX, LORA_TX);
  delay(1000);
  
  configureLora();
  
  Serial.println("RYLR998 Transmitter ready!");
}

void loop() {
  if (millis() - lastMeasurement >= MEASURE_INTERVAL) {
    lastMeasurement = millis();
    
    float distance = measureDistance();
    
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
    
    transmitDistance(distance);
  }
  
  // Check for LoRa responses (optional)
  if (Serial2.available()) {
    String response = Serial2.readStringUntil('\n');
    Serial.print("LoRa: ");
    Serial.println(response);
  }
}

// Measure distance using HC-SR04
float measureDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  
  // Send 10us pulse
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  // Read echo pulse
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);  // Timeout 30ms
  
  // Calculate distance in cm
  // Speed of sound = 343 m/s = 0.0343 cm/us
  // Distance = (duration / 2) * 0.0343
  float distance = (duration * 0.0343) / 2;
  
  // Handle out of range readings
  if (distance == 0 || distance > 400) {
    distance = -1;  // Invalid reading
  }
  
  return distance;
}

// Transmit distance via LoRa
void transmitDistance(float distance) {
  String data = String(distance, 1);  // 1 decimal place
  
  String command = "AT+SEND=" + String(DEST_ADDRESS) + "," + 
                   String(data.length()) + "," + data + "\r\n";
  
  Serial2.print(command);
  
  Serial.print("Sent: ");
  Serial.println(data);
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
  
  // IMPORTANT: Reduce transmission power to prevent brownout
  // Set to 10 dBm (lower power = less current draw)
  // Range: 5-22 dBm (default is often 15+)
  sendLoraCommand("AT+CRFOP=10");
  delay(100);
  
  // Optional: Optimize for lower power consumption
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
