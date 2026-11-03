#include "ecran.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "buzzer.h"
#include "motor.h"
#include "scanner.h"
#include "esp_log.h"
#include <stdio.h>

static const char *TAG = "MAIN";

// Define your authorized card UID
#define AUTHORIZED_UID 0x374AC663

void app_main(void)
{
    ecran_init();
    buzzer_init();
    motor_init();
    rfid_init();

    ESP_LOGI(TAG, "System Ready. Wave an RFID tag...");

    while (1) {
        uint32_t scanned_uid = rfid_scan_number();

        if (scanned_uid != 0) {
            ESP_LOGI(TAG, "Card Read: 0x%08X", (unsigned int)scanned_uid);

            if (scanned_uid == AUTHORIZED_UID) {
                ESP_LOGI(TAG, "Access Granted!");
                
                // Play access granted feedback
                buzzer_beep(100);
                open_gate();
                vTaskDelay(pdMS_TO_TICKS(3000)); // Hold open for 3 seconds
                close_gate();
            } else {
                ESP_LOGW(TAG, "Access Denied! Unauthorized Card: 0x%08X", (unsigned int)scanned_uid);
                
                // Error feedback (2 quick beeps)
                buzzer_beep(100);
                vTaskDelay(pdMS_TO_TICKS(100));
                buzzer_beep(100);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(200)); // Small polling delay
    }
}