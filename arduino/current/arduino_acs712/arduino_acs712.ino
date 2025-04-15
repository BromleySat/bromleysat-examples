
#define SENSOR_MID_POINT 2.5
#define VOLTS_PER_AMP 0.185

void setup() {
  Serial.begin(9600);
  pinMode(A0, INPUT);
}
 
void loop() {
  float sensorValue = readAnalogWithMultisampling(A0, 200.0);
  //convert the sensor value to current
  float current = (SENSOR_MID_POINT - fmap(sensorValue, 0.0, 1023.0, 0.0, 5.0)) / VOLTS_PER_AMP;

  //print current value
  Serial.print("SensorValue:");
  Serial.println(sensorValue);
  Serial.print("Current:");
  Serial.println(current);

  delay(100); //delay between measurements
}

//function to read analog values from a specific pin
//an average value is calculated based on the sampleSize parameter
//3ms delay between sample reads
float readAnalogWithMultisampling(uint8_t pin, float sampleSize) {
  float analogValue = 0.0, total = 0.0;

  for (int x = 0; x < sampleSize; x++) {
    analogValue = analogRead(pin);
    total = total + analogValue;
    delay(3);
  }
  float sensorValue = total / sampleSize;
}

//function to map sensor value range to the voltage range
float fmap(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
