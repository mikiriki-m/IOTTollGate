#include "ecran.h"
#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char *TAG = "ECRAN";

#define LCD_ADDR 0x27        // Most common I2C address (use 0x3F if blank)
#define LCD_BACKLIGHT 0x08   // Backlight flag bit
#define ENABLE_BIT 0x04      // Enable bit

static void lcd_send_nibble(uint8_t data, uint8_t mode) {
    uint8_t high = (data & 0xF0) | mode | LCD_BACKLIGHT;
    uint8_t buf[2] = { high | ENABLE_BIT, high };
    
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (LCD_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write(cmd, buf, 2, true);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_NUM_0, cmd, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd);
}

static void lcd_send_byte(uint8_t data, uint8_t mode) {
    lcd_send_nibble(data & 0xF0, mode);
    lcd_send_nibble((data << 4) & 0xF0, mode);
}

static void lcd_send_cmd(uint8_t cmd) {
    lcd_send_byte(cmd, 0x00);
}

static void lcd_send_data(uint8_t data) {
    lcd_send_byte(data, 0x01);
}

static void lcd_clear(void) {
    lcd_send_cmd(0x01);
    vTaskDelay(pdMS_TO_TICKS(5));
}

static void lcd_print(const char *str) {
    while (*str) {
        lcd_send_data((uint8_t)(*str++));
    }
}

void ecran_init(void) {
    ESP_LOGI(TAG, "Initializing I2C LCD...");
    
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = LCD_I2C_SDA_PIN,
        .scl_io_num = LCD_I2C_SCL_PIN,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 50000, // Lowered to 50 kHz for better signal stability
    };
    i2c_param_config(I2C_NUM_0, &conf);
    i2c_driver_install(I2C_NUM_0, conf.mode, 0, 0, 0);

    // CRITICAL: Give the LCD chip 100ms to boot up after power is applied
    vTaskDelay(pdMS_TO_TICKS(100));

    // Force HD44780 controller reset sequence into 4-bit mode
    lcd_send_nibble(0x30, 0x00);
    vTaskDelay(pdMS_TO_TICKS(10));
    lcd_send_nibble(0x30, 0x00);
    vTaskDelay(pdMS_TO_TICKS(10));
    lcd_send_nibble(0x30, 0x00);
    vTaskDelay(pdMS_TO_TICKS(10));
    
    // Switch to 4-bit mode
    lcd_send_nibble(0x20, 0x00);
    vTaskDelay(pdMS_TO_TICKS(10));

    // Function set: 2-line, 5x8 font
    lcd_send_cmd(0x28);
    vTaskDelay(pdMS_TO_TICKS(2));
    
    // Display control: Display ON, Cursor OFF, Blink OFF
    lcd_send_cmd(0x0C);
    vTaskDelay(pdMS_TO_TICKS(2));
    
    // Clear display
    lcd_send_cmd(0x01);
    vTaskDelay(pdMS_TO_TICKS(10));
    
    // Entry mode set: Increment cursor
    lcd_send_cmd(0x06);
    vTaskDelay(pdMS_TO_TICKS(2));
    
    ecran_show_welcome();
}

void ecran_show_welcome(void) {
    lcd_clear();
    lcd_send_cmd(0x80); // Move cursor to Row 0, Col 0
    lcd_print("    WELCOME!    ");
    lcd_send_cmd(0xC0); // Move cursor to Row 1, Col 0
    lcd_print("PLEASE SCAN CARD");
}

void ecran_show_success(const char* balance_str) {
    lcd_clear();
    lcd_send_cmd(0x80);
    lcd_print(" ACCESS GRANTED ");
    lcd_send_cmd(0xC0);
    char line2[17];
    snprintf(line2, sizeof(line2), "BAL: %s", balance_str ? balance_str : "OK");
    lcd_print(line2);
}

void ecran_show_insufficient_funds(void) {
    lcd_clear();
    lcd_send_cmd(0x80);
    lcd_print(" ACCESS DENIED  ");
    lcd_send_cmd(0xC0);
    lcd_print("NO FUNDS AVAIL  ");
}

void ecran_show_error(const char* err_msg) {
    lcd_clear();
    lcd_send_cmd(0x80);
    lcd_print(" SYSTEM ERROR   ");
    lcd_send_cmd(0xC0);
    lcd_print(err_msg ? err_msg : "TRY AGAIN");
}