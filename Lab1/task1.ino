const int sensorPin = A0;

// Adjust these values after calibration
const int dryValue = 300;    // Example: sensor reading when dry
const int wetValue = 800;    // Example: sensor reading when fully submerged

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  int sensorValue = analogRead(sensorPin);

  // Map the raw value (dryValue–wetValue) to a percentage (0–100)
  int percentage = map(sensorValue, dryValue, wetValue, 0, 100);

  // Ensure percentage stays within 0–100 range
  percentage = constrain(percentage, 0, 100);

  // Print both raw and percentage values, separated by a comma
  Serial.print(sensorValue);
  Serial.print(", ");
  Serial.println(percentage);

  delay(500);  // wait 0.5 second
}
