#include "venthood-devices.h"


// ---------------------
//  Santa's Little Helper Functions
// ---------------------

void switchToChannel(int targChan) {

    // TODO
    /// REMOVE TASTE SENSING CHANNEL.

    digitalWrite(ENABLE, HIGH);
    delay(10);
    switch (targChan) {
        //  N/A
        case 0:
            digitalWrite(CH_A, LOW);
            digitalWrite(CH_B, LOW);
            digitalWrite(CH_C, LOW);
            break;

        //  Lights
        case 1:
            digitalWrite(CH_A, HIGH);
            digitalWrite(CH_B, LOW);
            digitalWrite(CH_C, LOW);
            break;

        //  Power button
        case 2:
            digitalWrite(CH_A, LOW);
            digitalWrite(CH_B, HIGH);
            digitalWrite(CH_C, LOW);
            break;

        //  Fan low
        case 3:
            digitalWrite(CH_A, HIGH);
            digitalWrite(CH_B, HIGH);
            digitalWrite(CH_C, LOW);
            break;

        //  Fan medium
        case 4:
            digitalWrite(CH_A, LOW);
            digitalWrite(CH_B, LOW);
            digitalWrite(CH_C, HIGH);
            break;

        // Fan hi
        case 5:
            digitalWrite(CH_A, HIGH);
            digitalWrite(CH_B, LOW);
            digitalWrite(CH_C, HIGH);
            break;

        //  N/A
        case 6:
            digitalWrite(CH_A, LOW);
            digitalWrite(CH_B, HIGH);
            digitalWrite(CH_C, HIGH);
            break;

        //  N/A
        case 7:
            digitalWrite(CH_A, HIGH);
            digitalWrite(CH_B, HIGH);
            digitalWrite(CH_C, HIGH);
            break;

        //  Boost mode
        case 8:
            digitalWrite(CH_A, HIGH);
            digitalWrite(CH_B, LOW);
            digitalWrite(CH_C, HIGH);
            break;

        default:
            digitalWrite(CH_A, LOW);
            digitalWrite(CH_B, LOW);
            digitalWrite(CH_C, LOW);
            break;
    }
    delay(20);
    digitalWrite(ENABLE, LOW);
    delay(250);
    if (targChan == 8) {
        delay(2250);
    }
    digitalWrite(ENABLE, HIGH);

}

// ---------------------
//  DEVICE CLASS
// ---------------------

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

// ---------------------
//  LIGHT CLASS
// ---------------------


Light::Light(bool inputDeviceState = false) :
    Device(inputDeviceState), percentBrightness(0) {

    lightState = 0;
    prevState = 50;

}

void Light::setBrightnessTo(int inputBrightness) {

    percentBrightness = inputBrightness;
    Serial.print("LIGHT BRIGHTNESS ");
    Serial.println(percentBrightness);
}

int Light::getBrightnessLevel(void) {
    return percentBrightness;
}

void Light::executeLightChanges(void) {

    // TODO: Implement "hop" feature for buttons.

    if (analogRead(LIGHT_STATE) < 3000) {
        lightState = 0;
        percentBrightness = 0;
    }

    int desiredState = 0;
    if (percentBrightness == 50) {
        desiredState = 2;
    } else if (percentBrightness == 100) {
        desiredState = 1;
    }

    Serial.println("LIGHT TEST");
    for (int i = 0; i < 2; i++) {
        Serial.println(lightState);
        Serial.println(desiredState);
        if (lightState != desiredState) {
            switchToChannel(1);
            lightState = (lightState + 1) % 3;
            Serial.println("LIGHT CHANGE STATE");
        } else {
            break;
        }
    }

    Serial.println("LIGHT CHANGE EXECUTED");
}

void Light::turnDeviceOff(void) {
    onOffState = false;
    prevState = percentBrightness;
    this->setBrightnessTo(0);
    this->executeLightChanges();
    Serial.println("LIGHT OFF");
}

void Light::turnDeviceOn(void) {
    onOffState = true;
    this->setBrightnessTo(prevState);
    this->executeLightChanges();
    Serial.println("LIGHT ON");
}

void Light::process(void) {

    AnalogInputDebounced checkingForLightButton = AnalogInputDebounced(TASTI_READ, LIGHT_BTN_VOLTAGE);
    checkingForLightButton.updateInput();

    if (checkingForLightButton.isUniquelyActive()) {
        lightState = (lightState + 1) % 3;
        percentBrightness = (lightState * 50) % 101;
    }

}


// ---------------------
//  FAN CLASS
// ---------------------


Fan::Fan(bool inputDeviceState = false) :
    Device(inputDeviceState) { }

Fan::Fan(bool inputDeviceState = false, fanPowerLevel inputFanSpeed = Off) :
    Device(inputDeviceState), fanSpeed(inputFanSpeed) {

}

void Fan::setFanSpeed(int inputSpeed) {
    switch(inputSpeed) {
        case Off:
            fanSpeed = Off;
            turnDeviceOff();
            Serial.println("FAN OFF");
            break;

        case Low:
            fanSpeed = Low;
            switchToChannel(3);
            Serial.println("FAN LOW SPEED");
            break;

        case Med:
            fanSpeed = Med;
            switchToChannel(4);
            Serial.println("FAN MEDIUM SPEED");
            break;

        case Hi:
            fanSpeed = Hi;
            switchToChannel(5);
            Serial.println("FAN HIGH SPEED");
            break;

        case Boost:
            fanSpeed = Boost;
            switchToChannel(8);
            Serial.println("FAN BOOST SPEED");
            break;

        default:
            fanSpeed = Off;
            turnDeviceOff();
            Serial.println("NO ACTION/N.A.");
            break;
    }
}

fanPowerLevel Fan::currentFanSpeed(void) {
    return fanSpeed;
}

void Fan::turnDeviceOff(void) {
    onOffState = false;
    switchToChannel(2);
    Serial.println("FAN OFF");
}

void Fan::turnDeviceOn(void) {
    onOffState = true;
    this->setFanSpeed(Med);
}

void Fan::process(void) {
    static int cycleCount = 0;
    AnalogInputDebounced checkingForWiFiReset = AnalogInputDebounced(TASTI_READ, POWER_BTN_VOLTAGE);
    checkingForWiFiReset.updateInput();

    if (checkingForWiFiReset.isActive() && cycleCount < FAN_WIFI_RESET_SIGNAL_TIMEOUT) {
        cycleCount += 1;
    } else {
        cycleCount = 0;
    }
}
