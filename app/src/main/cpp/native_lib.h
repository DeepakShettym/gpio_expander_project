//
// Created by root on 23/12/24.
//

#ifndef MYAPPLICATION_NATIVE_LIB_H
#define MYAPPLICATION_NATIVE_LIB_H

#include <mutex>
#include <thread>
#include <atomic>

#include <jni.h>
#include <string>
#include <cstdio>
#include <iostream>
#include <stdio.h>
#include <android/log.h>
#include <pthread.h>
#include <unistd.h>

#include "leddriver.h"
#include "gpio_expander_tray.h"
#include "gpio_expander_led.h"
#include "buzzer_pwm_control.h"

using namespace std;


class NativeLib {

public:
//unique_lock<mutex> CircularBufferMutex_local(CircularBufferMutex);
//    void* nativeThreadFunc(void* args);
private:
};


#endif //MYAPPLICATION_NATIVE_LIB_H
