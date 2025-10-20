// Define the analog pin connected to the sensor's AO (Analog Output) pin.
const int WATER_SENSOR_PIN = A0; 


const int SENSOR_MIN_VALUE = 0;   // Analog reading when sensor is completely dry (0 is common)
const int SENSOR_MAX_VALUE = 1024; // Analog reading when sensor is fully submerged (1024 is max for 10-bit ADC)

void setup() {
  // Initialize serial communication for monitoring
  Serial.begin(9600);
  Serial.println("\n--- Water Level Monitoring System ---");
  Serial.println("Calibrated Range: " + String(SENSOR_MIN_VALUE) + " to " + String(SENSOR_MAX_VALUE));
}

void loop() {
  // 1. Read the raw analog value from the sensor
  int rawValue = analogRead(WATER_SENSOR_PIN);

  // 2. Map the raw reading to a percentage (0% to 100%)
  // constrain() ensures the raw value is within the MIN/MAX range before mapping
  int constrainedValue = constrain(rawValue, SENSOR_MIN_VALUE, SENSOR_MAX_VALUE);
  int waterLevelPercent = map(constrainedValue, SENSOR_MIN_VALUE, SENSOR_MAX_VALUE, 0, 100);

  // 3. Determine the water level status based on the percentage
  String status;
  if (waterLevelPercent == 0) {
    status = "DRY (No Water Detected)";
  } else if (waterLevelPercent > 0 && waterLevelPercent <= 30) {
    status = "LOW (Caution)";
  } else if (waterLevelPercent > 30 && waterLevelPercent <= 75) {
    status = "MEDIUM (Stable)";
  } else {
    status = "HIGH (Full)";
  }

  // 4. Print the results to the Serial Monitor
  Serial.print("Raw Reading: ");
  Serial.print(rawValue);
  
  Serial.print("\t| Water Level: ");
  Serial.print(waterLevelPercent);
  Serial.print("%");
  
  Serial.print("\t| Status: ");
  Serial.println(status);
  
  // Wait before the next reading
  delay(1000); 
}