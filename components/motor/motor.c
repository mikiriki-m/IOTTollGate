#include "motor.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// 2-Phase Drive (Full-Step) Sequence for Maximum Torque
static const uint8_t step_sequence[4][4] = {
    {1, 1, 0, 0},
    {0, 1, 1, 0},
    {0, 0, 1, 1},
    {1, 0, 0, 1}
};

#define STEPS_FOR_90_DEG 512
#define STEP_DELAY_MS    10

static void motor_set_step(uint8_t step) {
    gpio_set_level(MOTOR_GPIO_IN1, step_sequence[step][0]);
    gpio_set_level(MOTOR_GPIO_IN2, step_sequence[step][1]);
    gpio_set_level(MOTOR_GPIO_IN3, step_sequence[step][2]);
    gpio_set_level(MOTOR_GPIO_IN4, step_sequence[step][3]);
}

static void motor_stop(void) {
    gpio_set_level(MOTOR_GPIO_IN1, 0);
    gpio_set_level(MOTOR_GPIO_IN2, 0);
    gpio_set_level(MOTOR_GPIO_IN3, 0);
    gpio_set_level(MOTOR_GPIO_IN4, 0);
}

void motor_init(void) {
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << MOTOR_GPIO_IN1) | (1ULL << MOTOR_GPIO_IN2) |
                        (1ULL << MOTOR_GPIO_IN3) | (1ULL << MOTOR_GPIO_IN4),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);
    motor_stop();
}

void open_gate(void) {
    for (int i = 0; i < STEPS_FOR_90_DEG; i++) {
        motor_set_step(i % 4);
        vTaskDelay(pdMS_TO_TICKS(STEP_DELAY_MS));
    }
    motor_stop();
}

void close_gate(void) {
    for (int i = 0; i < STEPS_FOR_90_DEG; i++) {
        // Reverse direction phase mapping (3, 2, 1, 0)
        motor_set_step(3 - (i % 4));
        vTaskDelay(pdMS_TO_TICKS(STEP_DELAY_MS));
    }
    motor_stop();
}