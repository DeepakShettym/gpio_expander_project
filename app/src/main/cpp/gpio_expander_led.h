//
// Created by root on 26/12/24.
//

#ifndef MYAPPLICATION_GPIO_EXPANDER_LED_H
#define MYAPPLICATION_GPIO_EXPANDER_LED_H

#ifdef __cplusplus
extern "C"
{
#endif

    int TCA6416A_pin_led_init();
    int TCA6416A_port_set_output_led(int led_color_port0, int led_color_port1);
    int TCA6416A_pin_led_dinit();

#ifdef __cplusplus
};
#endif

#endif //MYAPPLICATION_GPIO_EXPANDER_LED_H
