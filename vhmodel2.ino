/** * Copyright (c) 2016 FirstBuild
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */
 
 // This #include statement was automatically added by the Particle IDE.

#include "clickButton/clickButton.h"
#include "venthood-devices.h"

//  Version
#define VENTHOOD_VERSION_MAJOR 0
#define VENTHOOD_VERSION_MINOR 1
#define VENTHOOD_VERSION_PATCH 0

//  Inputs pins
#define POWER_BTN_PIN A7
#define FAN_LOW_BTN_PIN A6
#define FAN_MED_BTN_PIN A5
#define FAN_HI_BOOST_BTN_PIN A4
#define LIGHT_BTN_PIN	A3

//  Output pins

// NOTE: These are the defaults pin numbers, softcoded into the library.
//  However, these pins may be remapped by reassigning different pins
//  to the associated PIN_NAME. If the default pins are used (the pins specified below), 
//  then the following preprocessor directives may be ommited. 
#define FAN_LOW_RELAY D5
#define FAN_MED_RELAY D4
#define FAN_HI_RELAY D3
#define FAN_BOOST_RELAY D2

#define PIXIE_OUTPUT TX
#define HALOGEN_HI__RELAY D1
#define HALOGEN_LOW_RELAY D0
#define GESTURE_SENSOR_2 A1
#define GESTURE_SENSOR_1 A0

#define MAX_ARGS 64

//	Pixie Controller configuration
#define NUM_OF_PIXIES    2
#define DEFAULT_BRIGHTNESS  200
#define LOW_BRIGHTNESS  75

Adafruit_Pixie strip = Adafruit_Pixie(2, &Serial1);

ClickButton powerBtn(POWER_BTN_PIN, HIGH);
ClickButton fanLowBtn(FAN_LOW_BTN_PIN, HIGH);
ClickButton fanMedBtn(FAN_MED_BTN_PIN, LOW);
ClickButton fanHiBoostBtn(FAN_HI_BOOST_BTN_PIN, LOW);
ClickButton lightBtn(LIGHT_BTN_PIN, LOW);
Fan venthoodFan = Fan(false);
Light venthoodLights= Light(false, &strip);

int powerBtnState = 0;

void setup() {
    Serial.begin(9600);
    Serial1.begin(115200);
  
    pinMode(POWER_BTN_PIN , INPUT);
    pinMode(FAN_LOW_BTN_PIN , INPUT);
    pinMode(FAN_MED_BTN_PIN , INPUT);
    pinMode(FAN_HI_BOOST_BTN_PIN , INPUT);
    pinMode(LIGHT_BTN_PIN , INPUT);
    // pinMode(GESTURE_SENSOR_2, INPUT);
    // pinMode(GESTURE_SENSOR_1, INPUT);
    
    pinMode(HALOGEN_HI__RELAY, OUTPUT);
    pinMode(HALOGEN_LOW_RELAY, OUTPUT);
    
    powerBtn.longClickTime = 2000;
    
    // Particle.function("incDecShifter", brightDimLight);
    Particle.function("setvalue", setPercentage);
    Particle.function("onoff", onoffDevice);
}


void loop()
{
    powerBtn.Update();
    fanLowBtn.Update();
    fanMedBtn.Update();
    fanHiBoostBtn.Update();
    lightBtn.Update();
    
    powerBtnState = powerBtn.clicks;
    
  // Save click codes in LEDfunction, as click codes are reset at next Update()
    if(powerBtnState == -1) {
        Serial.println("THIS IS POWER");
        if (Particle.connected()) {
            // Particle.disconnect();
        } else {
        //  No delay is required here because connect() is blocking
        //      until connection is estabilshed. Typically approx. 1 sec. delay.
            // Particle.connect();
        }
    } else if (powerBtnState == 1) {
        venthoodFan.turnDeviceOff();
        venthoodLights.turnDeviceOff();
        delay(250);
    }
    
    if (fanLowBtn.clicks == 1) {
        venthoodFan.setFanSpeed(1);
    } else if (fanMedBtn.clicks == 1) {
        venthoodFan.setFanSpeed(2);
    } else if (fanHiBoostBtn.clicks == 1) {
        if (venthoodFan.currentFanSpeed() != Hi) {
            venthoodFan.setFanSpeed(3);
        } else if (venthoodFan.currentFanSpeed() == Hi) {
            venthoodFan.setFanSpeed(4);
        }
    }
    
    if (lightBtn.clicks == 1) {
        if (venthoodLights.getBrightnessLevel() == Off) {
            venthoodLights.setBrightnessTo(LOW_BRIGHTNESS);
        } else if (venthoodLights.getBrightnessLevel() == LOW_BRIGHTNESS) {
            venthoodLights.setBrightnessTo(DEFAULT_BRIGHTNESS);
        } else if (venthoodLights.getBrightnessLevel() == DEFAULT_BRIGHTNESS) {
            venthoodLights.turnDeviceOff();
        } else {
            venthoodLights.turnDeviceOff();
        }
    }
    
    strip.setPixelColor(0, 255, 255, 255);
    strip.setPixelColor(1, 255, 255, 255);
    venthoodLights.executeLightChanges();
    
    powerBtnState = 0;
    delay(5);
}

int onoffDevice(String args) {
    int index = args.toInt();
    int value = 0;
    char applianceArgs[MAX_ARGS];

    args.toCharArray(applianceArgs, MAX_ARGS);
    
    sscanf(applianceArgs, "%d=%d", &index, &value);
    
    Serial.println();
    Serial.print("On/Off");
    Serial.println();
    Serial.print("Argument Value: ");
    Serial.print(args);
    Serial.println();
    Serial.print("Device: ");
    Serial.print(index ? "Fan" : "Lights");
    Serial.println();
    Serial.print("Value: ");
    Serial.print(value);
    Serial.println();

//  If index is 0 or 1 for device 0 or 1
	switch (index) {
	    case 0:
	        //  Turn on lights
	        if (value) {
	            venthoodLights.turnDeviceOn();
	        } else {
                venthoodLights.turnDeviceOff();
	        }
	        venthoodLights.executeLightChanges();
	        break;
	    case 1:
	        //  Turn on Fan
	        //  When value == 1, the relay switches to NO
	        //      which is open, or turns the switch/fan off
	        //      Vice versa applies. 
            if (value) {
	            venthoodFan.turnDeviceOn();
            } else {
	            venthoodFan.turnDeviceOff();
            }
	        break;
	}
	
	return index;
}

int setPercentage(String args) {
    int index = args.toInt();
    int value = 0;
    int incDecIdentifier = -1;
    char applianceArgs[MAX_ARGS];
    
    args.toCharArray(applianceArgs, MAX_ARGS);
    
    sscanf(applianceArgs, "%d=%d=%d", &index, &value, &incDecIdentifier);

    int powerLevel = (int) (((float) value * 250) / 100.0);
    
    if (index == 1) {
        powerLevel = value < 5 ? value : 4;
    }
    
    Serial.println();
    Serial.print("Percentage");
    Serial.println();
    Serial.print("Arguments: ");
    Serial.print(args);
    Serial.println();
    Serial.print("Device: ");
    Serial.print(index ? "Fan" : "Lights");
    Serial.println();
    Serial.print("Level: ");
    Serial.print(powerLevel);
    Serial.println();
    
    //  If index is 0 or 1 for device 0 or 1
	switch (index) {
	    case 0:
	        //  Turn on lights
	        //  Value 1 is increment, Value 0 is decrement.
	        if (incDecIdentifier == 1) {
	            powerLevel += venthoodLights.getBrightnessLevel();
	            Serial.println("BRIGHTNESS APPENDED");
	        } else if (incDecIdentifier == 0) {
	            powerLevel = venthoodLights.getBrightnessLevel() - powerLevel;
	            Serial.println("BRIGHTNESS APPENDED");
	        }
	        
	        powerLevel = powerLevel <= 250 ? powerLevel : 250;
	        powerLevel = powerLevel >= 0 ? powerLevel : 0;
    
            venthoodLights.setBrightnessTo(powerLevel);
	        venthoodLights.executeLightChanges();
	        break;
	    case 1:
	        //  Turn on Fan
	        //  When value == 1, the relay switches to NO
	        //      which is open, or turns the switch/fan off
	        //      Vice versa applies. 
            venthoodFan.setFanSpeed(powerLevel);
	        break;
	}
    return powerLevel;
}

