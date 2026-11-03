#ifndef MOTOR_H
#define MOTOR_H

#include <stdio.h>
#include "driver/gpio.h"

#define MOTOR_GPIO_IN1 GPIO_NUM_38
#define MOTOR_GPIO_IN2 GPIO_NUM_37
#define MOTOR_GPIO_IN3 GPIO_NUM_36
#define MOTOR_GPIO_IN4 GPIO_NUM_35

void motor_init(void);
void open_gate(void);
void close_gate(void);

#endif
