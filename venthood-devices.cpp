#include "venthood-devices.h"


Device::Device(bool inputDeviceState = false) : 
    onOffState(inputDeviceState) { }
   
void Device::turnDeviceOff(void) {
    onOffState = false;
}

void Device::turnDeviceOn(void) {
    onOffState = true;
}

bool Device::getDeviceState(void) {
    return onOffState;
}

//------------------------

Light::Light(bool inputDeviceState = false, Adafruit_Pixie* inputPixie = 0) : 
    Device(inputDeviceState), percentBrightness(0), pixieLightWrapper(inputPixie) {
        
    if (!pixieLightWrapper) {
        Serial.println("Pixie Pointer is Utopian Fantasy Wonderland of Segfaults!");
        return;
    }
    pixieLightWrapper->setBrightness(percentBrightness);
}
    
void Light::setBrightnessTo(int inputBrightness) {
    percentBrightness = inputBrightness;
    if (!pixieLightWrapper) {
        Serial.println("Pixie Pointer is Utopian Fantasy Wonderland of Segfaults!");
        return;
    }
    pixieLightWrapper->setBrightness(percentBrightness);
    Serial.print("LIGHT BRIGHTNESS ");
    Serial.println(percentBrightness);
}

int Light::getBrightnessLevel() {
    return percentBrightness;
}

void Light::executeLightChanges() {
    if (!pixieLightWrapper) {
        Serial.println("Pixie Pointer is Utopian Fantasy Wonderland of Segfaults!");
        return;
    }
    pixieLightWrapper->show();
    Serial.println("LIGHT CHANGE EXECUTED");
}

void Light::turnDeviceOff(void) {
    onOffState = false;
    this->setBrightnessTo(0);
    this->executeLightChanges();
    Serial.println("LIGHT OFF");
}

void Light::turnDeviceOn(void) {
    onOffState = true;
    this->setBrightnessTo(DEFAULT_BRIGHTNESS);
    this->executeLightChanges();
    Serial.println("LIGHT ON");
}

//------------------------

Fan::Fan(bool inputDeviceState = false) : 
    Device(inputDeviceState) { 
        this->setupFanRelayPins();
    }
    
Fan::Fan(bool inputDeviceState = false, fanPowerLevel inputFanSpeed = Off) :
    Device(inputDeviceState), fanSpeed(inputFanSpeed) {
        this->setupFanRelayPins();
    }
    
void Fan::setFanSpeed(int inputSpeed) {
    turnDeviceOff();
    switch(inputSpeed) {
        case Off:
            fanSpeed = Off;
            Serial.println("FAN OFF");
            break;
        
        case Low:
            fanSpeed = Low;
            digitalWrite(FAN_LOW_RELAY, HIGH);
            Serial.println("FAN LOW SPEED");
            break;
        
        case Med:
            fanSpeed = Med;
            digitalWrite(FAN_MED_RELAY, HIGH);
            Serial.println("FAN MEDIUM SPEED");
            break;
            
        case Hi:
            fanSpeed = Hi;
            digitalWrite(FAN_HI_RELAY, HIGH);
            Serial.println("FAN HIGH SPEED");
            break;
            
        case Boost:
            fanSpeed = Boost;
            digitalWrite(FAN_BOOST_RELAY, HIGH);
            Serial.println("FAN BOOST SPEED");
            break;
            
        default:
            fanSpeed = Off;
            Serial.println("NO ACTION/N.A.");
            break;
    }
}

fanPowerLevel Fan::currentFanSpeed() {
    return fanSpeed;
}

void Fan::turnDeviceOff(void) {
    onOffState = false;
    digitalWrite(FAN_LOW_RELAY, LOW);
    digitalWrite(FAN_MED_RELAY, LOW);
    digitalWrite(FAN_HI_RELAY, LOW);
    digitalWrite(FAN_BOOST_RELAY, LOW);
    Serial.println("FAN OFF");
}

void Fan::turnDeviceOn(void) {
    onOffState = true;
    this->setFanSpeed(Med);
}

void Fan::setupFanRelayPins(void) {
    pinMode(FAN_LOW_RELAY, OUTPUT);
    pinMode(FAN_MED_RELAY, OUTPUT);
    pinMode(FAN_HI_RELAY, OUTPUT);
    pinMode(FAN_BOOST_RELAY, OUTPUT);
    Serial.println("RelayPins have been defined with proper I/O modes.");
}
