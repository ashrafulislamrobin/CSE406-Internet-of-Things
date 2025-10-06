const int sensorPin = A0;

// Calibration values — adjust these based on your sensor readings
const int dryValue = 0;   // raw value when dry
const int wetValue = 661;   // raw value when fully submerged

// Define water level thresholds (in percentage)
const int LOW_LEVEL = 25;
const int MEDIUM_LEVEL = 75;
const int HIGH_LEVEL = 95;

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  int sensorValue = analogRead(sensorPin);

  // Map the raw value (dryValue–wetValue) to percentage (0–100)
  int percentage = map(sensorValue, dryValue, wetValue, 0, 100);
  percentage = constrain(percentage, 0, 100);

  // Print raw and percentage values (for Serial Plotter visualization)
  Serial.print(sensorValue);
  Serial.print(", ");
  Serial.println(percentage);

  // Check thresholds and print alert messages
  if (percentage < LOW_LEVEL) {
    Serial.println("ALERT: LOW WATER LEVEL! (Below 25%)");
    digitalWrite(LED_BUILTIN, LOW);  // LED off
  }
  else if (percentage >= LOW_LEVEL && percentage < MEDIUM_LEVEL) {
    Serial.println("Water level at MEDIUM range (25%–75%)");
    digitalWrite(LED_BUILTIN, LOW);
  }
  else if (percentage >= MEDIUM_LEVEL && percentage < HIGH_LEVEL) {
    Serial.println("Water level approaching HIGH (75%–95%)");
    digitalWrite(LED_BUILTIN, HIGH); // LED on to indicate nearing full
  }
  else if (percentage >= HIGH_LEVEL) {
    Serial.println("ALERT: HIGH WATER LEVEL! (Above 95%)");
    digitalWrite(LED_BUILTIN, HIGH);
  }

  delay(1000); // Update every second
}
