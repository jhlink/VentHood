#ifndef VENTHOOD_H
#define VENTHOOD_H

#include <application.h>
#include "AnalogInputDebounced.h"
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

const int LIGHT_BTN_VOLTAGE = 4095;
const int POWER_BTN_VOLTAGE = 3700;
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
        void setupFanRelayPins(void);

    public:
        Fan(bool inputDeviceState);
        Fan(bool inputDeviceState, fanPowerLevel inputFanSpeed);
        void setFanSpeed(int inputSpeed);
        fanPowerLevel currentFanSpeed(void);
        void turnDeviceOff(void);
        void turnDeviceOn(void);
        void process(void);
};

#endif
