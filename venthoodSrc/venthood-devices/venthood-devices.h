#ifndef VENTHOOD_H
#define VENTHOOD_H

#include <application.h>
#include "../AnalogInputDebounced/AnalogInputDebounced.h"
#include "../APDS9960/SparkFun_APDS9960.h"
#include "assert.h"

#ifndef CH_A
#define CH_A D2
#endif

#ifndef CH_B
#define CH_B D3
#endif

#ifndef CH_C
#define CH_C D4
#endif

#ifndef ENABLE
#define ENABLE D5
#endif

#ifndef LIGHT_STATE
#define LIGHT_STATE A1
#endif

#ifndef TASTI_READ
#define TASTI_READ A0
#endif

#ifndef LIGHT_SIGNAL_TIMEOUT
#define LIGHT_SIGNAL_TIMEOUT 30
#endif

#ifndef FAN_WIFI_RESET_SIGNAL_TIMEOUT
#define FAN_WIFI_RESET_SIGNAL_TIMEOUT 5000
#endif

#ifndef DEFAULT_BRIGHTNESS
#define DEFAULT_BRIGHTNESS 50
#endif

#ifndef APDS9960_INT
#define APDS9960_INT D6
#endif

const int LIGHT_BTN_VOLTAGE = 2450;
const int POWER_BTN_VOLTAGE = 1820;
const int FAN_HI_BTN_VOLTAGE = 100;
const int FAN_MED_BTN_VOLTAGE = 620;
const int FAN_LOW_BTN_VOLTAGE = 1220;
enum fanPowerLevel { Off=0, Low=25, Med=50, Hi=75, Boost=100 };

class Device {
    protected:
        bool onOffState;

    public:
        Device(bool inputDeviceState);
        virtual void turnDeviceOff(void);
        virtual void turnDeviceOn(void);
        virtual bool getDeviceState(void);
        virtual void process(void)=0;
};

class Light : public Device {
    private:
        int percentBrightness;
        int lightState;
        int prevState;
        AnalogInputDebounced checkingForLightButton;

    public:
        Light(bool inputDeviceState);
        void setBrightnessTo(int inputBrightness);
        int getBrightnessLevel(void);
        void executeLightChanges(void);
        void turnDeviceOff(void);
        void turnDeviceOn(void);
        void process(void);
        void updateLightState(void);
};

class Fan : public Device {
    private:
        fanPowerLevel fanSpeed;
        fanPowerLevel prevState;
        AnalogInputDebounced checkingForFanLowButton;
        AnalogInputDebounced checkingForFanMedButton;
        AnalogInputDebounced checkingForFanHiButton;
        AnalogInputDebounced checkingForPowerButton;
        bool longPressedBtn;

        void setupFanRelayPins(void);

    public:
        Fan(bool inputDeviceState);
        void setFanSpeed(int inputSpeed);
        void executeFanChanges(void);
        fanPowerLevel currentFanSpeed(void);
        void turnDeviceOff(void);
        void turnDeviceOn(void);
        void process(void);
        bool getLongPressedBoolean();
        void setLongPressedBoolean(bool inputBool);
};

class Gesture: public Device {
    private:
        int gestureFlag;
        volatile int isr_flag;
        SparkFun_APDS9960 apds;
        Light& lightDevice;
        Fan& fanDevice;

        void handleGesture(void);

    public:
        Gesture(Light& lightDevice, Fan& fanDevice);
        void init(void);
        void interruptRoutine(void);
        void turnDeviceOff(void);
        void turnDeviceOn(void);
        bool getDeviceState(void);
        void process(void);
};

#endif
