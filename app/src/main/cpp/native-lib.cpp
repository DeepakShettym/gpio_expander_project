
#include "native_lib.h"

bool isThreadRunning = false;
bool shouldThreadRun = false;

//unique_lock<mutex> CircularBufferMutex_local(CircularBufferMutex);
static mutex ContactSwitchMutex;
bool trayDetectThreadShouldRun = false;
bool isTrayDetectThreadRunning = false;

extern "C" JNIEXPORT void JNICALL Java_com_example_MyClass_callJavaFunction(JNIEnv* env, jobject obj);

// Java Virtual Machine pointer
JavaVM* jvm;

// Java method ID
jmethodID javaMethod;

// Java class ID
jclass javaClass;

// Native thread function
void* nativeThreadFunc(void* args) {
    JNIEnv* env;

    // Attach the native thread to JVM
    jvm->AttachCurrentThread(reinterpret_cast<JNIEnv **>((void **) &env), NULL);

    // Get the Java class and method IDs (if not already obtained)
    if (javaClass == NULL) {
        javaClass = reinterpret_cast<jclass>(env->NewGlobalRef(env->FindClass("com/example/MyClass")));
        javaMethod = env->GetStaticMethodID(javaClass, "javaFunction", "()V");
    }

    isTrayDetectThreadRunning = true;
    trayDetectThreadShouldRun = true;
    unique_lock<mutex> ContactSwitchMutex_local(ContactSwitchMutex, std::defer_lock);


    while(trayDetectThreadShouldRun){
        // Call the Java method
        ContactSwitchMutex_local.lock();
        //if(ContactSwitchMutex_local.owns_lock()){
            uint16_t contactSwitchState = TCA6416A_port_read_input();
            __android_log_print(ANDROID_LOG_INFO, "INPUT_SWITCH", "%d", contactSwitchState);
            ContactSwitchMutex_local.unlock();
            env->CallStaticVoidMethod(javaClass, javaMethod);
//        } else {
//            __android_log_print(ANDROID_LOG_INFO, "INPUT SWITCH", "Lock Missed");
//            std::this_thread::yield();
//        }

        sleep(1);
        //count++;
    }

    // Detach the native thread from JVM
    jvm->DetachCurrentThread();

    isTrayDetectThreadRunning = false;
    trayDetectThreadShouldRun = false;

    return NULL;
}

// Create and start the native thread
void startNativeThread() {
    if(!isTrayDetectThreadRunning){
        pthread_t thread;
        pthread_create(&thread, NULL, nativeThreadFunc, NULL);
    }
}

// Initialize JVM and Java method IDs
void initJVM(JavaVM* vm) {
    jvm = vm;

    // Get the JNIEnv pointer
    JNIEnv* env;
    jvm->GetEnv((void**)&env, JNI_VERSION_1_6);

    // Get the Java class and method IDs
    javaClass = reinterpret_cast<jclass>(env->NewGlobalRef(env->FindClass("com/example/MyClass")));
    javaMethod = env->GetStaticMethodID(javaClass, "javaFunction", "()V");

    startNativeThread();
}


static int hello(JNIEnv *env, jobject obj){
    __android_log_print(ANDROID_LOG_INFO, "MyTag", "The value is hello3");
    Java_com_example_MyClass_callJavaFunction(env, obj);
    return 0;
}

int init_system(){

    int retVal = 0;

    unique_lock<mutex> ContactSwitchMutex_initSystem_local(ContactSwitchMutex, std::defer_lock);

    ContactSwitchMutex_initSystem_local.lock();

    retVal = ledDriver_Init();
    if(retVal != 0){
        //return -1;
        __android_log_print(ANDROID_LOG_INFO, "ledDriver_Init", "::%d", retVal);
    }

    retVal = TCA6416A_pin_init();
    if(retVal != 0){
        //return -2;
        __android_log_print(ANDROID_LOG_INFO, "TCA6416A_pin_init", "::%d", retVal);
    }

    retVal = TCA6416A_pin_led_init();
    if(retVal != 0){
        //return -3;
        __android_log_print(ANDROID_LOG_INFO, "TCA6416A_pin_led_init", "::%d", retVal);
    }

    ContactSwitchMutex_initSystem_local.unlock();

    //startNativeThread();

    retVal = sys_pwm_init();
    if(retVal != 0){
        __android_log_print(ANDROID_LOG_INFO, "sys_pwm_init", "::%d", retVal);
    }

    sleep(1);

    return retVal;
}

int system_UnInit(){

    int retVal = 0;

    isTrayDetectThreadRunning = false;
    trayDetectThreadShouldRun = false;

    unique_lock<mutex> ContactSwitchMutex_UnInitSystem_local(ContactSwitchMutex, std::defer_lock);

    ContactSwitchMutex_UnInitSystem_local.lock();

    retVal = ledDriver_DeInit();
    if(retVal != 0){
        //return -1;
        __android_log_print(ANDROID_LOG_INFO, "ledDriver_DeInit", "::%d", retVal);
    }

    retVal = TCA6416A_pin_solenoid_dinit();
    if(retVal != 0){
        //return -2;
        __android_log_print(ANDROID_LOG_INFO, "TCA6416A_pin_solenoid_dinit", "::%d", retVal);
    }

    retVal = TCA6416A_pin_led_dinit();
    if(retVal != 0){
        //return -3;
        __android_log_print(ANDROID_LOG_INFO, "TCA6416A_pin_led_dinit", "::%d", retVal);
    }
    ContactSwitchMutex_UnInitSystem_local.unlock();

    retVal = sys_pwm_deinit();
    if(retVal != 0){
        __android_log_print(ANDROID_LOG_INFO, "sys_pwm_deinit", "::%d", retVal);
    }

    return 0;
}

int TCA6416A_port_set_output_driver_led(int led_port_0, int led_port_1){

    unique_lock<mutex> ContactSwitchMutex_driverLed_local(ContactSwitchMutex, std::defer_lock);

    ContactSwitchMutex_driverLed_local.lock();
    int retVal = TCA6416A_port_set_output_led(led_port_0, led_port_1);
    ContactSwitchMutex_driverLed_local.unlock();

    return retVal;
}

int TCA6416A_port_write_output_solenoid(int pin, int pin_state){

    unique_lock<mutex> ContactSwitchMutex_solenoidState_local(ContactSwitchMutex, std::defer_lock);

    ContactSwitchMutex_solenoidState_local.lock();
    int retVal = TCA6416A_port_write_output(pin, pin_state);
    ContactSwitchMutex_solenoidState_local.unlock();

    return retVal;
}

int ledDriver_SetRegVal_(unsigned char regAddr, unsigned char value){
    unique_lock<mutex> ContactSwitchMutex_ledDriver_SetRegVal_local(ContactSwitchMutex, std::defer_lock);

    ContactSwitchMutex_ledDriver_SetRegVal_local.lock();
    return ledDriver_SetRegVal(regAddr, value);
    ContactSwitchMutex_ledDriver_SetRegVal_local.unlock();

}

int ledDriver_GetRegVal_(unsigned char uchar_value){
    unique_lock<mutex> ContactSwitchMutex_ledDriver_GetRegVal_local(ContactSwitchMutex, std::defer_lock);

    ContactSwitchMutex_ledDriver_GetRegVal_local.lock();
    return ledDriver_GetRegVal(uchar_value);
    ContactSwitchMutex_ledDriver_GetRegVal_local.unlock();
}

int ledDriver_GetChannelStatus_(unsigned char uchar_value){
    unique_lock<mutex> ContactSwitchMutex_ledDriver_GetChannelStatus_local(ContactSwitchMutex, std::defer_lock);

    ContactSwitchMutex_ledDriver_GetChannelStatus_local.lock();
    return ledDriver_GetChannelStatus(uchar_value);
    ContactSwitchMutex_ledDriver_GetChannelStatus_local.unlock();
}

int ledDriver_EnableChannel_(unsigned char channelNo, unsigned char enable){
    unique_lock<mutex> ContactSwitchMutex_ledDriver_EnableChannel_local(ContactSwitchMutex, std::defer_lock);

    ContactSwitchMutex_ledDriver_EnableChannel_local.lock();
    return ledDriver_EnableChannel(channelNo, enable);
    ContactSwitchMutex_ledDriver_EnableChannel_local.unlock();
}

int ledDriver_GetChannelTonTime_(unsigned char channelNo){
    unique_lock<mutex> ContactSwitchMutex_ledDriver_GetChannelTonTime_local(ContactSwitchMutex, std::defer_lock);

    ContactSwitchMutex_ledDriver_GetChannelTonTime_local.lock();
    return ledDriver_GetChannelTonTime(channelNo);
    ContactSwitchMutex_ledDriver_GetChannelTonTime_local.unlock();
}

int ledDriver_SetChannelTonTime_(unsigned char channelNo, int tOnTime){
    unique_lock<mutex> ContactSwitchMutex_ledDriver_SetChannelTonTime_local(ContactSwitchMutex, std::defer_lock);

    ContactSwitchMutex_ledDriver_SetChannelTonTime_local.lock();
    return ledDriver_SetChannelTonTime(channelNo, tOnTime);
    ContactSwitchMutex_ledDriver_SetChannelTonTime_local.unlock();
}

int ledDriver_DeInit_(){
    unique_lock<mutex> ContactSwitchMutex_ledDriver_DeInit_local(ContactSwitchMutex, std::defer_lock);

    ContactSwitchMutex_ledDriver_DeInit_local.lock();
    return ledDriver_DeInit();
    ContactSwitchMutex_ledDriver_DeInit_local.unlock();
}

int ledDriver_GetIntensity_(){
    unique_lock<mutex> ContactSwitchMutex_ledDriver_GetIntensity_local(ContactSwitchMutex, std::defer_lock);

    ContactSwitchMutex_ledDriver_GetIntensity_local.lock();
    return ledDriver_GetIntensity();
    ContactSwitchMutex_ledDriver_GetIntensity_local.unlock();
}

int ledDriver_SetIntensity_(unsigned char intensity){
    unique_lock<mutex> ContactSwitchMutex_ledDriver_SetIntensity_local(ContactSwitchMutex, std::defer_lock);

    ContactSwitchMutex_ledDriver_SetIntensity_local.lock();
    return ledDriver_SetIntensity(intensity);
    ContactSwitchMutex_ledDriver_SetIntensity_local.unlock();
}
int ledDriver_GetOutputStatus_(){
    unique_lock<mutex> ContactSwitchMutex_ledDriver_GetOutputStatus_local(ContactSwitchMutex, std::defer_lock);

    ContactSwitchMutex_ledDriver_GetOutputStatus_local.lock();
    return ledDriver_GetOutputStatus();
    ContactSwitchMutex_ledDriver_GetOutputStatus_local.unlock();
}

int ledDriver_Output_(unsigned char enable){
    unique_lock<mutex> ContactSwitchMutex_ledDriver_Output_local(ContactSwitchMutex, std::defer_lock);

    ContactSwitchMutex_ledDriver_Output_local.lock();
    return ledDriver_Output(enable);
    ContactSwitchMutex_ledDriver_Output_local.unlock();
}

int turn_on_pwm_sys(){
    turn_on_pwm();
    return 0;
}

int turn_off_pwm_sys(){
    turn_off_pwm();
    return 0;
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_MainActivity_stringFromJNI(JNIEnv* env,jobject /* this */) {
    std::string hello = "Hello --from C++";

    __android_log_print(ANDROID_LOG_INFO, "MyTag", "The value is hello1");

    return env->NewStringUTF(hello.c_str());
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_MyClass_callJavaFunction(JNIEnv* env, jobject obj) {
    // Find the Java class
    jclass clazz = env->FindClass("com/example/MyClass");

    // Find the method ID of the Java function
    jmethodID methodID = env->GetStaticMethodID(clazz, "javaFunction", "()V");

    __android_log_print(ANDROID_LOG_INFO, "MyTag", "The value is hello4");

    // Call the Java function
    env->CallStaticVoidMethod(clazz, methodID);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_MainActivity_callJavaFunction(JNIEnv *env, jobject thiz) {
    hello(reinterpret_cast<JNIEnv *>(env), thiz);
    __android_log_print(ANDROID_LOG_INFO, "MyTag", "The value is hello2");

}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_MainActivity_initJVM(JNIEnv *env, jobject thiz) {
    int ans = TCA6416A_pin_init();
    __android_log_print(ANDROID_LOG_DEBUG, "LED_CONTROL", "deepak called this function");
    JavaVM* vm;
    env->GetJavaVM(&vm);
    // Initialize JVM
    initJVM(vm);
}


extern "C"
JNIEXPORT jint JNICALL
Java_com_example_MainActivity_ledDriver_1Init(JNIEnv *env, jobject thiz) {
    jint result =  ledDriver_Init();
    __android_log_print(ANDROID_LOG_DEBUG, "LED_CONTROL", "ledDriver_Init: %d", result);
    return result;
}
extern "C"
JNIEXPORT jint JNICALL
Java_com_example_MainActivity_ledDriver_1Output(JNIEnv *env, jobject thiz, jbyte enable) {
    unsigned char uchar_value = static_cast<unsigned char>(enable & 0xFF);
    __android_log_print(ANDROID_LOG_DEBUG, "LED_CONTROL", "ledDriver_Output set value: %d", uchar_value);
    jint result = ledDriver_Output_(uchar_value);
    __android_log_print(ANDROID_LOG_DEBUG, "LED_CONTROL", "ledDriver_Output: %d", result);
    return result;
}
extern "C"
JNIEXPORT jint JNICALL
Java_com_example_MainActivity_ledDriver_1GetOutputStatus(JNIEnv *env, jobject thiz) {
    jint result = ledDriver_GetOutputStatus_();
    __android_log_print(ANDROID_LOG_DEBUG, "LED_CONTROL", "ledDriver_GetOutputStatus: %d", result);
    return result;
}
extern "C"
JNIEXPORT jint JNICALL
Java_com_example_MainActivity_ledDriver_1SetIntensity(JNIEnv *env, jobject thiz, jbyte intensity) {
    unsigned char uchar_value = static_cast<unsigned char>(intensity & 0xFF);
    __android_log_print(ANDROID_LOG_DEBUG, "LED_CONTROL", "ledDriver_SetIntensity set value: %d", uchar_value);
    jint result = ledDriver_SetIntensity_(uchar_value);
    __android_log_print(ANDROID_LOG_DEBUG, "LED_CONTROL", "ledDriver_SetIntensity: %d", result);
    return result;
}
extern "C"
JNIEXPORT jint JNICALL
Java_com_example_MainActivity_ledDriver_1GetIntensity(JNIEnv *env, jobject thiz) {
    jint result = ledDriver_GetIntensity_();
    __android_log_print(ANDROID_LOG_DEBUG, "LED_CONTROL", "ledDriver_GetIntensity: %d", result);
    return result;
}
extern "C"
JNIEXPORT jint JNICALL
Java_com_example_MainActivity_ledDriver_1DeInit(JNIEnv *env, jobject thiz) {
    jint result = ledDriver_DeInit_();
    __android_log_print(ANDROID_LOG_DEBUG, "LED_CONTROL", "ledDriver_DeInit: %d", result);
    return result;
}
extern "C"
JNIEXPORT jint JNICALL
Java_com_example_MainActivity_ledDriver_1SetChannelTonTime(JNIEnv *env, jobject thiz,
                                                           jbyte channel_no, jint t_on_time) {
    unsigned char uchar_value = static_cast<unsigned char>(channel_no & 0xFF);
    __android_log_print(ANDROID_LOG_DEBUG, "LED_CONTROL", "ledDriver_SetChannelTonTime set Intensity: %d:%d", uchar_value,t_on_time);
    jint result = ledDriver_SetChannelTonTime_(uchar_value,t_on_time);
    __android_log_print(ANDROID_LOG_DEBUG, "LED_CONTROL", "ledDriver_SetChannelTonTime: %d", result);
    return result;
}
extern "C"
JNIEXPORT jint JNICALL
Java_com_example_MainActivity_ledDriver_1GetChannelTonTime(JNIEnv *env, jobject thiz,
                                                           jbyte channel_no) {
    unsigned char uchar_value = static_cast<unsigned char>(channel_no & 0xFF);
    __android_log_print(ANDROID_LOG_DEBUG, "LED_CONTROL", "ledDriver_GetChannelTonTime set value: %d", uchar_value);
    jint result = ledDriver_GetChannelTonTime_(uchar_value);
    __android_log_print(ANDROID_LOG_DEBUG, "LED_CONTROL", "ledDriver_GetChannelTonTime: %d", result);
    return result;
}
extern "C"
JNIEXPORT jint JNICALL
Java_com_example_MainActivity_ledDriver_1EnableChannel(JNIEnv *env, jobject thiz, jbyte channel_no,
                                                       jbyte channel_enable) {
    unsigned char uchar_value = static_cast<unsigned char>(channel_no & 0xFF);
    unsigned char channel_status = static_cast<unsigned char>(channel_enable & 0xFF);
    __android_log_print(ANDROID_LOG_DEBUG, "LED_CONTROL", "ledDriver_EnableChannel set value: channel_no: %d, channel_status: %d", uchar_value,channel_status);
    jint result = ledDriver_EnableChannel_(uchar_value,channel_status);
    __android_log_print(ANDROID_LOG_DEBUG, "LED_CONTROL", "ledDriver_EnableChannel: %d", result);
    return result;
}
extern "C"
JNIEXPORT jint JNICALL
Java_com_example_MainActivity_ledDriver_1GetChannelStatus(JNIEnv *env, jobject thiz,
                                                          jbyte channel_no) {
    unsigned char uchar_value = static_cast<unsigned char>(channel_no & 0xFF);
    __android_log_print(ANDROID_LOG_DEBUG, "LED_CONTROL", "ledDriver_GetChannelStatus set value: %d", uchar_value);
    jint result = ledDriver_GetChannelStatus_(uchar_value);
    __android_log_print(ANDROID_LOG_DEBUG, "LED_CONTROL", "ledDriver_GetChannelStatus: %d", result);
    return result;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_example_MainActivity_getLedRegisterStatusValue(JNIEnv *env, jobject thiz,
                                                        jbyte register_address) {
    unsigned char uchar_value = static_cast<unsigned char>(register_address & 0xff);
    __android_log_print(ANDROID_LOG_DEBUG, "LED_CONTROL", "getLedRegisterStatusValue  uchar_value: %x", uchar_value);
    jint result = ledDriver_GetRegVal_(uchar_value);
    __android_log_print(ANDROID_LOG_DEBUG, "LED_CONTROL", "getLedRegisterStatusValue: %d", result);
    return result;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_example_MainActivity_setLedRegisterValue(JNIEnv *env, jobject thiz, jbyte register_address,
                                                  jbyte register_value) {
    unsigned char register_address_ = static_cast<unsigned char>(register_address & 0xFF);
    unsigned char register_value_ = static_cast<unsigned char>(register_value & 0xFF);
    __android_log_print(ANDROID_LOG_DEBUG, "LED_CONTROL", "ledDriver_SetRegVal set value: register_address: %d, register_value: %d", register_address,register_value);
    jint result = ledDriver_SetRegVal_(register_address_,register_value_);
    __android_log_print(ANDROID_LOG_DEBUG, "LED_CONTROL", "ledDriver_SetRegVal: %d", result);
    return result;
}


/*
 * #include <iostream>
#include <vector>
#include <functional>
using namespace std;

vector<int> v;

vector<std::function<void()>> v_func;


void insertionInArrayOfVectors(int x){
    v.push_back(x);
}

void printElementsOfVector(){
    for (auto it = v.begin(); it != v.end(); ++it){
        cout << *it << ' ';
    }
}

void print_num( int x){
    cout << x << endl;
}


template <typename Temp>
Temp myMax(Temp x, Temp y)
{
    return (x>y)? x:y;
}

int multiply(int a, int b) {
    return a * b;
}

int add(int a, int b) {
    return a + b;
}

struct FuncInfo {
    std::function<int(int, int)> func;
    std::string name;
    int arg1;
    int arg2;
};


int main(){
    // insertionInArrayOfVectors(5);
    // insertionInArrayOfVectors(7);
    // insertionInArrayOfVectors(9);

    // printElementsOfVector();

    // for(auto i: v){
    //     cout << i << ' ';
    // }

    // cout << myMax<int>(3,7) << endl;
    std::function<void()> f_print_num = std::bind(print_num, 313378);
    f_print_num();

    std::vector<std::pair<std::function<int(int, int)>, std::pair<int, int>>> vec;

    vec.emplace_back(multiply, std::make_pair(5, 3));
    vec.emplace_back(add, std::make_pair(5, 3));

    for (const auto& i : vec) {
        std::cout << "Result: " << i.first(i.second.first, i.second.second) << std::endl;
    }


    std::vector<FuncInfo> vec1;

    vec1.push_back({multiply, "multiply", 5, 3});
    vec1.push_back({add, "add", 5, 3});

    for (const auto& i : vec1) {
        std::cout << "Executing: " << i.name << std::endl;
        std::cout << "Result: " << i.func(i.arg1, i.arg2) << std::endl;
    }

    // for(auto i:v_func){
    //     cout << i << ' ';
    // }

    return 0;
}

*/

//-------------------------------------
/*
    Using std::function: Since the function signatures are different,
    use std::function to store function pointers with various signatures.
    Storing Arguments: You can store the arguments for each function in a
    std::tuple to handle different numbers and types of arguments.

Sample Code

cpp

#include <iostream>
#include <vector>
#include <functional>
#include <tuple>

// Define example functions with different signatures
void func1(int a) {
    std::cout << "func1: " << a << std::endl;
}

void func2(double b, std::string c) {
    std::cout << "func2: " << b << ", " << c << std::endl;
}

void func3(int x, double y, char z) {
    std::cout << "func3: " << x << ", " << y << ", " << z << std::endl;
}

// Type alias to simplify the storage of functions
using FuncType = std::function<void(std::tuple<>)>;

// Utility to call a function with a tuple of arguments
template<typename F, typename Tuple, std::size_t... I>
void invoke_with_tuple(F f, Tuple&& t, std::index_sequence<I...>) {
    f(std::get<I>(std::forward<Tuple>(t))...);
}

template<typename F, typename Tuple>
void invoke(F f, Tuple&& t) {
    invoke_with_tuple(f, std::forward<Tuple>(t),
                      std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>::value>{});
}

// Store the function and corresponding argument tuple
struct FunctionWrapper {
    FuncType function;
    std::tuple<> args;
};

// Specialize the struct to allow different function arguments
template<typename F, typename... Args>
FunctionWrapper make_function_wrapper(F f, Args&&... args) {
    auto bound_func = [f](std::tuple<Args...> t) {
        invoke(f, std::move(t));
    };
    return {FuncType(bound_func), std::make_tuple(std::forward<Args>(args)...)};
}

int main() {
    // Create a vector of function wrappers
    std::vector<FunctionWrapper> functions;

    // Store different functions with their arguments
    functions.push_back(make_function_wrapper(func1, 42));
    functions.push_back(make_function_wrapper(func2, 3.14, std::string("Hello")));
    functions.push_back(make_function_wrapper(func3, 10, 20.5, 'a'));

    // Call the functions with their stored arguments
    for (auto& func : functions) {
        func.function(func.args);
    }

    return 0;
}

Explanation:

    Function Signatures: The example includes three functions func1, func2, and func3, each with different
    argument types and counts.

    FunctionWrapper: A FunctionWrapper struct stores the function and its arguments using std::function and std::tuple.
    Invoker: The invoke_with_tuple and invoke helper functions unpack the tuple arguments to call the stored function
    with the correct arguments.

    make_function_wrapper: This function helps create a FunctionWrapper by binding a function and its arguments,
    which are then stored in the vector.
    Executing Functions: The stored functions are executed in the for loop by calling the function with its respective
    arguments.

This approach allows you to store and manage a collection of functions with varying signatures and their arguments
 in a std::vector.*/

extern "C"
JNIEXPORT jint JNICALL
Java_com_example_MainActivity_sysInit(JNIEnv *env, jobject thiz) {
    jint result = init_system();
    return result;
}
extern "C"
JNIEXPORT jint JNICALL
Java_com_example_MainActivity_sysDinit(JNIEnv *env, jobject thiz) {
    jint result = system_UnInit();
    return result;
}
extern "C"
JNIEXPORT jint JNICALL
Java_com_example_MainActivity_solenoidSet(JNIEnv *env, jobject thiz, jint solenoid,
                                          jint solenoid_state) {
    jint result = TCA6416A_port_write_output_solenoid(solenoid, solenoid_state);
    return result;
}
extern "C"
JNIEXPORT jint JNICALL
Java_com_example_MainActivity_ledSetFrontPanel(JNIEnv *env, jobject thiz, jint led_front_panel,
                                               jint led_front_panel_state) {
    jint result = TCA6416A_port_set_output_driver_led(led_front_panel, led_front_panel_state);
    return result;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_example_MainActivity_turn_1on_1pwm_1(JNIEnv *env, jobject thiz) {
    // TODO: implement turn_on_pwm_()
    turn_on_pwm_sys();
    jint result = 1;
    return result;
}
extern "C"
JNIEXPORT jint JNICALL
Java_com_example_MainActivity_turn_1off_1pwm_1(JNIEnv *env, jobject thiz) {
    // TODO: implement turn_off_pwm_()
    turn_off_pwm_sys();
    jint result = 1;
    return result;
}