//
// Created by root on 20/11/24.
//



#include <stdint.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <error.h>
#include <string.h>
#include <unistd.h>

#include <android/log.h>
#include <stdbool.h>

#include "gpio_expander_tray.h"


#define HIGH    1
#define LOW     0

#define INPUT   0x0
#define OUTPUT  0x1

#define i2c_default 0
#define TCAREG_INPUT0       0x00
#define TCAREG_INPUT1       0x01
#define TCAREG_OUTPUT0      0x02
#define TCAREG_OUTPUT1      0x03
#define TCAREG_POLARITY0    0x04
#define TCAREG_POLARITY1    0x05
#define TCAREG_CONFIG0      0x06
#define TCAREG_CONFIG1      0x07

#define LED_DELAY_S         1000
uint16_t pinModesTraySolenoid;
uint16_t pinStateTraySolenoid;

#define DEVICE                 "/dev/i2c-5"

#define SUCCESS                 0
#define I2C_FAILURE            -1
#define INVALID_ARGUMENT       -2
#define DEVICE_NOT_FOUND       -3
#define DEVICE_NOT_INITIALIZED -4

int fd_i2c_expander_tray_solenoid = 0;


static int i2c_write_blocking(int *i2c, uint8_t addr, uint8_t *src, size_t len, bool nostop) {
    if (src == NULL || len == 0) {
        __android_log_print(ANDROID_LOG_ERROR, "I2C_I2C", "Invalid input parameters: src=%p, len=%zu", src, len);
        return -1;
    }

    if (fd_i2c_expander_tray_solenoid < 0) {
        __android_log_print(ANDROID_LOG_ERROR, "I2C_I2C", "Device not initialized, fd_i2c_expander_tray_solenoid: %d", fd_i2c_expander_tray_solenoid);
        return DEVICE_NOT_INITIALIZED;
    }

    int retVal = 0;
    const int retryCount = 5;

    struct i2c_msg msg[1];
    struct i2c_rdwr_ioctl_data msgdata;

    msg[0].addr = addr;
    msg[0].flags = 0;
    msg[0].len = len;
    msg[0].buf = src;

    msgdata.msgs = msg;
    msgdata.nmsgs = 1;

    for (int i = 0; i < retryCount; i++) {
        retVal = ioctl(fd_i2c_expander_tray_solenoid, I2C_RDWR, &msgdata);
        if (retVal >= 0) {
            break;
        }
        __android_log_print(ANDROID_LOG_WARN, "I2C_I2C", "Retry %d failed with errno %d", i + 1, retVal);
        usleep(1000);  // Adjust delay as necessary.
    }

    if (retVal < 0) {
        __android_log_print(ANDROID_LOG_ERROR, "I2C_I2C", "i2c_write_blocking failed, errno: %d", retVal);
    }

    return retVal;
}

static int i2c_write_blocking_port(int *i2c, uint8_t addr, uint8_t *src, size_t len, bool nostop) {
    if (src == NULL || len == 0) {
        __android_log_print(ANDROID_LOG_ERROR, "I2C_I2C", "Invalid input parameters: src=%p, len=%zu", src, len);
        return -1;
    }

    if (fd_i2c_expander_tray_solenoid < 0) {
        __android_log_print(ANDROID_LOG_ERROR, "I2C_I2C", "Device not initialized, fd_i2c_expander_tray_solenoid: %d", fd_i2c_expander_tray_solenoid);
        return DEVICE_NOT_INITIALIZED;
    }

    int retVal = 0;
    const int retryCount = 5;

    struct i2c_msg msg[1];
    struct i2c_rdwr_ioctl_data msgdata;

    msg[0].addr = addr;
    msg[0].flags = 0;
    msg[0].len = len;
    msg[0].buf = 0x00;

    msgdata.msgs = msg;
    msgdata.nmsgs = 1;

    for (int i = 0; i < retryCount; i++) {
        retVal = ioctl(fd_i2c_expander_tray_solenoid, I2C_RDWR, &msgdata);
        if (retVal >= 0) {
            break;
        }
        __android_log_print(ANDROID_LOG_WARN, "I2C_I2C", "Retry %d failed with errno %d", i + 1, retVal);
        usleep(1000);  // Adjust delay as necessary.
    }

    if (retVal < 0) {
        __android_log_print(ANDROID_LOG_ERROR, "I2C_I2C", "i2c_write_blocking failed, errno: %d", retVal);
    }

    return retVal;
}

static int i2c_read_blocking(int *i2c, uint8_t addr, uint8_t *dst, size_t len, bool nostop){

    if(fd_i2c_expander_tray_solenoid < 0) {
        __android_log_print(ANDROID_LOG_DEBUG, "I2C_I2C", "Device not initialized, fd_i2c_expander_tray_solenoid: %d", fd_i2c_expander_tray_solenoid);
        return DEVICE_NOT_INITIALIZED;
    }

    struct i2c_msg msg[2];
    struct i2c_rdwr_ioctl_data msgdata;
    int retryCount = 5;
    int retVal = -1;

    // Write address for reading
    msg[0].addr = addr;
    msg[0].flags = 0;
    msg[0].len = 0;
    msg[0].buf = NULL;

    // Read data
    msg[1].addr = addr;
    msg[1].flags = I2C_M_RD;
    msg[1].len = len;
    msg[1].buf = dst;

    msgdata.msgs = msg;
    msgdata.nmsgs = 2;

    for(int i = 0; i < retryCount; i++) {
        retVal = ioctl(fd_i2c_expander_tray_solenoid, I2C_RDWR, &msgdata);
        if (retVal >= 0) {
            break;
        }
        usleep(1000);
    }

    if(retVal < 0) {
        __android_log_print(ANDROID_LOG_DEBUG, "I2C_I2C", "i2c_read_blocking failed to read %d", retVal);
    }

    return retVal;
}


int i2cBusInitExpanderTraySolenoid(){

    unsigned char data;
    unsigned char regAddr;
    int retVal;

    fd_i2c_expander_tray_solenoid = open(DEVICE, O_RDWR);
    if (fd_i2c_expander_tray_solenoid < 0)
    {
        __android_log_print(ANDROID_LOG_DEBUG, "I2C_I2C", "BUS init error" );
        return DEVICE_NOT_FOUND;
    }

    return  SUCCESS;
}


int TCA6416A_mode_write_tray_solenoid(uint16_t modes) {
    uint8_t data[3];
    data[0] = TCAREG_CONFIG0;     // Configuration register address
    data[1] = modes & 0x00FF;     // Lower byte of modes
    data[2] = (modes >> 8) & 0x00FF; // Upper byte of modes

    int retVal = 0;
    // Send the configuration data to the TCA6416A
    __android_log_print(ANDROID_LOG_DEBUG, "I2C_I2C", " TCA6416A_mode_write_tray_solenoid" );
    i2c_write_blocking(i2c_default, 0x20, data, 3, false);
    if(retVal < 0) {
        printf("Failed to read input %d\n", retVal);
        __android_log_print(ANDROID_LOG_DEBUG, "I2C_I2C", " TCA6416A_mode_write_tray_solenoid Failed to read input %d", retVal );
        return -1;
    }
    // Update pinModesTraySolenoid with the new modes
    pinModesTraySolenoid = modes;
    return 0;
}

int  TCA6416A_pin_write_tray_solenoid(uint8_t pin_num, uint8_t level) {
    uint16_t mask = 1 << pin_num;
    if (level == HIGH) {
        pinStateTraySolenoid |= mask;
    } else {
        pinStateTraySolenoid &= ~mask;
    }
    //port_write(pin_state);
    return TCA6416A_mode_write_tray_solenoid(pinStateTraySolenoid);
}

int TCA6416A_pin_mode_tray_solenoid(uint8_t pinNum, int mode,  uint8_t i2caddr) {
    uint16_t mask = 1 << pinNum;

    if (mode == INPUT) {
        pinModesTraySolenoid |= mask; // Set the pin to INPUT
    } else {
        pinModesTraySolenoid &= ~mask; // Set the pin to OUTPUT
    }

    int retVal = TCA6416A_mode_write_tray_solenoid(pinModesTraySolenoid);
    return retVal;
}

int TCA6416A_port_read_tray_solenoid(uint8_t i2caddr, uint16_t pinModesTraySolenoid) {
    uint16_t tempInput = 0;
    uint8_t data[2];
    int retVal = 0;
    uint8_t regAddr = TCAREG_INPUT0;
    // Write the register address (TCAREG_INPUT0) to the TCA6416A
    __android_log_print(ANDROID_LOG_DEBUG, "I2C_I2C", " TCA6416A_port_read_tray_solenoid" );
    retVal = i2c_write_blocking(i2c_default, i2caddr, &regAddr, 1, true);
    if(retVal < 0) {
        printf("Failed to read input %d\n", retVal);
    }

    usleep(100);

    // Request the data from the device
    retVal = i2c_read_blocking(i2c_default, i2caddr, data, 2, false);
    if(retVal < 0) {
        printf("Failed to read input %d\n", retVal);
    }

    // Combine the two bytes into a 16-bit value
    tempInput = data[0] | (data[1] << 8);

    // Update pinStateTraySolenoid
    pinStateTraySolenoid = (pinStateTraySolenoid & ~pinModesTraySolenoid) | (tempInput & pinModesTraySolenoid);

    printf("%d \n", pinStateTraySolenoid);
    __android_log_print(ANDROID_LOG_DEBUG, "I2C_I2C", "Port Read %d \n", pinStateTraySolenoid );

    return (int)pinStateTraySolenoid;
}

int TCA6416A_port_read_input(){
    int i2cAddr = 0x20;
    return TCA6416A_port_read_tray_solenoid(i2cAddr, pinModesTraySolenoid);
}

int TCA6416A_pin_init() {

    int retVal = 0;
    int count = 0;

    retVal = i2cBusInitExpanderTraySolenoid();
    if(retVal == DEVICE_NOT_FOUND){
        return -1;
    }

    if(fd_i2c_expander_tray_solenoid == 0){
        return -2;
    } else {

        __android_log_print(ANDROID_LOG_DEBUG, "I2C_I2C", "Device initialized, fd_i2c_expander_tray_solenoid: %d", fd_i2c_expander_tray_solenoid);

        retVal  = TCA6416A_pin_mode_tray_solenoid(0, INPUT,0x20);
        if(retVal < 0) {
            return -3;
        }
        retVal  = TCA6416A_pin_mode_tray_solenoid(1, INPUT,0x20);
        if(retVal < 0){
            return -3;
        }
        retVal  = TCA6416A_pin_mode_tray_solenoid(2, INPUT,0x20);
        if(retVal < 0){
            return -3;
        }
        retVal  = TCA6416A_pin_mode_tray_solenoid(3, INPUT,0x20);
        if(retVal < 0){
            return -3;
        }

        retVal  = TCA6416A_pin_mode_tray_solenoid(10, OUTPUT,0x20);
        if(retVal < 0){
            return -3;
        }

        retVal  = TCA6416A_pin_mode_tray_solenoid(11, OUTPUT,0x20);
        if(retVal < 0){
            return -3;
        }

        retVal  = TCA6416A_pin_mode_tray_solenoid(12, OUTPUT,0x20);
        if(retVal < 0){
            return -3;
        }

        retVal  = TCA6416A_pin_mode_tray_solenoid(13, OUTPUT,0x20);
        if(retVal < 0){
            return -3;
        }


//        while(count < 10) {
//            //TCA6416A_pin_write_tray_solenoid(1,LOW);
//            sleep(LED_DELAY_S/1000);
//            TCA6416A_port_read_tray_solenoid(0x20, pinModesTraySolenoid);
//            //TCA6416A_pin_write_tray_solenoid(1,HIGH);
//            sleep(LED_DELAY_S/1000);
//            count++;
//        }
    }

    return 0;
}

int TCA6416A_port_write_output(int pin, int pin_state){
    int retVal = TCA6416A_pin_write_tray_solenoid(pin, pin_state);
    return retVal;
}

int TCA6416A_pin_solenoid_dinit(){

    TCA6416A_pin_write_tray_solenoid(10, LOW);
    TCA6416A_pin_write_tray_solenoid(11, LOW);
    TCA6416A_pin_write_tray_solenoid(12, LOW);
    TCA6416A_pin_write_tray_solenoid(13, LOW);

    fd_i2c_expander_tray_solenoid = -1;

    return 0;
}



