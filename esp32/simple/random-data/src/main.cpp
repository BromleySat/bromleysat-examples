#include <Arduino.h>

#define VOLTAGE_MIN 0.0f
#define VOLTAGE_MAX 5.0f
#define CURRENT_MIN -5.0f
#define CURRENT_MAX 5.0f

float randomFloat(float minVal, float maxVal) {
  return minVal + (esp_random() % 10001) / 10000.0f * (maxVal - minVal);
}

void setup() {
  Serial.begin(9600);
  delay(1000);
}

void loop() {
  float voltage = randomFloat(VOLTAGE_MIN, VOLTAGE_MAX);
  float current = randomFloat(CURRENT_MIN, CURRENT_MAX);

  Serial.print("Voltage:");
  Serial.print(voltage, 3);
  Serial.print(" Current:");
  Serial.println(current, 3);

  delay(200);
}
