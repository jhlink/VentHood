/**
   Copyright (c) 2016 FirstBuild - All Rights Reserved

*/

#include <Adafruit_NeoPixel.h>
#include <Process.h>
#include "filter.h"

#define WATER_HEATER_VERSION "0.2.4"

#define DEBUG 1

#define DEBOUNCE_IN_MS           15
#define HOLD_TIME_IN_MS          5000
#define PARSE_TIMEOUT_IN_MS            10000
#define RESPONSE_TIMEOUT_IN_MS            10000 
#define WAIT_TIMEOUT_IN_MS		500
#define SEND_DATA_INTERVAL_IN_MS 60000

#define PIN_WATER_FLOW             2
#define PIN_WIFI_RESET_BUTTON      3
#define PIN_NEOPIXEL_STATUS        6
#define PIN_THERMOCOUPLE_TOPLEFT     A0  // cold
#define PIN_THERMOCOUPLE_TOPRIGHT    A1  // hot
#define PIN_THERMOCOUPLE_BOTTOMRIGHT A2  // bottom
#define PIN_THERMOCOUPLE_BOTTOMLEFT  A3

#define SUCCESS_REQUEST "\"ACTIVE\""
#define STORING_DATA "STORING"

//  API URL server address and endpoiendpointnt
const char *server = "dv8ecs8jrd.execute-api.us-east-1.amazonaws.com";
const char *endpoint = "prod/testlogger"; 
const float VCC_VAL = 2.56;  // Internal Voltage Ref
const char projectID[] = "he";

//  Static variables
const char deviceID[] = "test2";

Process proc;		//  Linux processor variable and charBuff for holding commands
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(1, PIN_NEOPIXEL_STATUS, NEO_GRB + NEO_KHZ800);
static unsigned long oldTime;
static int volatile NbTopsFan; //  Interrupt variables

class WaterHeaterLogger {
  protected:
    int waterVolumeInGal;			// 	Value to store volatile flow meter values
    int buttonVal;      // 	value read from button
    int buttonLast;     // 	buffered value of the button's previous state
    long btnDnTime;         // 	time the button was pressed down
    long btnUpTime;         // 	time the button was released
    bool ignoreUp;  	// 	Whether to ignore the button release because the click+hold was triggered
    bool masterAPMode;	//	Used to determine whether device is in Access Point mode
    char charBuff[256];	//	Used to store and send linux parameters
    FilterBeLp3 filterT0;
    FilterBeLp3 filterT1;
    FilterBeLp3 filterT2;
    FilterBeLp3 filterT3;


  public:
    WaterHeaterLogger(void) :
      waterVolumeInGal(0),
      buttonVal(0),
      buttonLast(0),
      //			NbTopsFan(0),
      btnDnTime(0),
      btnUpTime(0),
      ignoreUp(false),
      masterAPMode(false),
      charBuff() {};


    //  Interrupt Function to colllect data from flow sensor
    static void waterFlowCounter() {
      NbTopsFan++;
    }

    void neoSetColor(uint32_t color) {
      pixels.setPixelColor(0, color);
      pixels.show();
    }

    void resetRebootWifi() {
      bool procStatus;
      do {
        procStatus = proc.running();
        if (!procStatus)  {
          proc.runShellCommand("wifi-reset-and-reboot");
        }
        delay(50);
      } while (procStatus);
    }

    int isClient_Master_Mode() {
      //  Result returns 0, 1, 2, which are NULL, Client, and Master
      //      respectively
      int result = 0;

      //  Wait until program finishes running.
      while (proc.running());

      //  When the Arduino is connecting to WiFi,
      //      the following code pulses green.
      const char *clientChar = "Client";
      const char *apChar = "Master";
      int i = 0, j = 0;

      while (proc.available() > 0) {
        char c = proc.read();
        //    Serial.print(c);

        if (c == clientChar[i]) {
          i++;
        } else if (c == apChar[j]) {
          j++;
        }

        if (i == 6) {
          result = 1;
          break;
        } else if (j == 6) {
          result = 2;
          break;
        }
      }

      return result;
    }

    bool verifyWifi(int checkMatchMode) {
      bool wifiMatchBool = false;

      //  Determines whether mode is in Client or AP mode
      //  -- If neither, which occurs in beginning of setup,
      //      resend command to Atheros AR9331
      proc.runShellCommandAsynchronously("/usr/bin/pretty-wifi-info.lua | grep Mode");

      int wifiMode = isClient_Master_Mode();

      proc.close();

      if (checkMatchMode == wifiMode) {
        wifiMatchBool = true;
      }

      return wifiMatchBool;
    }

    float analReadToTherm(int analogReadValue) {
      //  Convert analogRead values into voltage and convert resulting
      //      voltage values into temperature (Fahren.)
      float x2 = (analogReadValue * VCC_VAL) / 1023;
      //  Following is for Model a
//      float adjustedVolt = (x2 - 1.25) / .005;

      //   Following is for Model b
      float adjustedVolt = x2 / .005;
      float fahren2 = adjustedVolt * (9 / 5.0) + 32;

      return fahren2;
    }

    void longPressButtonEvent() {
      // Ardunino Yun is setting up for AP mode.
      //  Sets LED status to Orange
      neoSetColor(pixels.Color(165, 165, 0));
      resetRebootWifi();
      masterAPMode = true;
    }

    void startDate() {
      if (!proc.running())  {
        // Requests the date in Linux Epoch Time
        char dateComm[] = "date +%s";
        proc.runShellCommand(dateComm);
      }
    }

    String parseDate()  {
      String timeString = "";
      unsigned long timeout = millis();
      while (proc.available() > 0 && ((millis() - timeout) < PARSE_TIMEOUT_IN_MS)) {
        timeString = proc.readString();
      }
      return timeString;
    }

    void send_request(String timeVal, float value1, float value2, float value3, float value4, float liters) {

      char coldInletTemp[10];
      dtostrf(value1, 4, 2, coldInletTemp);

      char hotOutletTemp[10];
      dtostrf(value2, 4, 2, hotOutletTemp);

      char bottomTemp[10];
      dtostrf(value3, 4, 2, bottomTemp);

      char miscTemp[10];
      dtostrf(value4, 4, 2, miscTemp);

      char waterVol[10];
      dtostrf(liters, 4, 2, waterVol);

      //	Serial.println(floatString);
      //	Serial.println(floatString2);
      //	Serial.println(floatString3);
      //	Serial.println(floatString4);
      //	Serial.println(floatString5);
      //	Serial.println();

      proc.begin("/root/db/dataStoreOrSend.py");

      //    The order of the following variables must NOT change.
      proc.addParameter(deviceID);
      proc.addParameter(timeVal.c_str());
      proc.addParameter(coldInletTemp);
      proc.addParameter(hotOutletTemp);
      proc.addParameter(bottomTemp);
      proc.addParameter(miscTemp);
      proc.addParameter(waterVol);
      proc.addParameter(projectID);
      
      // POST URI:
      snprintf(charBuff, 80, "https://%s/%s", server, endpoint);
      proc.addParameter(charBuff);

      // Run the command synchronously
      proc.run(); 
    }

    void read_response() {
      bool printBool = true;
      char verif[10];
      int i = 0;
      unsigned long timeout = millis();

      while (proc.available() && ((millis() - timeout) < RESPONSE_TIMEOUT_IN_MS)) {
        char c = proc.read();
        // Print only until the first carriage return

        //std::cout << "\n" << c << "\t" << proc.available() <<"\n";

        if (c == '\n') {
          printBool = false;
        }
        if (printBool) {
          Serial.print(c);
        }
        if (i < 9) {
          verif[i++] = c;
        }
      }

      //std::cout << "This is millis: " << millis() - timeout;

      //std::cout<< "\n" << verif << "\n";

      //  Verifies whether data was succesfully sent from AWS API response
      //      and sustains solid green light.
      if (strncmp(verif, SUCCESS_REQUEST, 8) == 0) {

				//	Displays solid green if server has accepted the data
        Serial.println("\nData sent.");
        neoSetColor(pixels.Color(0, 255, 0));
      } else if (strncmp(verif, STORING_DATA, 7) == 0) {
       
				//	Displays pink if data is stored locally. 
        Serial.println("\nData stored.");
        neoSetColor(pixels.Color(121, 30, 29));
      } else {

        //  Displays solid red if server does not accept data.
        //  Sets LED status to Red
        neoSetColor(pixels.Color(255, 0, 0));

        Serial.println("\nData not sent.");
        Serial.println(verif);
//        Serial.println(proc.exitValue());

      }
    }

    void wait_response() {
      unsigned int respOldTime = millis();
      while (!proc.available() && ((millis() - respOldTime) < WAIT_TIMEOUT_IN_MS)) {
        delay(100);
      }
    }

    void trackButtonStates(unsigned long inputCurrentTime) {
      // Read the state of the button
      buttonVal = digitalRead(PIN_WIFI_RESET_BUTTON);

      // Test for button pressed and store the down time
      if (buttonVal == LOW && buttonLast == HIGH && (inputCurrentTime - btnUpTime) > long(DEBOUNCE_IN_MS)) {
        btnDnTime = inputCurrentTime;
      }

      // Test for button held down for longer than the hold time
      if (buttonVal == LOW && (inputCurrentTime - btnDnTime) > long(HOLD_TIME_IN_MS)) {
        longPressButtonEvent();
        ignoreUp = true;
        btnDnTime = inputCurrentTime;
      }

      // Test for button release and store the up time
      if (buttonVal == HIGH && buttonLast == LOW && (inputCurrentTime - btnDnTime) > long(DEBOUNCE_IN_MS)) {
        if (ignoreUp) {
          ignoreUp = false;
        } else {
          //			event1();
        }
        btnUpTime = inputCurrentTime;
      }

      buttonLast = buttonVal;
    }

    void setup() {
      delay(2500);
      Serial.begin(9600);
      pixels.begin();

      // Yun starting up and set color to light blue
      neoSetColor(pixels.Color(0, 0, 150));
      Serial.println("Software Version: " WATER_HEATER_VERSION);

      Bridge.begin();

      //  Voltage to AREF is VCC_VAL
      analogReference(INTERNAL);

      //  Set button input pins
      pinMode(PIN_WATER_FLOW, INPUT);
      pinMode(PIN_THERMOCOUPLE_TOPLEFT, INPUT);
      pinMode(PIN_THERMOCOUPLE_TOPRIGHT, INPUT);
      pinMode(PIN_THERMOCOUPLE_BOTTOMRIGHT, INPUT);
      pinMode(PIN_THERMOCOUPLE_BOTTOMLEFT, INPUT);
      pinMode(PIN_WIFI_RESET_BUTTON, INPUT_PULLUP);

      Serial.println("Verifying Wifi");

      //  Delays until Wifi is connected to Mode:Client network
      while (!verifyWifi(1)) {

        Serial.println("Within Setup -> ...");

        //  Checks if Yun Wifi is in AP Mode
        if (verifyWifi(2)) {
          // If Mode:Master network is selected
          masterAPMode = true;
          Serial.println("Setup -> AP Mode identified.");
          break;
        }
      }

      //    else if
      //        proceedToConsole();

      //  When Arduino connects to wifi, display solid green
      //      at max intensity.
      if (masterAPMode) {
        Serial.println("Mode: Master AP");
        neoSetColor(pixels.Color(255, 255, 0));
      } else {
        Serial.println("Connected to Network!");
        neoSetColor(pixels.Color(160, 32, 240));
      }


      pixels.show();

      oldTime = millis();

      Serial.println("Preparing interrupt pins...");

      //  Attach interrupt behaviour to PIN_WATER_FLOW
      attachInterrupt(digitalPinToInterrupt(PIN_WATER_FLOW), waterFlowCounter, RISING);

      Serial.println("READY!");


    }

    void loop() {

      unsigned long currentTime = millis();
      //  Time interval for sending data is set via "SEND_DATA_INTERVAL_IN_MS" variable
      if ((currentTime - oldTime) > SEND_DATA_INTERVAL_IN_MS && !masterAPMode) {
        //  Initialize x1-4 for debugging.
        float x1 = 0, x2 = 0, x3 = 0, x4 = 0;

        //  Prepare temperature values
        float PIN_THERMOCOUPLE_TOPLEFT_FAHREN = filterT0.step(analReadToTherm(analogRead(PIN_THERMOCOUPLE_TOPLEFT)));
        float PIN_THERMOCOUPLE_TOPRIGHT_FAHREN = filterT1.step(analReadToTherm(analogRead(PIN_THERMOCOUPLE_TOPRIGHT)));
        float PIN_THERMOCOUPLE_BOTTOMRIGHT_FAHREN = filterT2.step(analReadToTherm(analogRead(PIN_THERMOCOUPLE_BOTTOMRIGHT)));
        float PIN_THERMOCOUPLE_BOTTOMLEFT_FAHREN = filterT3.step(analReadToTherm(analogRead(PIN_THERMOCOUPLE_BOTTOMLEFT)));

        //  Calculating water flow -- Measuring gallons during data interval
        //      and resets every time data is sent.
        noInterrupts();             //Disable interrupts
        waterVolumeInGal = NbTopsFan;    //(Pulse frequency x 60) / 7.5Q, = flow rate
        NbTopsFan = 0;              //Set NbTops to 0 ready for calculations
        interrupts();			    //Enables interrupts

#ifdef DEBUG
        //  Prepare debuggging variables -- Return voltage values of TC amplifers
        //      with respect to VCC_VAL, which is whatever voltage connected to AREF
        x1 = (analogRead(PIN_THERMOCOUPLE_TOPLEFT) * (VCC_VAL / 1023));
        x2 = (analogRead(PIN_THERMOCOUPLE_TOPRIGHT) * (VCC_VAL / 1023));
        x3 = (analogRead(PIN_THERMOCOUPLE_BOTTOMRIGHT) * (VCC_VAL / 1023));
        x4 = (analogRead(PIN_THERMOCOUPLE_BOTTOMLEFT) * (VCC_VAL / 1023));
#endif


        //  Execute date command from Linux processor
        startDate();
        String val = parseDate();
        val.trim();

#ifdef DEBUG
        Serial.print("\nThis is the date ");
        Serial.println(val);

        Serial.print("temp 1: ");
        Serial.print(PIN_THERMOCOUPLE_TOPLEFT_FAHREN);
        Serial.print("\tanalog: ");
        Serial.print(analogRead(PIN_THERMOCOUPLE_TOPLEFT));
        Serial.print("\tvolt:");
        Serial.println(x1);

        Serial.print("temp 2: ");
        Serial.print(PIN_THERMOCOUPLE_TOPRIGHT_FAHREN);
        Serial.print("\tanalog: ");
        Serial.print(analogRead(PIN_THERMOCOUPLE_TOPRIGHT));
        Serial.print("\tvolt: ");
        Serial.println(x2);

        Serial.print("temp 3: ");
        Serial.print(PIN_THERMOCOUPLE_BOTTOMRIGHT_FAHREN);
        Serial.print("\tanalog: ");
        Serial.print(analogRead(PIN_THERMOCOUPLE_BOTTOMRIGHT));
        Serial.print("\tvolt: ");
        Serial.println(x3);

        Serial.print("temp 4: ");
        Serial.print(PIN_THERMOCOUPLE_BOTTOMLEFT_FAHREN);
        Serial.print("\tanalog: ");
        Serial.print(analogRead(PIN_THERMOCOUPLE_BOTTOMLEFT));
        Serial.print("\tvolt: ");
        Serial.println(x4);

        Serial.print("Water Gal: ");
        Serial.println(waterVolumeInGal);
#endif

        //  Send HTTPS request to AWS server with variables
        send_request(val, PIN_THERMOCOUPLE_TOPLEFT_FAHREN, PIN_THERMOCOUPLE_TOPRIGHT_FAHREN, PIN_THERMOCOUPLE_BOTTOMRIGHT_FAHREN, PIN_THERMOCOUPLE_BOTTOMLEFT_FAHREN, waterVolumeInGal);
        wait_response();
        read_response();

        //  Reset time interval counter
        oldTime = currentTime;
      }

      // Checking when wifi mode of Yun is in Master Mode
      if (masterAPMode) {
        neoSetColor(pixels.Color(255, 255, 0));
        //        Serial.println("Inside Main loop");
        //  Waiting until Yun reboots.

        return;
      }
      trackButtonStates(currentTime);


      static unsigned long filterUpdateOldTime = millis();
      if ((millis() - filterUpdateOldTime) > 19)
      {
        filterUpdateOldTime = millis();
        filterT0.step(analReadToTherm(analogRead(PIN_THERMOCOUPLE_TOPLEFT)));
        filterT1.step(analReadToTherm(analogRead(PIN_THERMOCOUPLE_TOPRIGHT)));
        filterT2.step(analReadToTherm(analogRead(PIN_THERMOCOUPLE_BOTTOMRIGHT)));
        filterT3.step(analReadToTherm(analogRead(PIN_THERMOCOUPLE_BOTTOMLEFT)));
      }
    }

};
