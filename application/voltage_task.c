/****************************************************************************
 *  24 V rail ADC task — same model as RoboMaster 20.standard_robot/voltage_task.
 ****************************************************************************/

#include "voltage_task.h"
#include "main.h"
#include "cmsis_os.h"

#ifndef DISABLE_BATTERY_ADC
#include "bsp_adc.h"
#endif

#define VOLTAGE_DROP 0.00f

#ifndef DISABLE_BATTERY_ADC

static float battery_voltage;
static float electricity_percentage;

static float calc_battery_percentage(float voltage)
{
  float percentage;
  float voltage_2 = voltage * voltage;
  float voltage_3 = voltage_2 * voltage;

  if (voltage < 19.5f)
  {
    percentage = 0.0f;
  }
  else if (voltage < 21.9f)
  {
    percentage = 0.005664f * voltage_3 - 0.3386f * voltage_2 + 6.765f * voltage - 45.17f;
  }
  else if (voltage < 25.5f)
  {
    percentage = 0.02269f * voltage_3 - 1.654f * voltage_2 + 40.34f * voltage - 328.4f;
  }
  else
  {
    percentage = 1.0f;
  }
  if (percentage < 0.0f)
  {
    percentage = 0.0f;
  }
  else if (percentage > 1.0f)
  {
    percentage = 1.0f;
  }
  return percentage;
}

static void battery_voltage_thread(void const *argument)
{
  (void)argument;
  osDelay(1000);
  init_vrefint_reciprocal();
  for (;;)
  {
    battery_voltage = get_battery_voltage() + VOLTAGE_DROP;
    electricity_percentage = calc_battery_percentage(battery_voltage);
    osDelay(100);
  }
}

uint16_t get_battery_percentage(void)
{
  return (uint16_t)(electricity_percentage * 100.0f);
}

float get_battery_voltage_volts(void)
{
  return battery_voltage;
}

static osThreadId s_volt_task;

void battery_voltage_task_init(void)
{
  osThreadDef(VOLTAGE_TASK, battery_voltage_thread, osPriorityBelowNormal, 0, 256);
  s_volt_task = osThreadCreate(osThread(VOLTAGE_TASK), NULL);
  (void)s_volt_task;
}

#else /* DISABLE_BATTERY_ADC */

void battery_voltage_task_init(void)
{
}

uint16_t get_battery_percentage(void)
{
  return 0;
}

float get_battery_voltage_volts(void)
{
  return 0.0f;
}

#endif
