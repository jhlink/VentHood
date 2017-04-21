/** * Copyright (c) 2016 FirstBuild
 * * Permission is hereby granted, free of charge, to any person obtaining a copy * of this software and associated documentation files (the "Software"), to deal * in the Software without restriction, including without limitation the rights * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
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
#include "venthood-devices/venthood-devices.h"

//#define DEBUG_PRINTING_ON
#ifdef DEBUG_PRINTING_ON
#define DEBUG_PRINTLN(m) serial.println(m)
#define DEBUG_PRINT(m) serial.print(m)
#else
#define DEBUG_PRINTLN(m) 
#define DEBUG_PRINT(m)
#endif

#undef GESTURE
#define VOICE

#pragma SPARK_NO_PREPROCESSOR
#include "Particle.h"
#include "softap_http.h"
#include "softiePage/webpage.h"

#define VENTHOOD_VERSION_MAJOR 0
#define VENTHOOD_VERSION_MINOR 3
#define VENTHOOD_VERSION_PATCH 9

SYSTEM_THREAD(ENABLED);
SYSTEM_MODE(SEMI_AUTOMATIC);

struct Page
{
  const char* url;
  const char* mime_type;
  const char* data;
  const uint32_t length;
};

Page myPages[] = {
  { "/index.html", "text/html", index_html, 0 },
  { "/rsa-utils/rsa.js", "application/javascript", rsa_js, 0 },
  { "/style.css", "text/css", style_css, 0 },
  { "/rsa-utils/rng.js", "application/javascript", rng_js, 0 },
  { "/rsa-utils/jsbn_2.js", "application/javascript", jsbn_2_js, 0 },
  { "/rsa-utils/jsbn_1.js", "application/javascript", jsbn_1_js, 0 },
  { "/devnam.js", "application/javascript", devnam_js, 0 },
  { "/softap.js", "application/javascript", softap_js, 0 },
  { "/rsa-utils/prng4.js", "application/javascript", prng4_js, 0 },
  { "/1BLogo.svg", "image/svg+xml", OneBLogo_svg, 0 },
  { "/FSEmeric-Regular.woff", "application/octet-stream", (const char *)FSEmeric_Regular_woff, FSEmeric_Regular_woff_len },
  { NULL, NULL, NULL, 0 }
};

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

#define WIFI_CONNECTING_TIMEOUT 10000
#define COMMISSION_TIMEOUT 600000

Fan venthoodFan = Fan(false);
Light venthoodLights = Light(false);
Gesture venthoodGesture = Gesture(venthoodLights, venthoodFan);
bool lightMode = false;
String jsonPayload = ""; 
static unsigned long commissionTimeTracker = 0;

static void chunkedSender(Writer* result, const uint8_t *pData, uint32_t length) {
  uint16_t chunkSize = 1028;

  Serial.println("Sending chunked data.");

  while ( length > 0 ) {
    if (length <= chunkSize) {
      chunkSize = length;
    }
    result->write(pData, chunkSize);
    pData += chunkSize;
    length -= chunkSize;
    HAL_Delay_Milliseconds(10);
  }

  Serial.println("Data sent.");
}

void wifiResetAndPowerCycle() {
  WiFi.clearCredentials();
  System.reset();
}

void myPage(const char* url, ResponseCallback* cb, void* cbArg, Reader* body, Writer* result, void* reserved) {
  Serial.printlnf("handling page %s", url);

  if (strcmp(url,"/index")==0) {
    Serial.println("sending redirect");
    Header h("Location: /index.html\r\n");
    cb(cbArg, 0, 301, "text/plain", &h);
    return;
  }

  if (strcmp(url, "/cfg") ==0) {
    char* data = body->fetch_as_string();
    // We'll need to use SparkJSON for this.
    //{"idx":0,"lightNm":"asdfasdf","fanNm":"dfasdfasd","email":"asdfasdfasdf"}
    Serial.print("POST Data: ");
    jsonPayload = data;
    Serial.println(jsonPayload);
    cb(cbArg, 0, 200, "text/plain", nullptr);
    result->write("{\"status\": \"OK\"}");
    // free the data! IMPORTANT!
    free(data);
    return;
  }

  if (strcmp(url, "/alive") ==0) {
    Serial.println("I'm alive.");
    signed long remainingTimeToReset = COMMISSION_TIMEOUT - (millis() - commissionTimeTracker);
    String timerResult = String::format("{\"a\": %s }", String(remainingTimeToReset, DEC).c_str());
    size_t timerResultCharArrLen =  (timerResult.length() + 1);
    char timerResultInCharArray[timerResultCharArrLen];
    timerResult.toCharArray(timerResultInCharArray, timerResultCharArrLen);

    if (remainingTimeToReset <= -5000) {
      Serial.println("COMMISSION TIMEOUT -- EXECUTE RESET");
      wifiResetAndPowerCycle();
    }

    Serial.println(timerResultInCharArray);
    Serial.printlnf("Alive Time: %d", remainingTimeToReset);
    cb(cbArg, 0, 200, "text/plain", nullptr);
    result->write(timerResultInCharArray);
    return;
  }

  int8_t idx = 0;
  for (;;idx++) {
    Page& p = myPages[idx];
    if (!p.url) {
      idx = -1;
      break;
    }
    else if (strcmp(url, p.url)==0) {
      break;
    }
  }

  if (idx==-1) {
    cb(cbArg, 0, 404, nullptr, nullptr);
  } else {
    cb(cbArg, 0, 200, (const char *)(&myPages[idx].mime_type[0]), nullptr);
    if (myPages[idx].length == 0) {
      chunkedSender(result, (const uint8_t *)(&myPages[idx].data[0]), strlen(&myPages[idx].data[0]));
    } else {
      chunkedSender(result, (const uint8_t *)(&myPages[idx].data[0]), myPages[idx].length);
    }
  }
}

STARTUP(softap_set_application_page_handler(myPage, nullptr));

void publishDeviceCommissionInformation() {
  Serial.println("Publishing device info to Particle Cloud");
  if (jsonPayload.length() > 0) {
    bool publishEventBool = Particle.publish("alexa-config", jsonPayload, 15, PRIVATE);
    if (publishEventBool) {
      Serial.println("Success: Particle Cloud has received data.");
    } else {
      Serial.println("Error: Device commission info transmission error");
    }
  }
}


void resetWiFiOnPowerButtonLongPress() {
  venthoodFan.process();
  if (venthoodFan.wasPowerButtonLongPressed()) {
    wifiResetAndPowerCycle();
  }
}

bool wifiIsListening() {
  resetWiFiOnPowerButtonLongPress();
  return !WiFi.listening();
}

bool wifiCommissioning() {
  if (WiFi.hasCredentials()) {
    Serial.println("Connecting");
    WiFi.connect(WIFI_CONNECT_SKIP_LISTEN);
  } else {  
    Serial.println("Listening");
    WiFi.listen();
  }
  waitUntil(wifiIsListening);
  Serial.println("Connecting after listening");
  waitFor(WiFi.ready, 20000);
  if (!(WiFi.ready())) {
    Serial.println("Clearing credentials");
    Serial.println("Failure");
    WiFi.clearCredentials();
    return false;
  }
  Serial.println("Success");
  Serial.println("\n");
  return true;
}

int turnOnDevice(String args) {
  int index = args.toInt();
  char applianceArgs[MAX_ARGS];
  args.toCharArray(applianceArgs, MAX_ARGS);
  String deviceType = index ? "Fan" : "Light";
  sscanf(applianceArgs, "%d", &index);

  Serial.print("\nOn/Off\n");
  Serial.print("Argument Value: ");
  Serial.print(args);
  Serial.print("\nDevice: ");
  Serial.print(deviceType);

  //  If index is 0 or 1 for device 0 or 1
  switch (index) {
    case 0:
      venthoodLights.turnDeviceOn();
      break;
    case 1:
      venthoodFan.turnDeviceOn();
      break;

    default:
      return -1;
      break;
  }
  return 104;
}

int turnOffDevice(String args) {
  int index = args.toInt();
  char applianceArgs[MAX_ARGS];

  args.toCharArray(applianceArgs, MAX_ARGS);

  sscanf(applianceArgs, "%d", &index);

  Serial.print("\nOn/Off\n");
  Serial.print("Argument Value: ");
  Serial.print(args);
  Serial.print("\nDevice: ");
  Serial.print(index ? "Fan" : "Lights");

  //  If index is 0 or 1 for device 0 or 1
  switch (index) {
    case 0:
      venthoodLights.turnDeviceOff();
      break;
    case 1:
      venthoodFan.turnDeviceOff();
      break;

    default:
      return -1;
      break;
  }
  return 104;
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
  Serial.print("Increment or Decrement Identifier: ");
  Serial.print(incDecIdentifier);
  Serial.println();

  //  If index is 0 or 1 for device 0 or 1
  switch (index) {
    case 0:
      //  Turn on lights
      //  Value 1 is increment, Value 0 is decrement.

      //  Correct values. Dim/Brighten features are the
      //    only features enabled. By default,
      //    brightness increments or decrements by 25.

      //  SetPercentage is used for both 'dim' and 'brighten' 
      //    functions from Alexa. This is why the current light brightness
      //    is queried and incremented/decremented by one level. 
      //  When a power level percentage is provided, the given percentage
      //    will be divided by 50 to return the quotient, which defines
      //    the power level.
      powerLevel = venthoodLights.getBrightnessLevel() / 50;
      if (incDecIdentifier == 1) {
        powerLevel += (value / 50);
        Serial.println("BRIGHTENED");
      } else if (incDecIdentifier == 2) {
        powerLevel -= (value / 50);
        Serial.println("DIMMED");
      } else if (incDecIdentifier == -1) {
        powerLevel = value / 50;
        Serial.print("New Power level: ");
        Serial.println(powerLevel);
      }

      powerLevel = powerLevel <= 2 ? powerLevel : 2;
      powerLevel = powerLevel >= 0 ? powerLevel : 0;
      powerLevel = powerLevel * 50;

      venthoodLights.setBrightnessTo(powerLevel);
      venthoodLights.executeLightChanges();
      break;

    case 1:
      //  Turn on Fan
      powerLevel = venthoodFan.getCurrentFanSpeed() / 25;
      if (incDecIdentifier == 1) {
        powerLevel += (value / 25);
        Serial.println("INCREASE");
      } else if (incDecIdentifier == 2) {
        powerLevel -= (value / 25);
        Serial.println("DECREASE");
      } else if (incDecIdentifier == -1) {
        powerLevel = value / 25;
        Serial.print("New Power level: ");
        Serial.println(powerLevel);
      }

      powerLevel = powerLevel <= 4 ? powerLevel : 4;
      powerLevel = powerLevel >= 0 ? powerLevel : 0;
      powerLevel = powerLevel * 25;
      venthoodFan.setFanSpeed(powerLevel);
      venthoodFan.executeFanChanges();
      break;
  }
  return powerLevel;
}

/* -------- TEST Code -------- */
void testPublish() {
  static unsigned long publishTimer = millis();
  jsonPayload = "{\"idx\":0,\"lightDeviceName\":\"lights\",\"fanDeviceName\":\"exhaust\",\"amznEmail\":\"james@firstbuild.com\"}";
  if ((millis() - publishTimer) > 15000) {
    //  This is Spark json thing. 
    publishDeviceCommissionInformation();
    publishTimer = millis();
  }
}

void wifiReset() {
  static unsigned long wifiResetTimer = millis();
  if ((millis() - wifiResetTimer) > 30000) {
    WiFi.clearCredentials();
    WiFi.listen();
    wifiResetTimer = millis();
  }
}

void powerCycle() {
  System.reset();
}

/* -------- END of TEST Code -------- */

Timer commissionResetTimer(WIFI_CONNECTING_TIMEOUT, wifiResetAndPowerCycle, true);

void setup() {
  Serial.begin(9600);

  pinMode(CH_A, OUTPUT);
  pinMode(CH_B, OUTPUT);
  pinMode(CH_C, OUTPUT);
  pinMode(ENABLE, OUTPUT);
  pinMode(LIGHT_STATE, INPUT);
  pinMode(TASTI_READ, INPUT);

#ifdef VOICE
  System.set(SYSTEM_CONFIG_SOFTAP_PREFIX, "1B Vent Hood");
  
  // Ensure that any latent information stored for 10 minutes is cleared
  //  and reset after timeout.
  commissionTimeTracker = millis();

  while (!wifiCommissioning());
  Particle.connect();
  while (!Particle.connected());
  publishDeviceCommissionInformation();

  // When the device is hung attempting to connect with the loop, 
  //  wifi reset and power cycle.
#endif

#ifdef GESTURE
  venthoodGesture.init();
#endif

#ifdef VOICE
  Particle.function("setvalue", setPercentage);
  Particle.function("on", turnOnDevice);
  Particle.function("off", turnOffDevice);
#endif
}

void loop() {
  venthoodFan.process();
  venthoodLights.process();

  #ifdef VOICE 
  if (WiFi.connecting()) {
    commissionResetTimer.start();
  }

  if (!WiFi.listening() && venthoodFan.wasPowerButtonLongPressed()) {
    Serial.println("Power button long pressed");
    wifiResetAndPowerCycle();
  }
  #endif 
}
