const int sensorPin = A0;
const int buzzerPin = A1;

// Calibration values — adjust these based on your sensor readings
const int dryValue = 0;     // raw value when dry
const int wetValue = 661;   // raw value when fully submerged

// Define water level thresholds (in percentage)
const int LOW_LEVEL = 25;
const int MEDIUM_LEVEL = 75;
const int HIGH_LEVEL = 95;

// Variable to store the previous reading
int previousPercentage = 0;

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
}

void loop() {
  int sensorValue = analogRead(sensorPin);

  // Map the raw value (dryValue–wetValue) to percentage (0–100)
  int percentage = map(sensorValue, dryValue, wetValue, 0, 100);
  percentage = constrain(percentage, 0, 100);

  // Calculate rate of change (difference between current and previous readings)
  int rateOfChange = percentage - previousPercentage;

  // Print current water level and rate of change for Serial Plotter
  Serial.print(percentage);
  Serial.print(", ");
  Serial.println(rateOfChange);

  // Check thresholds and trigger alerts
  if (percentage < LOW_LEVEL) {
    Serial.println("ALERT: LOW WATER LEVEL! (Below 25%)");
    digitalWrite(LED_BUILTIN, LOW);
    noTone(buzzerPin);  // Buzzer OFF
  }
  else if (percentage >= LOW_LEVEL && percentage < MEDIUM_LEVEL) {
    Serial.println("Water level at MEDIUM range (25%–75%)");
    digitalWrite(LED_BUILTIN, LOW);
    noTone(buzzerPin);  // Buzzer OFF
  }
  else if (percentage >= MEDIUM_LEVEL && percentage < HIGH_LEVEL) {
    Serial.println("Water level approaching HIGH (75%–95%)");
    digitalWrite(LED_BUILTIN, HIGH);
    noTone(buzzerPin);  // Buzzer OFF
  }
  else if (percentage >= HIGH_LEVEL) {
    Serial.println("ALERT: HIGH WATER LEVEL! (Above 95%)");
    digitalWrite(LED_BUILTIN, HIGH);
    tone(buzzerPin, 1000);  // Buzzer ON only for HIGH level
  }

  // Update the previous reading
  previousPercentage = percentage;

  delay(1000); // Update every second
}
