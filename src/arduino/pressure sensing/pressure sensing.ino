#include <Wire.h>
#include <Adafruit_MPRLS.h>

Adafruit_MPRLS mpr = Adafruit_MPRLS();

float pressure_zero = 0.0;
float filtered_pressure = 0.0;

void setup() {
  Serial.begin(9600);

  if (!mpr.begin()) {
    Serial.println("Sensor not found");
    while (1);
  }

  // Keep sensor tube OPEN to air during startup
  delay(3000);

  // Take many readings and average them to get stable zero
  float sum = 0.0;
  int count = 0;

  for (int i = 0; i < 50; i++) {
    float p = mpr.readPressure();

    if (!isnan(p) && p > 0 && p < 2000) {
      sum += p;
      count++;
    }

    delay(20);
  }

  if (count == 0) {
    Serial.println("Failed to initialize zero");
    while (1);
  }

  pressure_zero = sum / count;

  Serial.print("Zero pressure = ");
  Serial.println(pressure_zero, 2);

  filtered_pressure = 0;

  Serial.println("Pressure sensor ready");
}

void loop() {
  float pressure_now = mpr.readPressure();

  // Ignore invalid readings
  if (isnan(pressure_now) || pressure_now <= 0 || pressure_now > 2000) {
    delay(50);
    return;
  }

  float chamber_pressure = pressure_now - pressure_zero;

  // Reject impossible jumps caused by bad reads
  if (abs(chamber_pressure - filtered_pressure) > 30) {
    chamber_pressure = filtered_pressure;
  }

  // Tiny noise around atmosphere should become exactly zero
  if (abs(chamber_pressure) < 0.5) {
    chamber_pressure = 0;
  }

  // Gentle smoothing without runaway drift
  filtered_pressure = 0.7 * filtered_pressure + 0.3 * chamber_pressure;

  // Force nearly-zero values to exactly zero
  if (abs(filtered_pressure) < 0.3) {
    filtered_pressure = 0;
  }

  Serial.println(filtered_pressure, 2);

  delay(50);
}