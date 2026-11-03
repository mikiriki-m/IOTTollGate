idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES ecran buzzer motor scanner
                    REQUIRES driver esp_driver_gpio esp_driver_ledc)