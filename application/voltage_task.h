#ifndef VOLTAGE_TASK_H
#define VOLTAGE_TASK_H

#include <stdint.h>

void battery_voltage_task_init(void);
uint16_t get_battery_percentage(void);

#endif
