#include <Kasia.h>

#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

#define DEFAULT_VREF 1100                 // Use adc2_vref_to_gpio() to obtain a better estimate
#define VOLTAGE_DIVIDER_MULTIPLIER 5.0f   // This value is calculated based on the Voltage Divider: https://bromleysat.com/articles/esp32/measuring-voltage/maximum-precision#voltage-divider-multiplier
#define DEVICE_SPECIFIC_ADJUSTMENT 0.006f // set this to 0.0f if your device does not need any adjustments

#ifndef CONFIG_IDF_TARGET_ESP32
#error "This example is configured for ESP32."
#endif

static esp_adc_cal_characteristics_t *adcCharacteristics;
static const adc1_channel_t channel = ADC1_CHANNEL_4; // GPIO32 if ADC1
static const adc_bits_width_t width = ADC_WIDTH_BIT_12;

static const adc_atten_t atten = ADC_ATTEN_DB_11; // ADC_ATTEN_MAX; // ADC_ATTEN_0db; // ADC_ATTEN_DB_6; // ADC_ATTEN_DB_11;
static const adc_unit_t unit = ADC_UNIT_1;

float voltage = 0;
float voltageMultisampling = 0;
float voltageMultisamplingV2 = 0;

static void route_vref_to_gpio()
{
  esp_err_t status = adc_vref_to_gpio(ADC_UNIT_1, GPIO_NUM_33);
  if (status == ESP_OK)
  {
    logInfo("v_ref routed to GPIO\n");
  }
  else
  {
    logInfo("failed to route v_ref\n");
  }
}

static void check_efuse(void)
{
  // Check if TP is burned into eFuse
  if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK)
  {
    logInfo("eFuse Two Point: Supported\n");
  }
  else
  {
    logInfo("eFuse Two Point: NOT supported\n");
  }
  // Check Vref is burned into eFuse
  if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_VREF) == ESP_OK)
  {
    logInfo("eFuse Vref: Supported\n");
  }
  else
  {
    logInfo("eFuse Vref: NOT supported\n");
  }
}

static void print_char_val_type(esp_adc_cal_value_t val_type)
{
  if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP)
  {
    logInfo("Characterized using Two Point Value\n");
  }
  else if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF)
  {
    logInfo("Characterized using eFuse Vref\n");
  }
  else
  {
    logInfo("Characterized using Default Vref\n");
  }
}

void setup()
{
  kasia.bindData("Voltage", &voltage);
  kasia.bindData("VoltageMultisampling", &voltageMultisampling);
  kasia.bindData("VoltageMultisamplingV2", &voltageMultisamplingV2);
  kasia.start("Voltage Sensors", 9600, "<Your-WiFi-SSID>", "<Your-WiFi-Password>");
  //kasia.start("Voltage Sensor");
  delay(1000);

  check_efuse();

  // Configure ADC
  adc1_config_width(width);
  adc1_config_channel_atten(channel, atten);

  // Characterize ADC
  adcCharacteristics = (esp_adc_cal_characteristics_t *)calloc(1, sizeof(esp_adc_cal_characteristics_t));
  esp_adc_cal_value_t val_type = esp_adc_cal_characterize(unit, atten, width, DEFAULT_VREF, adcCharacteristics);

  print_char_val_type(val_type);
  logInfo("vRef: ", adcCharacteristics->vref);

  // does not work on our devices
  // route_vref_to_gpio();
}

void updateVoltageOnce()
{
  auto tempVoltage = 0.0f;
  auto adcReading = adc1_get_raw((adc1_channel_t)channel);

  if (adcReading > 0)
  {
    tempVoltage = esp_adc_cal_raw_to_voltage(adcReading, adcCharacteristics);
    // tempVoltage -= DEVICE_SPECIFIC_ADJUSTMENT * tempVoltage;
    tempVoltage = tempVoltage * VOLTAGE_DIVIDER_MULTIPLIER; // enable this line if you are using a voltage divider
    voltage = round(tempVoltage) / 1000.0f;
  }
}

void updateVoltageMultisampling(uint32_t sampleSize)
{
  logInfo("multisample start");
  auto tempVoltage = 0.0f;
  uint32_t adc_reading = 0;

  for (uint32_t i = 0; i < sampleSize; i++)
  {
    adc_reading += adc1_get_raw((adc1_channel_t)channel);
  }

  adc_reading /= sampleSize;
  tempVoltage = esp_adc_cal_raw_to_voltage(adc_reading, adcCharacteristics);
  // tempVoltage -= DEVICE_SPECIFIC_ADJUSTMENT * tempVoltage;
  tempVoltage = tempVoltage * VOLTAGE_DIVIDER_MULTIPLIER; // enable this line if you are using a voltage divider
  voltageMultisampling = round(tempVoltage) / 1000.0f;
  logInfo("multisample done");
}

void updateVoltageMultisamplingV2(uint32_t sampleSize)
{
  logInfo("multisampleV2 start");
  std::vector<uint32_t> adcReadings(sampleSize);
  auto tempVoltage = 0.0f;

  for (uint32_t i = 0; i < sampleSize; i++)
  {
    adcReadings[i] = adc1_get_raw((adc1_channel_t)channel);
  }

  std::sort(adcReadings.begin(), adcReadings.end());

  auto adc_reading = adcReadings[sampleSize / 2];
  tempVoltage = esp_adc_cal_raw_to_voltage(adc_reading, adcCharacteristics);
  // tempVoltage -= DEVICE_SPECIFIC_ADJUSTMENT * tempVoltage;
  tempVoltage = tempVoltage * VOLTAGE_DIVIDER_MULTIPLIER; // enable this line if you are using a voltage divider
  voltageMultisamplingV2 = round(tempVoltage) / 1000.0f;
  logInfo("multisampleV2 done");
}

void loop()
{
  updateVoltageOnce();
  updateVoltageMultisampling(90);
  updateVoltageMultisamplingV2(10);
  vTaskDelay(pdMS_TO_TICKS(1000));
}
