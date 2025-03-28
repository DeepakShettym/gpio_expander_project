//
// Created by root on 16/01/25.
//

#include <stdint.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <android/log.h>
#include "buzzer_pwm_control.h"
#include <error.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>

int export_fd = -1;
int period_fd = -1;
int duty_cycle_fd = -1;
int enable_fd = -1;

int open_sysfs_file(const char* path) {
    int fd = open(path, O_WRONLY);
    if (fd < 0) {
        __android_log_print(ANDROID_LOG_ERROR, "PWM_CONTROL", "Failed to open sysfs file: %s, Error: %s", path, strerror(errno));
    } else {
        __android_log_print(ANDROID_LOG_DEBUG, "PWM_CONTROL", "Successfully opened sysfs file: %s", path);
    }
    return fd;
}


void write_to_fd(int fd, const char* value) {
    if (fd < 0) {
        __android_log_print(ANDROID_LOG_ERROR, "PWM_CONTROL", "Invalid file descriptor");
        return;
    }

    ssize_t bytes_to_write = strlen(value);
    ssize_t bytes_written = write(fd, value, bytes_to_write);

    if (bytes_written < 0) {
        __android_log_print(ANDROID_LOG_ERROR, "PWM_CONTROL", "Failed to write value '%s'", value);
    } else if (bytes_written != bytes_to_write) {
        __android_log_print(ANDROID_LOG_ERROR, "PWM_CONTROL", "Partial write: only %zd of %zd bytes written for value '%s'", bytes_written, bytes_to_write, value);
    } else {
        __android_log_print(ANDROID_LOG_DEBUG, "PWM_CONTROL", "Successfully wrote value '%s'", value);
    }
}


void turn_on_pwm() {
    write_to_fd(export_fd, "2");               // Export the PWM channel
    write_to_fd(period_fd, "250000");          // Set the period
    write_to_fd(duty_cycle_fd, "125000");      // Set the duty cycle
    write_to_fd(enable_fd, "1");
    __android_log_print(ANDROID_LOG_DEBUG,"PWM CONTROL PWM turned ON.  " ," %s, %d",__FUNCTION__, __LINE__ );

}

void turn_off_pwm() {
    write_to_fd(enable_fd, "0");               // Disable the PWM
    __android_log_print(ANDROID_LOG_DEBUG,"PWM CONTROL PWM turned OFF.  " ," %s, %d",__FUNCTION__, __LINE__ );
}

int sys_pwm_init() {
    export_fd       = open_sysfs_file("/sys/class/pwm/pwmchip0/device/pwm/pwmchip0/export");
    period_fd       = open_sysfs_file("/sys/class/pwm/pwmchip0/device/pwm/pwmchip0/pwm2/period");
    duty_cycle_fd   = open_sysfs_file("/sys/class/pwm/pwmchip0/device/pwm/pwmchip0/pwm2/duty_cycle");
    enable_fd       = open_sysfs_file("/sys/class/pwm/pwmchip0/device/pwm/pwmchip0/pwm2/enable");

    /*t527-demo-car:# echo 2 > /sys/class/pwm/pwmchip0/device/pwm/pwmchip0/export
    t527-demo-car:# echo 250000 > /sys/class/pwm/pwmchip0/device/pwm/pwmchip0/pwm2/period
    t527-demo-car:# echo 125000 > /sys/class/pwm/pwmchip0/device/pwm/pwmchip0/pwm2/duty_cycle
    t527-demo-car:# echo 1 > /sys/class/pwm/pwmchip0/device/pwm/pwmchip0/pwm2/enable
    t527-demo-car:# echo 0 > /sys/class/pwm/pwmchip0/device/pwm/pwmchip0/pwm2/enable*/

    __android_log_print(ANDROID_LOG_DEBUG,"PWM CONTROL Performing some operations while PWM is ON....  " ," %s, %d",__FUNCTION__, __LINE__ );

    return 0;
}

int sys_pwm_deinit(){

    close(export_fd);
    close(period_fd);
    close(duty_cycle_fd);
    close(enable_fd);

    return 0;
}
