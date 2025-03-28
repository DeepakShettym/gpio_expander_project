
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
#include <stdlib.h>

#define MAX_INTENSITY_LEVEL    0xa0
#define DEVICE                 "/dev/i2c-5"
#define LED_PATH "/sys/class/leds/BlackBox/brightness"


#define SUCCESS                 0
#define I2C_FAILURE            -1
#define INVALID_ARGUMENT       -2
#define DEVICE_NOT_FOUND       -3
#define DEVICE_NOT_INITIALIZED -4

unsigned char slaveAddr = 0x22;
unsigned char readOnlyRegs[6] = {0x00, 0x01, 0x11, 0x12, 0x13, 0x15};
int errno;
int fd = 0;

static int i2cWrite(unsigned char slaveAddr, unsigned char regAddr, unsigned char len, unsigned char* data )
{
    struct i2c_msg msg[2];
    struct i2c_rdwr_ioctl_data msgdata;
    unsigned char wrData[10];
    int retryCount = 15;
    int retVal;

    if(fd < 0)
    {
        return DEVICE_NOT_INITIALIZED;
    }

    wrData[0] = regAddr;

    for(int i=0; i<len; i++)
    {
        wrData[i+1] = data[i];
    }

    msg[0].addr = slaveAddr;
    msg[0].flags = 0;
    msg[0].len = len+1;
    msg[0].buf = wrData;

    msgdata.msgs = msg;
    msgdata.nmsgs = 1;

    for(int i=0; i< retryCount; i++)
    {
        retVal = ioctl(fd, I2C_RDWR, &msgdata) ;
        if (retVal >= 0)
        {
            break;
        }
        usleep(10);
    }
    return retVal;
}

static int i2cRead(unsigned char slaveAddr, unsigned char regAddr, unsigned char len, unsigned char* data )
{

    struct i2c_msg msg[4];
    struct i2c_rdwr_ioctl_data msgdata;
    int retryCount = 15;

    if(fd < 0)
    {
        return DEVICE_NOT_INITIALIZED;
    }

    msg[0].addr = slaveAddr;
    msg[0].flags = 0;
    msg[0].len = 1;
    msg[0].buf = &regAddr;

    msgdata.msgs = msg;
    msgdata.nmsgs = 1;
    for(int i=0; i<retryCount; i++)
    {
        int retVal = ioctl(fd, I2C_RDWR, &msgdata) ;
        if (retVal >= 0)
        {
            break;
        }
        usleep(10);
    }

    msg[0].addr = slaveAddr;
    msg[0].flags = I2C_M_RD;
    msg[0].len = len;
    msg[0].buf = data;

    msgdata.msgs = msg;
    msgdata.nmsgs = 1;

    for(int i=0; i<retryCount; i++)
    {
        int retVal = ioctl(fd, I2C_RDWR, &msgdata) ;
        if (retVal >= 0)
        {
            break;
        }
        usleep(10);
    }

    return 0;
}

void set_led_brightness(int value) {
    int fd_led = open(LED_PATH, O_WRONLY);
    if (fd_led < 0) {
        __android_log_print(ANDROID_LOG_DEBUG, "LED GPIO Enable","Failed to open LED brightness file");
        return;
    }

    char buffer[2];
    snprintf(buffer, sizeof(buffer), "%d", value);

    if (write(fd_led, buffer, sizeof(buffer)) < 0) {
        __android_log_print(ANDROID_LOG_DEBUG, "LED GPIO Enable","Failed to write to LED brightness file");
    }

    close(fd_led);
}

int led_set_level(int reset_gpio_level) {

    set_led_brightness(reset_gpio_level);
    __android_log_print(ANDROID_LOG_DEBUG, "LED GPIO Enable","LED brightness set to %d\n", reset_gpio_level);
    return EXIT_SUCCESS;
}


int ledDriver_Init()
{
    unsigned char data;
    unsigned char regAddr;
    int retVal;

    led_set_level(0);
    usleep(50000);
    led_set_level(1);

    fd = open(DEVICE, O_RDWR);
    if (fd < 0)
    {
        return DEVICE_NOT_FOUND;
    }

    //Read devId
    regAddr = 0x00;
    retVal = i2cRead(slaveAddr, regAddr, 1, &data);
    if (retVal < 0)
    {
        printf("\n Failed to read device ID\n");
    }
    else
    {
        printf("\n i2cRead Success: device id : 0x%x",data);
    }

    //Read RevId
    regAddr = 0x01;
    retVal = i2cRead(slaveAddr, regAddr, 1, &data);
    if (retVal < 0)
    {
        printf("\n Failed to read revision ID\n");
        return I2C_FAILURE;
    }
    else
    {
        printf("\n i2cRead Success: revision id : 0x%x",data);
    }

    //Disabling external dim through dim pin
    regAddr = 0x03;
    data = 0x00;
    retVal = i2cWrite(slaveAddr, regAddr, 1, &data);
    if (retVal < 0)
    {
        printf("\n Failed to disable external dim\n");
        return I2C_FAILURE;
    }

    //Set the short threshold voltage
    regAddr = 0x0E;
    data = 0b00010000;

    retVal = i2cWrite(slaveAddr, regAddr,1,&data);
    if (retVal < 0)
    {
        printf("\n LED_CONTROL Failed to set led short ckt threshold voltage\n");
        __android_log_print(ANDROID_LOG_DEBUG, "LED_CONTROL", "i2cRead reg: ,Failed to set led short ckt threshold voltage: ");
        return I2C_FAILURE;
    }

    //Enable fast-softstart
    regAddr = 0x02;
    data = 0x04;
    retVal = i2cWrite(slaveAddr, regAddr, 1, &data);
    if (retVal < 0)
    {
        printf("\n Failed to enable fast-softstart\n");
        return I2C_FAILURE;
    }
    return  SUCCESS;
}

int ledDriver_EnableChannel(unsigned char channelNo, unsigned char enable)
{
    unsigned char regAddr = 0x0f;
    unsigned char data;
    int retVal;

//    //Enable fast-softstart
//    unsigned char regAddr2 = 0x02;
//    unsigned char data2 = 0x04;
//    int retVal2 = i2cWrite(slaveAddr, regAddr2, 1, &data2);
//    if (retVal2 < 0)
//    {
//        printf("\n Failed to enable fast-softstart\n");
//        return I2C_FAILURE;
//    }

    retVal = i2cRead(slaveAddr, regAddr, 1, &data);
    if (retVal < 0)
    {
        return I2C_FAILURE;
    }

    if(enable)
    {
        for(int i = 0; i< channelNo-1; i++)
        {
            if(data & (1<<(i)))
            {
                return INVALID_ARGUMENT;
            }
        }

        data &= ~(1<<(channelNo-1));
    }
    else
    {

        for(int i = 4; i> channelNo; i--)
        {
            if(!(data & (1<<(i-1))))
            {
                return INVALID_ARGUMENT;
            }
        }
        data |= (1<<(channelNo-1));

    }

    printf("\n ledDriver_EnableChannel :: 0x%x",data);

    retVal = i2cWrite(slaveAddr, regAddr, 1, &data);
    if (retVal < 0)
    {
        return I2C_FAILURE;
    }
    return  SUCCESS;
}

int ledDriver_GetChannelStatus(unsigned char channelNo)
{
    unsigned char regAddr = 0x0f;
    unsigned char data;
    int retVal;

    retVal = i2cRead(slaveAddr, regAddr, 1, &data);
    if (retVal < 0)
    {
        return I2C_FAILURE;
    }
    else
    {
        if( data & (1 << (channelNo-1) ) )
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }
    return  SUCCESS;
}

int ledDriver_Output(unsigned char enable)
{
    unsigned char regAddr = 0x04;
    unsigned char data;
    int retVal;

    //changing the default brightness value from 0xff to 0x11
    data = 0x11;
    retVal = i2cWrite(slaveAddr, regAddr, 1, &data);
    if(retVal<0)
    {
        return I2C_FAILURE;
    }

    regAddr = 0x05;
    unsigned char tdata[9] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    retVal = i2cWrite(slaveAddr, regAddr, 9, (uint8_t*)&tdata);
    if(retVal < 0)
    {
        return I2C_FAILURE;
    }

    // Enabling o/p LEDs and Boost converter and fast soft start
    regAddr = 0x02;
    if (enable == 1)
    {
        data = 0x06;
    }
    else if (enable == 0)
    {
        data =0x00;
    }
    else
    {
        return INVALID_ARGUMENT;
    }

    retVal = i2cWrite(slaveAddr, regAddr, 1, &data);
    if(retVal < 0)
    {
        return I2C_FAILURE;
    }
    return  SUCCESS;

}

int ledDriver_GetOutputStatus()
{
    unsigned char regAddr = 0x02;
    unsigned char data;
    int retVal;

    retVal = i2cRead(slaveAddr, regAddr, 1, &data);
    if (retVal < 0)
    {
        return I2C_FAILURE;
    }
    else
    {
        retVal = i2cRead(slaveAddr, regAddr, 1, &data);
        if (retVal < 0)
        {
            return I2C_FAILURE;
        }
        else
        {
            return ((data >> 1) & 0x01);
        }
    }
}

int ledDriver_SetIntensity(unsigned char intensity)
{
    unsigned char regAddr = 0x04;
    int retVal;

    if(intensity <= MAX_INTENSITY_LEVEL)
    {
        regAddr = 0x05;
        unsigned char data[9] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        retVal = i2cWrite(slaveAddr, regAddr, 9, (uint8_t*)&data);
        if(retVal < 0)
        {
            return I2C_FAILURE;
        }

        regAddr = 0x04;
        retVal = i2cWrite(slaveAddr, regAddr, 1, &intensity);
        if(retVal<0)
        {
            return I2C_FAILURE;
        }

    }
    else
    {
        return INVALID_ARGUMENT;
    }
    return  SUCCESS;
}

int ledDriver_GetIntensity()
{
    unsigned char regAddr = 0x04;
    unsigned char data;
    int retVal;

    //Reading ADIM reg
    retVal = i2cRead(slaveAddr, regAddr, 1, &data);
    if (retVal < 0)
    {
        return I2C_FAILURE;
    }
    else
    {
        return data;
    }

}

int ledDriver_SetChannelTonTime(unsigned char channelNo, int tOnTime)
{
    unsigned char regAddr = 0x05 + ((channelNo-1)*2);
    uint8_t data[2];
    int retVal = 0;

    data[1] = (tOnTime) & (0xff);
    data[0] = (tOnTime>>8) & (0xff);


    retVal = i2cWrite(slaveAddr, regAddr, 2, (uint8_t*)&data);
    if(retVal < 0)
    {
        return I2C_FAILURE;
    }

    regAddr = 0x0d;
    retVal = i2cRead(slaveAddr, regAddr, 1, (uint8_t*)&data);
    if(retVal < 0)
    {
        return I2C_FAILURE;
    }

    regAddr = 0x0d;
    data[0] = 00;
    retVal = i2cWrite(slaveAddr, regAddr, 1, (uint8_t*)&data[0]);
    if(retVal < 0)
    {
        return I2C_FAILURE;
    }

    return  SUCCESS;
}

int ledDriver_GetChannelTonTime(unsigned char channelNo)
{
    unsigned char regAddr = 0x05 + ((channelNo-1)*2);
    uint8_t data[3];
    int retVal = 0;
    int tOnTime = 0;

    retVal = i2cRead(slaveAddr, regAddr, 2, (uint8_t*)&data);
    if(retVal < 0)
    {

        return I2C_FAILURE;
    }

    regAddr = 0x0d;
    retVal = i2cRead(slaveAddr, regAddr, 1, (uint8_t*)&data[2]);
    if(retVal < 0)
    {

        return I2C_FAILURE;
    }

    tOnTime = (data[1] )| (data[0]<<8) ;
    return tOnTime;
}

// SET Register Values
int ledDriver_SetRegVal(unsigned char regAddr, unsigned char value)
{
    int retVal;

    for(int i=0; i<sizeof(readOnlyRegs); i++)
    {
        if(regAddr == readOnlyRegs[i])
        {
            return INVALID_ARGUMENT;
        }
    }

    if(regAddr > 0x15)
    {
        return INVALID_ARGUMENT;
    }

    retVal = i2cWrite(slaveAddr, regAddr, 1, &value);
    if (retVal < 0)
    {
        return I2C_FAILURE;
    }
    else
    {
        return SUCCESS;
    }
}

//GET Register Values
int ledDriver_GetRegVal(unsigned char regAddr)
{
    unsigned char value = 0;
    int retVal;

    if(regAddr > 0x15)
    {
        return INVALID_ARGUMENT;
    }

    retVal = i2cRead(slaveAddr, regAddr, 1, &value);
    if (retVal < 0)
    {
        return I2C_FAILURE;
    }
    else
    {
        __android_log_print(ANDROID_LOG_DEBUG, "LED_CONTROL", "i2cRead reg: %c ,value: %d", regAddr,value);
        return value;
    }
}



int ledDriver_DeInit()
{
    led_set_level(0);

    if(fd < 0)
    {
        return DEVICE_NOT_INITIALIZED;
    }
    else
    {
        fd = -1;
        return SUCCESS;
    }
}