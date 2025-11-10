#include <Wire.h>

#define I2C_SLAVE_ADDR 0x08

volatile uint8_t receivedData[64];
volatile uint8_t receivedBytes = 0;
volatile uint8_t checksumToSend = 0;
volatile unsigned long packetsReceived = 0;
volatile bool dataReady = false;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n=== ARDUINO I2C SLAVE ===");
  Serial.println("Slave Address: 0x08");
  
  // Initialize I2C as slave
  Wire.begin(I2C_SLAVE_ADDR);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  
  Serial.println("I2C Slave Ready");
  Serial.println("Waiting for data...\n");
}

void loop() {
  // Print received data in main loop (not in ISR)
  if (dataReady) {
    dataReady = false;
    
    Serial.print("Packet #");
    Serial.print(packetsReceived);
    Serial.print(" | Bytes: ");
    Serial.print(receivedBytes);
    Serial.print(" | Checksum: 0x");
    Serial.print(checksumToSend, HEX);
    Serial.print(" | Data: ");
    for (int i = 0; i < min(5, (int)receivedBytes); i++) {
      Serial.print("0x");
      if (receivedData[i] < 16) Serial.print("0");
      Serial.print(receivedData[i], HEX);
      Serial.print(" ");
    }
    if (receivedBytes > 5) Serial.print("...");
    Serial.println();
  }
  delay(1);
}

// Called when master sends data - KEEP SHORT!
void receiveEvent(int numBytes) {
  receivedBytes = 0;
  checksumToSend = 0;
  
  // Read all bytes and calculate checksum
  while (Wire.available() && receivedBytes < 64) {
    receivedData[receivedBytes] = Wire.read();
    checksumToSend ^= receivedData[receivedBytes];
    receivedBytes++;
  }
  
  // Clear any remaining bytes
  while (Wire.available()) {
    Wire.read();
  }
  
  packetsReceived++;
  dataReady = true;
}

// Called when master requests data - KEEP SHORT!
void requestEvent() {
  Wire.write(checksumToSend);
}