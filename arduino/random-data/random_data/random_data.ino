
#define VOLTAGE_MIN 0.0
#define VOLTAGE_MAX 5.0
#define CURRENT_MIN -5.0
#define CURRENT_MAX 5.0


float randomFloat(float minVal, float maxVal) {
  return minVal + (random(0, 10001) / 10000.0) * (maxVal - minVal);
}

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(A0));
}

void loop() {
  float voltage = randomFloat(VOLTAGE_MIN, VOLTAGE_MAX);
  float current = randomFloat(CURRENT_MIN, CURRENT_MAX);

  Serial.print("Voltage:");
  Serial.print(voltage, 3);
  Serial.print(" Current2:");
  Serial.println(current, 3);

  delay(200);
}

