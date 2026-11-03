idf_component_register(SRCS "scanner.c"
                    INCLUDE_DIRS "include"
                    REQUIRES log driver rc522 esp_common esp_driver_gpio)
