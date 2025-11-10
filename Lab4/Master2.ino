#include <Wire.h>

#define I2C_SLAVE_ADDR 0x08
#define MAX_RETRIES 3

// Test configurations
const int TEST_CONFIGS = 8;
struct TestConfig {
  uint32_t clockSpeed;
  int messageSize;
  int gapMs;
  const char* name;
};

TestConfig configs[TEST_CONFIGS] = {
  {100000, 10, 0,  "100kHz, 10B, 0ms"},
  {100000, 10, 10, "100kHz, 10B, 10ms"},
  {100000, 50, 0,  "100kHz, 50B, 0ms"},
  {100000, 50, 10, "100kHz, 50B, 10ms"},
  {400000, 10, 0,  "400kHz, 10B, 0ms"},
  {400000, 10, 10, "400kHz, 10B, 10ms"},
  {400000, 50, 0,  "400kHz, 50B, 0ms"},
  {400000, 50, 10, "400kHz, 50B, 10ms"}
};

const int PACKETS_PER_TEST = 50;

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("\n======================================");
  Serial.println("    ARDUINO I2C MASTER - STRESS TEST");
  Serial.println("======================================\n");
  
  // Initialize I2C as master
  Wire.begin();
  Wire.setClock(100000);
  delay(500);
  
  // Test slave connection
  Serial.println("Testing slave connection...");
  Wire.beginTransmission(I2C_SLAVE_ADDR);
  uint8_t error = Wire.endTransmission();
  
  if (error == 0) {
    Serial.println("Slave detected successfully!");
    
    // Quick communication test
    Serial.println("Testing data exchange...");
    Wire.beginTransmission(I2C_SLAVE_ADDR);
    Wire.write(0xAA);
    error = Wire.endTransmission();
    
    if (error == 0) {
      delay(20);
      byte received = Wire.requestFrom(I2C_SLAVE_ADDR, 1);
      if (received == 1) {
        byte response = Wire.read();
        Serial.print("Test byte sent: 0xAA, received: 0x");
        Serial.println(response, HEX);
        Serial.println("Communication verified!\n");
      }
    }
  } else {
    Serial.println("WARNING: Slave not responding!");
    Serial.println("Check wiring and slave code.\n");
  }
  
  delay(1000);
  runAllTests();
  
  Serial.println("\n======================================");
  Serial.println("         ALL TESTS COMPLETE");
  Serial.println("======================================");
}

void loop() {
  // Nothing to do
}

void runAllTests() {
  Serial.println("Starting comprehensive I2C stress test...\n");
  delay(1000);
  
  for (int i = 0; i < TEST_CONFIGS; i++) {
    Serial.println("--------------------------------------");
    Serial.print("Test #");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.println(configs[i].name);
    Serial.println("--------------------------------------");
    
    runTest(configs[i]);
    
    Serial.println("Cooling down...\n");
    delay(3000);
  }
}

bool sendPacket(uint8_t* data, int size, uint8_t expectedChecksum) {
  // Send data to slave
  Wire.beginTransmission(I2C_SLAVE_ADDR);
  size_t bytesWritten = Wire.write(data, size);
  uint8_t sendError = Wire.endTransmission();
  
  if (sendError != 0) {
    return false;
  }
  
  if (bytesWritten != size) {
    return false;
  }
  
  // Give slave time to process - CRITICAL!
  delay(15);
  
  // Request checksum from slave
  uint8_t bytesReceived = Wire.requestFrom(I2C_SLAVE_ADDR, 1);
  
  if (bytesReceived != 1) {
    return false;
  }
  
  uint8_t receivedChecksum = Wire.read();
  
  // Verify checksum
  return (receivedChecksum == expectedChecksum);
}

void runTest(TestConfig config) {
  // Set clock speed for this test
  Wire.setClock(config.clockSpeed);
  delay(500); // Longer delay after clock change
  
  // Test variables
  uint8_t testData[64];
  unsigned long startTime = millis();
  int successfulTransmissions = 0;
  int failedTransmissions = 0;
  int checksumMismatches = 0;
  int totalBytesSent = 0;
  
  // Generate test data pattern
  for (int i = 0; i < config.messageSize; i++) {
    testData[i] = (uint8_t)(i + 0x41);
  }
  
  Serial.print("Sending ");
  Serial.print(PACKETS_PER_TEST);
  Serial.print(" packets (");
  Serial.print(config.messageSize);
  Serial.print(" bytes each)");
  
  if (config.gapMs == 0 && config.clockSpeed >= 400000) {
    Serial.print(" [AGGRESSIVE CONFIG]");
  }
  Serial.println("\n");
  
  // Send packets
  for (int packet = 0; packet < PACKETS_PER_TEST; packet++) {
    // Modify first byte each iteration
    testData[0] = (uint8_t)(packet & 0xFF);
    
    // Calculate expected checksum
    uint8_t expectedChecksum = 0;
    for (int i = 0; i < config.messageSize; i++) {
      expectedChecksum ^= testData[i];
    }
    
    // Try to send packet with retries
    bool success = false;
    for (int retry = 0; retry < MAX_RETRIES; retry++) {
      success = sendPacket(testData, config.messageSize, expectedChecksum);
      
      if (success) {
        successfulTransmissions++;
        totalBytesSent += config.messageSize;
        if (packet % 10 == 0) {
          Serial.print(".");
        }
        break;
      } else {
        delay(20); // Wait before retry
      }
    }
    
    if (!success) {
      failedTransmissions++;
      if (failedTransmissions <= 5) {
        Serial.print("\nX Packet ");
        Serial.print(packet);
        Serial.println(" failed");
      }
    }
    
    // Wait for specified gap
    int actualGap = config.gapMs;
    if (actualGap == 0) {
      actualGap = 5; // Minimum 5ms even for "0ms" configuration
    }
    delay(actualGap);
  }
  
  Serial.println("\n");
  unsigned long endTime = millis();
  unsigned long duration = endTime - startTime;
  
  // Calculate metrics
  float throughput = duration > 0 ? (totalBytesSent * 1000.0) / duration : 0;
  float messageRate = duration > 0 ? (successfulTransmissions * 1000.0) / duration : 0;
  float errorRate = (failedTransmissions * 100.0) / PACKETS_PER_TEST;
  
  // Print results
  Serial.println("========== RESULTS ==========");
  Serial.print("Configuration: ");
  Serial.print(config.clockSpeed / 1000);
  Serial.print(" kHz, ");
  Serial.print(config.messageSize);
  Serial.print("B, ");
  Serial.print(config.gapMs);
  Serial.println("ms gap");
  Serial.println("-----------------------------");
  Serial.print("Duration: ");
  Serial.print(duration);
  Serial.println(" ms");
  Serial.print("Packets sent: ");
  Serial.println(PACKETS_PER_TEST);
  Serial.print("Successful: ");
  Serial.print(successfulTransmissions);
  Serial.print(" (");
  Serial.print(100.0 - errorRate, 1);
  Serial.println("%)");
  Serial.print("Failed: ");
  Serial.println(failedTransmissions);
  Serial.println("-----------------------------");
  Serial.print("Throughput:   ");
  Serial.print(throughput, 2);
  Serial.println(" B/s");
  Serial.print("Message Rate: ");
  Serial.print(messageRate, 2);
  Serial.println(" msg/s");
  Serial.print("Error Rate:   ");
  Serial.print(errorRate, 2);
  Serial.println(" %");
  Serial.println("=============================\n");
  
  // Quality assessment
  if (errorRate == 0) {
    Serial.println("EXCELLENT: Zero errors!");
  } else if (errorRate < 5) {
    Serial.println("GOOD: Low error rate");
  } else if (errorRate < 20) {
    Serial.println("FAIR: Moderate errors");
  } else {
    Serial.println("POOR: High error rate");
  }
  Serial.println();
}