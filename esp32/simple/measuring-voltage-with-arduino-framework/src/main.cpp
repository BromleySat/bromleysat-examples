#include <Arduino.h>

#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

#define DEFAULT_VREF 1100 // Use adc2_vref_to_gpio() to obtain a better estimate
#define VREF_BASE 1135
#define VOLTAGE_ADJUSTMENT_MULTIPLIER 0.005f; // equivalent of dividing by 200. no idea why this works ¯\_(ツ)_/¯

#ifndef CONFIG_IDF_TARGET_ESP32
#error "This example is configured for ESP32."
#endif

static esp_adc_cal_characteristics_t *adc_chars;
static const adc1_channel_t channel = ADC1_CHANNEL_4; // GPIO32 if ADC1
static const adc_bits_width_t width = ADC_WIDTH_BIT_12;

static const adc_atten_t atten = ADC_ATTEN_DB_11; // ADC_ATTEN_MAX; // ADC_ATTEN_0db; // ADC_ATTEN_DB_6; // ADC_ATTEN_DB_11;
static const adc_unit_t unit = ADC_UNIT_1;

static uint32_t vrefAdjust = 0;
float voltage = 0;

static void route_vref_to_gpio()
{
  esp_err_t status = adc_vref_to_gpio(ADC_UNIT_1, GPIO_NUM_33);
  if (status == ESP_OK)
  {
    Serial.println("v_ref routed to GPIO\n");
  }
  else
  {
    Serial.println("failed to route v_ref\n");
  }
}

static void check_efuse(void)
{
  // Check if TP is burned into eFuse
  if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK)
  {
    Serial.println("eFuse Two Point: Supported\n");
  }
  else
  {
    Serial.println("eFuse Two Point: NOT supported\n");
  }
  // Check Vref is burned into eFuse
  if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_VREF) == ESP_OK)
  {
    Serial.println("eFuse Vref: Supported\n");
  }
  else
  {
    Serial.println("eFuse Vref: NOT supported\n");
  }
}

static void print_char_val_type(esp_adc_cal_value_t val_type)
{
  if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP)
  {
    Serial.println("Characterized using Two Point Value\n");
  }
  else if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF)
  {
    Serial.println("Characterized using eFuse Vref\n");
  }
  else
  {
    Serial.println("Characterized using Default Vref\n");
  }
}

void setup()
{
  Serial.begin(9600);

  check_efuse();

  // Configure ADC
  adc1_config_width(width);
  adc1_config_channel_atten(channel, atten);

  // Characterize ADC
  adc_chars = (esp_adc_cal_characteristics_t *)calloc(1, sizeof(esp_adc_cal_characteristics_t));
  esp_adc_cal_value_t val_type = esp_adc_cal_characterize(unit, atten, width, DEFAULT_VREF, adc_chars);
  print_char_val_type(val_type);
  vrefAdjust = VREF_BASE - adc_chars->vref;
  Serial.print("vRef: ");
  Serial.println(adc_chars->vref);

  // does not work on our devices
  // route_vref_to_gpio();
}

void loop()
{
  voltage = 0.0f;
  auto adc_reading = adc1_get_raw((adc1_channel_t)channel);

  uint32_t voltageRaw = 0;
  if (adc_reading > 0)
  {
    adc_reading -= vrefAdjust;
    voltageRaw = esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);
    voltage = voltageRaw * VOLTAGE_ADJUSTMENT_MULTIPLIER;
    voltage = round(voltage * 100) / 100;
  }

  Serial.print("voltage: ");
  Serial.println(voltage);
  delay(2000);
}
