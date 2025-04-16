
#define SENSOR_MID_POINT 2.5
#define VOLTS_PER_AMP 0.185  //0.185 => 5A  0.100 => 20A 0.066 => 30A

void setup() {
  Serial.begin(9600);
  pinMode(A0, INPUT);
}

void loop() {
  float current = (SENSOR_MID_POINT - readVoltageWithMultiSampling(A0, 200.0)) / VOLTS_PER_AMP;

  Serial.print("Current:");
  Serial.println(current);

  delay(100);  //delay between measurements
}

//reading an average analog value and then converting it to voltage from 0 to 5V
float readVoltageWithMultiSampling(uint8_t pin, float sampleSize) {
  float sensorValue = readAnalogWithMultiSampling(pin, sampleSize);
  return fmap(sensorValue, 0.0, 1023.0, 0.0, 5.0);
}

//function to read analog values from a specific pin
//an average value is calculated based on the sampleSize parameter
//3ms delay between sample reads
float readAnalogWithMultiSampling(uint8_t pin, float sampleSize) {
  float analogValue = 0.0, total = 0.0;

  for (int x = 0; x < sampleSize; x++) {
    analogValue = analogRead(pin);
    total = total + analogValue;
    delay(3);
  }
  float sensorValue = total / sampleSize;
  return sensorValue;
}

//function to map sensor value range to the voltage range
float fmap(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
