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

//#undef MIME_TABLE( ENTRY ) \  ENTRY( MIME_TYPE_TLV = 0 ,                "application/x-tlv8"               ) \
//    ENTRY( MIME_TYPE_APPLE_BINARY_PLIST,      "application/x-apple-binary-plist" ) \
//    ENTRY( MIME_TYPE_APPLE_PROXY_AUTOCONFIG,  "application/x-ns-proxy-autoconfig") \
//    ENTRY( MIME_TYPE_BINARY_DATA,             "application/octet-stream"         ) \
//    ENTRY( MIME_TYPE_JAVASCRIPT,              "application/javascript"           ) \
//    ENTRY( MIME_TYPE_JSON,                    "application/json"                 ) \
//    ENTRY( MIME_TYPE_HAP_JSON,                "application/hap+json"             ) \
//    ENTRY( MIME_TYPE_HAP_PAIRING,             "application/pairing+tlv8"         ) \
//    ENTRY( MIME_TYPE_HAP_VERIFY,              "application/hap+verify"           ) \
//    ENTRY( MIME_TYPE_TEXT_HTML,               "text/html"                        ) \
//    ENTRY( MIME_TYPE_TEXT_PLAIN,              "text/plain"                       ) \
//    ENTRY( MIME_TYPE_TEXT_CSS,                "text/css"                         ) \
//    ENTRY( MIME_TYPE_IMAGE_PNG,               "image/png"                        ) \
//    ENTRY( MIME_TYPE_IMAGE_GIF,               "image/gif"                        ) \
//    ENTRY( MIME_TYPE_IMAGE_MICROSOFT,         "image/vnd.microsoft.icon"         ) \
//    ENTRY( MIME_TYPE_ALL,                     "*/*"                              ) /* This must always be the last mimne*/
//#define MIME_TABLE( ENTRY ) \
//    ENTRY( MIME_TYPE_TLV = 0 ,                "application/x-tlv8"               ) \
//    ENTRY( MIME_TYPE_APPLE_BINARY_PLIST,      "application/x-apple-binary-plist" ) \
//    ENTRY( MIME_TYPE_APPLE_PROXY_AUTOCONFIG,  "application/x-ns-proxy-autoconfig") \
//    ENTRY( MIME_TYPE_BINARY_DATA,             "application/octet-stream"         ) \
//    ENTRY( MIME_TYPE_JAVASCRIPT,              "application/javascript"           ) \
//    ENTRY( MIME_TYPE_JSON,                    "application/json"                 ) \
//    ENTRY( MIME_TYPE_HAP_JSON,                "application/hap+json"             ) \
//    ENTRY( MIME_TYPE_HAP_PAIRING,             "application/pairing+tlv8"         ) \
//    ENTRY( MIME_TYPE_HAP_VERIFY,              "application/hap+verify"           ) \
//    ENTRY( MIME_TYPE_TEXT_HTML,               "text/html"                        ) \
//    ENTRY( MIME_TYPE_TEXT_PLAIN,              "text/plain"                       ) \
//    ENTRY( MIME_TYPE_TEXT_CSS,                "text/css"                         ) \
//    ENTRY( MIME_TYPE_IMAGE_PNG,               "image/png"                        ) \
//    ENTRY( MIME_TYPE_IMAGE_GIF,               "image/gif"                        ) \
//    ENTRY( MIME_TYPE_IMAGE_MICROSOFT,         "image/vnd.microsoft.icon"         ) \
//    ENTRY( MIME_TYPE_FONT_WOFF,               "application/x-font-woff"          ) \
//    ENTRY( MIME_TYPE_IMAGE_SVG_XML,           "image/svg+xml"                    ) \
//    ENTRY( MIME_TYPE_ALL,                     "*/*"                              ) /* This must always be the last mimne*/

#pragma SPARK_NO_PREPROCESSOR
#include "Particle.h"
#include "softap_http.h"
#include "softiePage/webpage.h"

#define VENTHOOD_VERSION_MAJOR 0
#define VENTHOOD_VERSION_MINOR 3
#define VENTHOOD_VERSION_PATCH 6

SYSTEM_THREAD(ENABLED);
SYSTEM_MODE(SEMI_AUTOMATIC);

struct Page
{
  const char* url;
  const char* mime_type;
  const char* data;
};

Page myPages[] = {
  { "/index.html", "text/html", index_html },
  { "/rsa-utils/rsa.js", "application/javascript", rsa_js },
  { "/style.css", "text/css", style_css },
  { "/rsa-utils/rng.js", "application/javascript", rng_js },
  { "/rsa-utils/jsbn_2.js", "application/javascript", jsbn_2_js },
  { "/rsa-utils/jsbn_1.js", "application/javascript", jsbn_1_js },
  { "/devnam.js", "application/javascript", devnam_js },
  { "/softap.js", "application/javascript", softap_js },
  { "/rsa-utils/prng4.js", "application/javascript", prng4_js },
  { "/1BLogo.svg", "image/svg+xml", OneBLogo_svg },
  { "/FSEmeric-Regular.woff", "application/x-font-woff", NULL },
  { "/FSEmeric-Medium.woff", "application/x-font-woff", NULL },
  { nullptr }
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

Fan venthoodFan = Fan(false);
Light venthoodLights = Light(false);
Gesture venthoodGesture = Gesture(venthoodLights, venthoodFan);
bool lightMode = false;
String jsonPayload = ""; 


void myPage(const char* url, ResponseCallback* cb, void* cbArg, Reader* body, Writer* result, void* reserved){
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
    cb(cbArg, 0, 200, "text/plain", nullptr);
    result->write("{\"a\": \"1\"}");
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
    //if (idx==10) {
    //  String contentLengthHeader = "Transfer-Encoding: gzip";
    //  //contentLengthHeader.concat(String(FSEmeric_Regular_woff_len));

    //  String contentTypeHeader = "Content-Type: ";
    //  contentTypeHeader.concat(String(myPages[idx].mime_type));

    //  char buff[contentLengthHeader.length() + 1];
    //  //contentTypeHeader.toCharArray(buff, contentTypeHeader.length());
    //  contentLengthHeader.toCharArray(buff, contentLengthHeader.length() + 1);

    //  Header h(buff);
    //  cb(cbArg, 0, 200, myPages[idx].mime_type, &h);
    //} else {
      String contentTypeHeader = "Content-Type: ";
      contentTypeHeader.concat(String(myPages[idx].mime_type));
      char buff[contentTypeHeader.length() + 1];
      contentTypeHeader.toCharArray(buff, contentTypeHeader.length() + 1);
      Header h(buff);
      cb(cbArg, 0, 200, myPages[idx].mime_type, &h);
    //}

    uint16_t chpt = 0;
    uint16_t dataLengthDifference = 0;
    size_t magicNumber = 512;
    uint16_t chunksSent = 4;
    size_t fileSize = 0;
    const uint8_t delayTime = 5;
    switch (idx) {
      case 10:
        while (chpt < FSEmeric_Regular_woff_len) {
          if ((FSEmeric_Regular_woff_len - chpt) < magicNumber) {
            dataLengthDifference = FSEmeric_Regular_woff_len - chpt;
            result->write(FSEmeric_Regular_woff + chpt, dataLengthDifference); 
            chpt += dataLengthDifference;

            //Serial.print("FINAL    ");
            //Serial.println(chpt);
            DEBUG_PRINT("FINAL    ");
            DEBUG_PRINTLN(chpt);
            break;
          } else {
            result->write(FSEmeric_Regular_woff + chpt, magicNumber);
            chpt += magicNumber;
            //Serial.print("CONT  ");
            //Serial.println(chpt);
            DEBUG_PRINT("CONT  ");
            DEBUG_PRINTLN(chpt);
          }
          if (chunksSent > 0) {
            chunksSent--;
          } else { 
            HAL_Delay_Milliseconds(delayTime);
            chunksSent = 4;
          }
          result->write("\r\n");
          //delay(10);
          //HAL_Delay_Milliseconds(2);
          //wiced_rtos_delay_milliseconds(10);
        }
        break;

       // result->write(FSEmeric_Regular_woff, FSEmeric_Regular_woff_len);
        //for(uint32_t i=0;i<FSEmeric_Regular_woff_len; i++) {
        //  result->write((uint8_t)FSEmeric_Regular_woff[i]);
        //}
        //break;

      case 11:
        for(uint32_t i=0;i<FSEmeric_Medium_woff_len; i++) {
          result->write((uint8_t)FSEmeric_Medium_woff[i]);
        }
        break;

      default:
        fileSize = strlen(myPages[idx].data);
        while (chpt < fileSize) {
          if ((fileSize - chpt) < magicNumber) {
            dataLengthDifference = fileSize - chpt;
            result->write((const uint8_t*) (myPages[idx].data + chpt), dataLengthDifference); 
            chpt += dataLengthDifference;

            //Serial.print("FINAL    ");
            //Serial.println(chpt);
            DEBUG_PRINT("FINAL    ");
            DEBUG_PRINTLN(chpt);
            break;
          } else {
            result->write((const uint8_t*)(myPages[idx].data + chpt), magicNumber);
            chpt += magicNumber;
            //Serial.print("CONT  ");
            //Serial.println(chpt);
            DEBUG_PRINT("CONT  ");
            DEBUG_PRINTLN(chpt);
          }
          if (chunksSent > 0) {
            chunksSent--;
          } else { 
            HAL_Delay_Milliseconds(delayTime);
            chunksSent = 4;
          }
        }
        result->write("\r\n");

        HAL_Delay_Milliseconds(delayTime);
        break;
    }
  }
}


STARTUP(softap_set_application_page_handler(myPage, nullptr));

void wifiReset() {
  static unsigned long timer = millis();
  if ((millis() - timer) > 30000) {
    WiFi.clearCredentials();
    WiFi.listen();
    timer = millis();
  }
}

void factoryReset() {
  WiFi.clearCredentials();
  System.reset();
}

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

void testPublish() {
  static unsigned long timer = millis();
  jsonPayload = "{\"idx\":0,\"lightDeviceName\":\"lights\",\"fanDeviceName\":\"exhaust\",\"amznEmail\":\"james@firstbuild.com\"}";
  if ((millis() - timer) > 15000) {
    //  This is Spark json thing. 
    publishDeviceCommissionInformation();
    timer = millis();
  }
}

bool wifiIsListening() {
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

void setup() {
  Serial.begin(9600);

  System.set(SYSTEM_CONFIG_SOFTAP_PREFIX, "FirstBuild Vent Hood");
  System.set(SYSTEM_CONFIG_SOFTAP_SUFFIX, "RFDO");
  
  while (!wifiCommissioning());
  Particle.connect();
  while (!Particle.connected());
  publishDeviceCommissionInformation();

  pinMode(CH_A, OUTPUT);
  pinMode(CH_B, OUTPUT);
  pinMode(CH_C, OUTPUT);
  pinMode(ENABLE, OUTPUT);
  pinMode(LIGHT_STATE, INPUT);
  pinMode(TASTI_READ, INPUT);

  //venthoodGesture.init();
  //functionTest.stop();


  Particle.function("setvalue", setPercentage);
  Particle.function("on", turnOnDevice);
  Particle.function("off", turnOffDevice);
}

void loop() {
  venthoodFan.process();
  venthoodLights.process();
  static unsigned long prevTime = millis();
  if ((millis() - prevTime) > 5000) {
    Serial.println("TESTING TESTING");
    prevTime = millis();
  }

  if (WiFi.listening()) {
    Serial.println("Within listening mode within main loop. Leaving with System reset.");
    WiFi.clearCredentials();
    System.reset();
  }

  if (!WiFi.listening() && venthoodFan.wasPowerButtonLongPressed()) {
    Serial.println("Power button long pressed");
   // WiFi.clearCredentials();
   // System.reset();
  }
}
