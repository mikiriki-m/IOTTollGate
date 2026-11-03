#include "scanner.h"
#include <stdio.h>
#include "esp_log.h"
#include "rc522.h"
#include "driver/rc522_spi.h"
#include "rc522_picc.h" 

static const char *TAG = "RFID_SCANNER";
static rc522_handle_t scanner_handle = NULL;
static rc522_driver_handle_t driver_handle = NULL;
static uint32_t last_scanned_uid = 0;


static void on_picc_state_changed(void *arg, esp_event_base_t base, int32_t event_id, void *event_data) {
    // 1. Cast event_data to the actual event wrapper struct
    rc522_picc_state_changed_event_t *event = (rc522_picc_state_changed_event_t *)event_data;

    // 2. Extract the picc pointer safely
    if (event != NULL && event->picc != NULL) {
        rc522_picc_t *picc = event->picc;

        // 3. Only act when a tag transitions into the ACTIVE state
        if (picc->state == RC522_PICC_STATE_ACTIVE && picc->uid.length >= 4) {
            last_scanned_uid = ((uint32_t)picc->uid.value[0] << 24) |
                               ((uint32_t)picc->uid.value[1] << 16) |
                               ((uint32_t)picc->uid.value[2] << 8)  |
                               ((uint32_t)picc->uid.value[3]);

            ESP_LOGI(TAG, "TAG DETECTED! Scanned UID: 0x%08X", (unsigned int)last_scanned_uid);
        }
    }
}

esp_err_t rfid_init(void) {
    // 1. Configure SPI Driver
    rc522_spi_config_t driver_config = {
        .host_id = SPI2_HOST,
        .bus_config = &(spi_bus_config_t){
            .miso_io_num = RFID_SPI_MISO_PIN,
            .mosi_io_num = RFID_SPI_MOSI_PIN,
            .sclk_io_num = RFID_SPI_SCK_PIN,
            .quadwp_io_num = -1,
            .quadhd_io_num = -1,
        },
        .dev_config = {
            .clock_speed_hz = 5000000,
            .spics_io_num = RFID_SPI_SDA_PIN,
        },
    };

    esp_err_t ret = rc522_spi_create(&driver_config, &driver_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create RC522 SPI driver: %s", esp_err_to_name(ret));
        return ret;
    }

    // 2. Install the SPI driver (Allocates driver->device & initializes SPI bus)
    ret = rc522_driver_install(driver_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to install RC522 driver: %s", esp_err_to_name(ret));
        return ret;
    }

    // 3. Attach driver to RC522 instance
    rc522_config_t scanner_config = {
        .driver = driver_handle,
    };

    ret = rc522_create(&scanner_config, &scanner_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create RC522 instance: %s", esp_err_to_name(ret));
        return ret;
    }

    // 4. Register callback for state change events
    rc522_register_events(scanner_handle, RC522_EVENT_PICC_STATE_CHANGED, on_picc_state_changed, NULL);
    return rc522_start(scanner_handle);
}

uint32_t rfid_scan_number(void) {
    uint32_t uid = last_scanned_uid;
    last_scanned_uid = 0; // Clear after read
    return uid;
}