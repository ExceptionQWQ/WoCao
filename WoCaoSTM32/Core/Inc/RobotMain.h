#ifndef ROBOTMAIN_H
#define ROBOTMAIN_H

#include "main.h"
#include "WheelPWM.h"
#include "RobotMotion.h"
#include "ServoDriver.h"
#include "OpenMVDriver.h"
#include "MediumBlur.h"
#include "LCD/lcd.h"
#include "LCD/GUI.h"

#ifdef __cplusplus
#include <cmath>
#include <cstring>
#include <cstdio>
#endif



#ifdef __cplusplus
extern RobotMotion robotMotion;
extern ServoDriver servoDriver;
extern OpenMVDriver openMVDriver;
#endif


#ifdef __cplusplus
extern "C"
{
#endif
void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);

void RobotMain();

#ifdef __cplusplus
}
#endif


#endif