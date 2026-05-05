#include "bsp_adc.h"
#include "adc.h"
#include "main.h"

volatile float voltage_vrefint_proportion = 8.0586080586080586080586080586081e-4f;

static uint16_t adcx_get_chx_value(ADC_HandleTypeDef *ADCx, uint32_t ch)
{
  ADC_ChannelConfTypeDef sConfig = {0};

  HAL_ADC_Stop(ADCx);

  sConfig.Channel = ch;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;

  if (HAL_ADC_ConfigChannel(ADCx, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_ADC_Start(ADCx);
  HAL_ADC_PollForConversion(ADCx, 50);
  return (uint16_t)HAL_ADC_GetValue(ADCx);
}

void init_vrefint_reciprocal(void)
{
  uint32_t total_adc = 0;
  for (int i = 0; i < 200; i++)
  {
    total_adc += adcx_get_chx_value(&hadc1, ADC_CHANNEL_VREFINT);
  }
  voltage_vrefint_proportion = 200.0f * 1.2f / (float)total_adc;
}

float get_battery_voltage(void)
{
  uint16_t adcx = adcx_get_chx_value(&hadc3, ADC_CHANNEL_8);
  return (float)adcx * voltage_vrefint_proportion * 10.090909090909090909090909090909f;
}
