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
    checkingForLightButton = AnalogInputDebounced(TASTI_READ, LIGHT_BTN_VOLTAGE);
}

void Light::setBrightnessTo(int inputBrightness) {

    percentBrightness = inputBrightness;
    Serial.print("LIGHT BRIGHTNESS ");
    Serial.println(percentBrightness);
}

int Light::getBrightnessLevel(void) {
    return percentBrightness;
}

void Light::updateLightState() {
    static unsigned long prevTime = millis();

    if ((millis() - prevTime) < 20) {
        return;
    } else {
        prevTime = millis();
    }

    int lightStateReading = analogRead(LIGHT_STATE);
    if (lightStateReading < 3000) {
        lightState = 0;
    } else if ((lightStateReading >= 3000) && lightState == 0) {
        lightState = 1;
    }
}

void Light::executeLightChanges(void) {

    // TODO: Implement "hop" feature for buttons.

    int desiredState = 0;
    if (percentBrightness == 50) {
        desiredState = 2;
    } else if (percentBrightness == 100) {
        desiredState = 1;
    }

    Serial.println("LIGHT TEST");
    Serial.print("DESIRED STATE: ");
    Serial.println(desiredState);
    while (!(lightState == desiredState)) {
        updateLightState();
        switchToChannel(1);
        lightState = ((lightState + 1) % 3);
        Serial.println("LIGHT CHANGE STATE");
        delay(150);
    }

    Serial.println(lightState);
    Serial.println(percentBrightness);

    Serial.println("LIGHT CHANGE EXECUTED");
}

void Light::turnDeviceOff(void) {
    if (!onOffState) {
      return;
    }
    onOffState = false;
    prevState = percentBrightness;
    this->setBrightnessTo(0);
    this->executeLightChanges();
    Serial.println("LIGHT OFF");
}

void Light::turnDeviceOn(void) {
    if (onOffState) {
      return;
    }
    onOffState = true;
    this->setBrightnessTo(prevState);
    this->executeLightChanges();
    Serial.println("LIGHT ON");
}

void Light::process(void) {

    checkingForLightButton.updateInput();

    if (checkingForLightButton.isUniquelyActive()) {
        lightState = (lightState + 1) % 3;
        percentBrightness = lightState == 1 ? 100 : 50;
        percentBrightness = lightState == 0 ? 0 : percentBrightness;
        onOffState = lightState >= 1 ? true : false;

        Serial.println(lightState);
        Serial.println(percentBrightness);
    }

    updateLightState();
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
  fanPowerLevel enumPlaceholder;
  switch (inputSpeed) {
      case 0:
        enumPlaceholder = Off;
        break;

      case 25:
        enumPlaceholder = Low;
        break;

      case 50:
        enumPlaceholder = Med;
        break;

      case 75:
        enumPlaceholder = Hi;
        break;

      case 100:
        enumPlaceholder = Boost;
        break;

      default:
        enumPlaceholder = Off;
        break;
  }
  fanSpeed = enumPlaceholder;
}

void Fan::executeFanChanges(void) {
    switch(fanSpeed) {
        case Off:
            turnDeviceOff();
            Serial.println("FAN OFF");
            break;

        case Low:
            switchToChannel(3);
            Serial.println("FAN LOW SPEED");
            break;

        case Med:
            switchToChannel(4);
            Serial.println("FAN MEDIUM SPEED");
            break;

        case Hi:
            switchToChannel(5);
            Serial.println("FAN HIGH SPEED");
            break;

        case Boost:
            switchToChannel(8);
            Serial.println("FAN BOOST SPEED");
            break;

        default:
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

void Gesture::interruptRoutine() {
  isr_flag = 1;
}

Gesture::Gesture(Light& inputLightDevice, Fan& inputFanDevice) :
  Device(true), isr_flag(-1), apds(SparkFun_APDS9960()), lightDevice(inputLightDevice), fanDevice(inputFanDevice) {}

void Gesture::init(void) {

 pinMode(APDS9960_INT, INPUT);

 //apds = SparkFun_APDS9960();
 attachInterrupt(APDS9960_INT, &Gesture::interruptRoutine, this, FALLING);

 // Initialize APDS-9960 (configure I2C and initial values)
 if ( apds.init() ) {
   Serial.println(F("APDS-9960 initialization complete"));
 } else {
   Serial.println(F("Something went wrong during APDS-9960 init!"));
 }

 // Start running the APDS-9960 gesture sensor engine
 if ( apds.enableGestureSensor(true) ) {
   Serial.println(F("Gesture sensor is now running"));
 } else {
   Serial.println(F("Something went wrong during gesture sensor init!"));
 }
}

void Gesture::handleGesture() {
    int lightBrightness = lightDevice.getBrightnessLevel();
    int fanLevel = fanDevice.currentFanSpeed();
    if ( apds.isGestureAvailable() ) {
        switch ( apds.readGesture() ) {
          case DIR_UP:
            Serial.println("UP");
            gestureFlag = 1;
            lightBrightness += 50;
            lightBrightness = lightBrightness > 100 ? 100 : lightBrightness;
            lightDevice.setBrightnessTo(lightBrightness);
            break;

          case DIR_DOWN:
            Serial.println("DOWN");
            gestureFlag = 2;
            lightBrightness -= 50;
            lightBrightness = lightBrightness < 0 ? 0 : lightBrightness;
            lightDevice.setBrightnessTo(lightBrightness);
            break;

          case DIR_LEFT:
            Serial.println("LEFT");
            gestureFlag = 3;
            fanLevel -= 25;
            fanLevel = fanLevel < 0? 0 : fanLevel;
            fanDevice.setFanSpeed(fanLevel);
            break;

          case DIR_RIGHT:
            Serial.println("RIGHT");
            gestureFlag = 4;
            fanLevel += 25;
            fanLevel = fanLevel > 100 ? 100 : fanLevel;
            fanDevice.setFanSpeed(fanLevel);
            break;

          case DIR_NEAR:
            Serial.println("NEAR");
            break;

          case DIR_FAR:
            Serial.println("FAR");
            break;

          default:
            Serial.println("NONE");
            gestureFlag = 0;
            break;

        }
  }
}

void Gesture::turnDeviceOff(void) {
  detachInterrupt(APDS9960_INT);
  apds.disableGestureSensor();
}

void Gesture::turnDeviceOn(void) {
  attachInterrupt(APDS9960_INT, &Gesture::interruptRoutine, this, FALLING);
  apds.enableGestureSensor(true);
}

bool Gesture::getDeviceState(void) {
  return apds.isGestureAvailable();
}

void Gesture::process(void) {
  if( isr_flag == 1 ) {
    detachInterrupt(APDS9960_INT);
    handleGesture();
    isr_flag = 0;
    attachInterrupt(APDS9960_INT, &Gesture::interruptRoutine, this, FALLING);
  }

  // Cases corresponding to certain Gesture Flags
  //  in case repetitive looping is required
  switch (gestureFlag) {
    case 0:
      break;

    case 1:
      lightDevice.executeLightChanges();
      gestureFlag = 0;
      break;

    case 2:
      lightDevice.executeLightChanges();
      gestureFlag = 0;
      break;

    case 3:
      fanDevice.executeFanChanges();
      gestureFlag = 0;
      break;

    case 4:
      fanDevice.executeFanChanges();
      gestureFlag = 0;
      break;

    default:
      gestureFlag = 0;
      break;
  }
}
