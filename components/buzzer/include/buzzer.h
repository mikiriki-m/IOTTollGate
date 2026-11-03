#ifndef BUZZER_H
#define BUZZER_H

#include <stdio.h>
#include "driver/gpio.h"

#define BUZZER_GPIO_PIN GPIO_NUM_21

void buzzer_init(void);
void buzzer_on(void);
void buzzer_off(void);
void buzzer_beep(uint32_t duration_ms);

#endif