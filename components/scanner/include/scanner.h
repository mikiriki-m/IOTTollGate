
#ifndef SCANNER_H
#define SCANNER_H

#include <stdio.h>
#include "driver/gpio.h"
#include "esp_err.h"
#include "rc522.h"

#define RFID_SPI_MISO_PIN   5
#define RFID_SPI_MOSI_PIN   12
#define RFID_SPI_SCK_PIN    13
#define RFID_SPI_SDA_PIN    14

esp_err_t rfid_init(void);
uint32_t rfid_scan_number(void);

#endif
