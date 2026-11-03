#ifndef ECRAN_H
#define ECRAN_H

#include <stdio.h>
#include "driver/i2c.h"

#define LCD_I2C_SDA_PIN GPIO_NUM_20
#define LCD_I2C_SCL_PIN GPIO_NUM_19
#define LCD_ADDR 0x27       

void ecran_init(void);
void ecran_show_welcome(void);
void ecran_show_success(const char* balance_str);
void ecran_show_insufficient_funds(void);
void ecran_show_error(const char* err_msg);

#endif
