// ESP32 I2C Master – Comparative Stress Test
#include <Wire.h>

#define I2C_DEV_ADDR 0x55
#define NUM_PACKETS  20

struct Cfg { uint32_t hz; uint8_t size; uint16_t gap_ms; };

Cfg grid[] = {
  {100000,10,0}, {100000,10,10},
  {100000,50,0}, {100000,50,10},
  {400000,10,0}, {400000,10,10},
  {400000,50,0}, {400000,50,10},
};

uint8_t payload[128];

static inline uint8_t checksum(const uint8_t* p, uint8_t n){
  uint8_t s = 0; for(uint8_t i = 0; i < n; i++) s ^= p[i]; return s;
}

void run_one(const Cfg& c){
  Wire.setClock(c.hz);
  for (uint8_t i = 0; i < c.size; i++) payload[i] = 'A' + (i % 26);

  uint32_t sent = 0, valid = 0;
  uint32_t t0 = millis();

  for (uint16_t k = 0; k < NUM_PACKETS; k++){
    uint8_t chk = checksum(payload, c.size);

    Wire.beginTransmission(I2C_DEV_ADDR);
    Wire.write((uint8_t)(k >> 8)); Wire.write((uint8_t)k);  // sequence
    Wire.write(c.size);
    Wire.write(payload, c.size);
    Wire.write(chk);
    uint8_t err = Wire.endTransmission(true);
    if (err == 0) sent++;

    if (Wire.requestFrom(I2C_DEV_ADDR, (uint8_t)1) == 1) {
      uint8_t ack = Wire.read();
      if (ack == chk && err == 0) valid++;
    }

    if (c.gap_ms) delay(c.gap_ms); else delayMicroseconds(500);
  }

  float T = (millis() - t0) / 1000.0f;
  float thr = (valid * c.size) / T;
  float rate = (float)valid / T;
  float errp = sent ? (100.0f * (sent - valid) / sent) : 100.0f;

  Serial.printf("F=%luHz size=%uB gap=%ums | sent=%lu valid=%lu T=%.2fs | "
                "thr=%.1fB/s rate=%.2f/s err=%.1f%%\n",
                (unsigned long)c.hz, c.size, c.gap_ms,
                (unsigned long)sent, (unsigned long)valid, T, thr, rate, errp);
}

void setup(){
  Serial.begin(115200);
  Wire.begin();
  Serial.println("I2C Master (test program) ready");
  for (auto &c : grid) {
    Serial.printf("=== %lu Hz, %u B, %u ms ===\n",
                  (unsigned long)c.hz, c.size, c.gap_ms);
    run_one(c);
    delay(300);
  }
  Serial.println("All tests done.");
}

void loop() {}