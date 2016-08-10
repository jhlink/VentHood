#ifndef VENTHOOD_H
#define VENTHOOD_H

#include "Adafruit_Pixie.h"

enum fanPowerLevel { Off, Low, Med, Hi, Boost };

class Device {
    protected:
        bool onOffState;
    
    public: 
        Device(bool inputDeviceState);
        virtual void turnDeviceOff(void);
        virtual void turnDeviceOn(void);
        virtual bool getDeviceState(void);
};

#ifndef DEFAULT_BRIGHTNESS 
#define DEFAULT_BRIGHTNESS 200
#endif

class Light : public Device {
    private:
        int percentBrightness;
        Adafruit_Pixie* pixieLightWrapper;
        
    public:
        Light(bool inputDeviceState, Adafruit_Pixie* inputPixie);
            
        void setBrightnessTo(int inputBrightness);
        int getBrightnessLevel();
        void executeLightChanges();
        void turnDeviceOff(void);
        void turnDeviceOn(void);
};

#ifndef FAN_LOW_RELAY
#define FAN_LOW_RELAY D5
#endif

#ifndef FAN_MED_RELAY
#define FAN_MED_RELAY D4
#endif

#ifndef FAN_HI_RELAY
#define FAN_HI_RELAY D3
#endif
 
#ifndef FAN_BOOST_RELAY
#define FAN_BOOST_RELAY D2
#endif

class Fan : public Device {
    private:
        fanPowerLevel fanSpeed;
        void setupFanRelayPins();
        
    public:
        Fan(bool inputDeviceState);
        Fan(bool inputDeviceState, fanPowerLevel inputFanSpeed);
            
        void setFanSpeed(int inputSpeed);
        fanPowerLevel currentFanSpeed();
        void turnDeviceOff(void);
        void turnDeviceOn(void);
};

#endif