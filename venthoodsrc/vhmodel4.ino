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
#include "venthood-devices.h"

//  Version
#define VENTHOOD_VERSION_MAJOR 0
#define VENTHOOD_VERSION_MINOR 4
#define VENTHOOD_VERSION_PATCH 2

//  Inputs pins

// NOTE: These are the defaults pin numbers, softcoded into the library.
//  However, these pins may be remapped by reassigning different pins
//  to the associated PIN_NAME. If the default pins are used (the pins specified below),
//  then the following preprocessor directives may be ommited.

#define LIGHT_STATE A1
#define TASTI_READ A0

//  Output pins

// NOTE: These are the defaults pin numbers, softcoded into the library.
//  However, these pins may be remapped by reassigning different pins
//  to the associated PIN_NAME. If the default pins are used (the pins specified below),
//  then the following preprocessor directives may be ommited.

#define CH_A D2
#define CH_B D3
#define CH_C D4

// #define GESTURE_SENSOR_2 A1
// #define GESTURE_SENSOR_1 A0

#define MAX_ARGS 64

//  Pixie Controller configuration
// #define NUM_OF_PIXIES    2
// #define DEFAULT_BRIGHTNESS  200
// #define LOW_BRIGHTNESS  75

Fan venthoodFan = Fan(false);
Light venthoodLights = Light(false);
Gesture venthoodGesture = Gesture(venthoodLights, venthoodFan);
bool lightMode = false;

void swChannel(int targChan) {

    // TODO
    /// REMOVE TASTE SENSING CHANNEL.

    digitalWrite(ENABLE, HIGH);
    delay(10);
    switch (targChan) {
        case 0:
            // digitalWrite(CH_A, LOW);
            // digitalWrite(CH_B, LOW);
            // digitalWrite(CH_C, LOW);
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

        // Fan Hi
        case 5:
            digitalWrite(CH_A, HIGH);
            digitalWrite(CH_B, LOW);
            digitalWrite(CH_C, HIGH);
            break;

        case 6:
            digitalWrite(CH_A, LOW);
            digitalWrite(CH_B, HIGH);
            digitalWrite(CH_C, HIGH);
            break;

        case 7:
            digitalWrite(CH_A, HIGH);
            digitalWrite(CH_B, HIGH);
            digitalWrite(CH_C, HIGH);
            break;

        case 8:
            // BOOST MODE
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
    delay(50);
    digitalWrite(ENABLE, LOW);
    delay(2000);
    if (targChan == 8) {
        delay(2250);
    }
    digitalWrite(ENABLE, HIGH);
}


int deviceClass[] = {1, 1, 1, 2, 3, 4, 5, 8};
int currentValue = 0;
void beammeup() {
    int testState = deviceClass[(currentValue++) % 8];
    swChannel(testState);

    switch (testState) {
        case 1:
            Serial.println("POWER BUTTON: FAN TURNS OFF");
            break;

        case 2:
            Serial.println("LIGHT BUTTON: LOW, HIGH, OFF STATES");
            break;

        case 3:
            Serial.println("FAN LOW BUTTON");
            break;

        case 4:
            Serial.println("FAN MED BUTTON");
            break;

        case 5:
            Serial.println("FAN HI BUTTON");
            break;

        case 8:
            Serial.println("FAN BOOST STATE");
            break;
    }
}

void measure() {
  Serial.println(analogRead(TASTI_READ));
}

Timer functionTest(5000, beammeup);

void setup() {
    Serial.begin(9600);
    while(!Serial);
    pinMode(CH_A, OUTPUT);
    pinMode(CH_B, OUTPUT);
    pinMode(CH_C, OUTPUT);
    pinMode(ENABLE, OUTPUT);
    pinMode(LIGHT_STATE, INPUT);
    pinMode(TASTI_READ, INPUT);
    venthoodGesture.init();
    functionTest.stop();

    // pinMode(GESTURE_SENSOR_2, INPUT);
    // pinMode(GESTURE_SENSOR_1, INPUT);

    Particle.function("setvalue", setPercentage);
    Particle.function("onoff", onoffDevice);
}

void loop() {
    //venthoodFan.process();
    venthoodLights.process();
    venthoodGesture.process();
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
      if (value == 1) {
        venthoodLights.turnDeviceOn();
      } else {
        venthoodLights.turnDeviceOff();
      }
      break;
      case 1:
      //  Turn on Fan
      if (value == 1) {
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

    int powerLevel = value;

    Serial.println();
    Serial.print("Percentage");
    Serial.println();
    Serial.print("Arguments: ");
    Serial.print(args);
    Serial.println();
    Serial.print("Device: ");
    Serial.print(index ? "Fan" : "Lights");
    Serial.println();
    Serial.print("0%, 25%, 50%, 75%, 100%: ");
    Serial.print(powerLevel);
    Serial.println();

    //  If index is 0 or 1 for device 0 or 1
    switch (index) {
      case 0:
          //  Turn on lights
          //  Value 1 is increment, Value 0 is decrement.

          //  Correct values. Dim/Brighten features are the
          //    only features enabled. By default,
          //    brightness increments or decrements by 25.

          powerLevel = venthoodLights.getBrightnessLevel() / 50;
          if (incDecIdentifier == 1) {
              powerLevel++;
              Serial.println("BRIGHTENED");
          } else if (incDecIdentifier == 2) {
              powerLevel--;
              Serial.println("DIMMED");
          }

          powerLevel = powerLevel <= 2 ? powerLevel : 2;
          powerLevel = powerLevel >= 0 ? powerLevel : 0;
          powerLevel = powerLevel * 50;

          venthoodLights.setBrightnessTo(powerLevel);
          venthoodLights.executeLightChanges();
          break;

      case 1:
          //  Turn on Fan
          venthoodFan.setFanSpeed(powerLevel);
          venthoodFan.executeFanChanges();
          break;
    }
    return powerLevel;
}
