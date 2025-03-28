//
// Created by root on 16/01/25.
//

#ifndef MYAPPLICATION_BUZZER_PWM_CONTROL_H
#define MYAPPLICATION_BUZZER_PWM_CONTROL_H

#ifdef __cplusplus
extern "C"
{
#endif

int open_sysfs_file(const char* path);
void write_to_fd(int fd, const char* value);
void turn_on_pwm();
void turn_off_pwm();
int sys_pwm_init();
int sys_pwm_deinit();

#ifdef __cplusplus
};
#endif

#endif //MYAPPLICATION_BUZZER_PWM_CONTROL_H
