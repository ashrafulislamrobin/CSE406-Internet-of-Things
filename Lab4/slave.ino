// ESP32 I2C Slave – Lab 04 (Checksum + ACK)
#include <Wire.h>

#define I2C_DEV_ADDR 0x55

volatile unsigned long rx_count = 0;
volatile uint8_t last_checksum = 0;

void onReceive(int len) {
  uint8_t sum = 0;
  Serial.printf("onReceive[%d]: ", len);
  while (Wire.available()) {
    uint8_t b = Wire.read();
    sum ^= b;
    Serial.write(b);
  }
  Serial.println();
  last_checksum = sum;
  rx_count++;
}

void onRequest() {
  Wire.write(last_checksum);
  Serial.println("onRequest fired (ACK sent)");
}

void setup() {
  Serial.begin(115200);
  Wire.onReceive(onReceive);
  Wire.onRequest(onRequest);
  Wire.begin((uint8_t)I2C_DEV_ADDR);
  Serial.println("I2C Slave ready");
}

void loop() {
  static uint32_t t0 = 0;
  if (millis() - t0 > 2000) {
    t0 = millis();
    Serial.printf("rx_count=%lu\n", rx_count);
  }
}