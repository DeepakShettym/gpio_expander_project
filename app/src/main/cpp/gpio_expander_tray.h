//
// Created by root on 20/11/24.
//

#ifndef MYAPPLICATION_GPIO_EXPANDER_TRAY_H
#define MYAPPLICATION_GPIO_EXPANDER_TRAY_H
#ifdef __cplusplus
extern "C"
{
#endif

    int TCA6416A_pin_init();
    int TCA6416A_port_read_input();
    int TCA6416A_port_write_output(int pin, int pin_state);
    int TCA6416A_pin_solenoid_dinit();

#ifdef __cplusplus
};
#endif

#endif //MYAPPLICATION_GPIO_EXPANDER_TRAY_H
