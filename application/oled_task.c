/****************************************************************************
 *  Copyright (C) 2020 RoboMaster.
 *  OLED task: SSD1327 on I2C2 (PF0/PF1), aligned with RoboMaster
 *  Development-Board-C 20.standard_robot (ssd1306 + oled_task layout).
 ****************************************************************************/

#include "cmsis_os.h"
#include "main.h"
#include "oled_task.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "oledfont.h"
#include "offline_service.h"
#include "voltage_task.h"
#include "FreeRTOS.h"
#include "task.h"

#define OLED_CONTROL_TIME_MS 10U
#define OLED_REFRESH_DIV     10U

static uint8_t refresh_div;

static offline_event motor_gimbal_offline(uint8_t toe_idx)
{
    /* toe_idx 1..7 matches detect_task CHASSIS_MOTOR1..TRIGGER */
    switch (toe_idx)
    {
    case 1:
        return OFFLINE_CHASSIS_MOTOR1;
    case 2:
        return OFFLINE_CHASSIS_MOTOR2;
    case 3:
        return OFFLINE_CHASSIS_MOTOR3;
    case 4:
        return OFFLINE_CHASSIS_MOTOR4;
    case 5:
        return OFFLINE_GIMBAL_YAW;
    case 6:
        return OFFLINE_GIMBAL_PITCH;
    case 7:
        return OFFLINE_GIMBAL_TURN_MOTOR;
    default:
        return OFFLINE_DBUS;
    }
}

static offline_event sensor_offline(uint8_t toe_idx)
{
    /* toe_idx 8..11: gyro, accel, mag, referee — map to closest RoboRTS events */
    switch (toe_idx)
    {
    case 8:
        return OFFLINE_SINGLE_GYRO;
    case 9:
        return OFFLINE_MANIFOLD2_HEART;
    case 10:
        return OFFLINE_GIMBAL_INFO;
    case 11:
        return OFFLINE_CONTROL_CMD;
    default:
        return OFFLINE_DBUS;
    }
}

void oled_task(void const *argument)
{
    (void)argument;

    osDelay(1000);

    ssd1306_Init();
    ssd1306_Fill(Black);
    ssd1306_UpdateScreen();
    ssd1306_Logo();
    ssd1306_UpdateScreen();
    osDelay(3000);
    ssd1306_Fill(Black);
    ssd1306_UpdateScreen();

    refresh_div = 0;

    while (1)
    {
        refresh_div++;
        if (refresh_div >= (configTICK_RATE_HZ / OLED_CONTROL_TIME_MS) / OLED_REFRESH_DIV)
        {
            refresh_div = 0;

            ssd1306_Fill(Black);

            ssd1306_show_graphic(0, 1, &battery_box);
            ssd1306_SetCursor(3, 4);
#ifndef DISABLE_BATTERY_ADC
            ssd1306_printf(Font_6x8, White, "%3u", (unsigned)get_battery_percentage());
#else
            ssd1306_printf(Font_6x8, White, "---");
#endif

            ssd1306_SetCursor(90, 27);
            ssd1306_printf(Font_6x8, White, "DBUS");
            ssd1306_show_graphic(115, 27,
                                 &check_box[offline_manage[OFFLINE_DBUS].online_state]);

            for (uint8_t i = 1; i < (uint8_t)(7 + 1); i++)
            {
                uint8_t show_col = (uint8_t)(((i - 1U) * 32U) % 128U);
                uint8_t show_row = (uint8_t)(15U + (i - 1U) / 4U * 12U);
                offline_event ev = motor_gimbal_offline(i);

                ssd1306_SetCursor(show_col, show_row);
                ssd1306_WriteChar('M', Font_6x8, White);
                ssd1306_SetCursor((uint8_t)(show_col + 6), show_row);
                ssd1306_WriteChar((char)('0' + i), Font_6x8, White);
                ssd1306_show_graphic((uint8_t)(show_col + 12), show_row,
                                     &check_box[offline_manage[ev].online_state]);
            }

            static const char other_name[4][5] = { "GYR\0", "ACC\0", "MAG\0", "REF\0" };

            for (uint8_t i = 8; i < (uint8_t)(11 + 1); i++)
            {
                uint8_t show_col = (uint8_t)((i * 32U) % 128U);
                uint8_t show_row = (uint8_t)(15U + i / 4U * 12U);
                offline_event ev = sensor_offline(i);

                ssd1306_SetCursor(show_col, show_row);
                ssd1306_printf(Font_6x8, White, "%s", other_name[i - 8U]);
                ssd1306_show_graphic((uint8_t)(show_col + 18U), show_row,
                                     &check_box[offline_manage[ev].online_state]);
            }

            ssd1306_UpdateScreen();
        }

        osDelay(OLED_CONTROL_TIME_MS);
    }
}

static osThreadId s_oled_task_handle;

void oled_task_init(void)
{
    osThreadDef(OLED_TASK, oled_task, osPriorityBelowNormal, 0, 1024);
    s_oled_task_handle = osThreadCreate(osThread(OLED_TASK), NULL);
    (void)s_oled_task_handle;
}
