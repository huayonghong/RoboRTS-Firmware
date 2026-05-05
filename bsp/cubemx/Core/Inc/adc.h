/**
  ******************************************************************************
  * @file    adc.h
  * @brief   ADC1 (VREFINT calib) + ADC3 (PF10 battery sense), same as RM C board.
  ******************************************************************************
  */
#ifndef __ADC_H
#define __ADC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc3;

void MX_ADC1_Init(void);
void MX_ADC3_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* __ADC_H */
