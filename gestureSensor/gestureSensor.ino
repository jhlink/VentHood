/****************************************************************
GestureTest.ino
APDS-9960 RGB and Gesture Sensor
Shawn Hymel @ SparkFun Electronics
May 30, 2014
https://github.com/sparkfun/APDS-9960_RGB_and_Gesture_Sensor

Tests the gesture sensing abilities of the APDS-9960. Configures
APDS-9960 over I2C and waits for gesture events. Calculates the
direction of the swipe (up, down, left, right) and displays it
on a serial console. 

To perform a NEAR gesture, hold your hand
far above the sensor and move it close to the sensor (within 2
inches). Hold your hand there for at least 1 second and move IT
away.

To perform a FAR gesture, hold your hand within 2 inches of the
sensor for at least 1 second and then move it above (out of
range) of the sensor.

Hardware Connections:

IMPORTANT: The APDS-9960 can only accept 3.3V!

 Arduino Pin  APDS-9960 Board  Function

 3.3V         VCC              Power
 GND          GND              Ground
 D0           SDA              I2C Data
 D1           SCL              I2C Clock
 2            INT              Interrupt

Resources:
Include Wire.h and SparkFun_APDS-9960.h
****************************************************************/

// This #include statement was automatically added by the Particle IDE.
#include "FastLED/FastLED.h"
FASTLED_USING_NAMESPACE;

#include "SparkFun_APDS9960/SparkFun_APDS9960.h"

// Pins
#define APDS9960_INT    2 // Needs to be an interrupt pin

#define NUM_LEDS    144 
#define DATA_PIN    A3
#define BRIGHTNESS  96
#define FRAMES_PER_SECOND   120
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB

#define LED_SPEED 144
#define DETECT_END 5
#define BRIGHT_LIM 240
#define BRIGHT_NORM 90


// Constants

// Global Variables
SparkFun_APDS9960 apds = SparkFun_APDS9960();
int isr_flag = 0;
int gestureFlag = 0;
uint8_t gHue = 0;
CRGB leds[NUM_LEDS];
int against = 0;

void setup() {

  delay(3000);
  // Initialize Serial port
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

  Serial.begin(9600);
  Serial.println();
  Serial.println("--------------------------------");
  Serial.println("SparkFun APDS-9960 - GestureTest");
  Serial.println("--------------------------------");

  // Initialize interrupt service routine
  attachInterrupt(APDS9960_INT, interruptRoutine, FALLING);

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

void loop() {
  
  if( isr_flag == 1 ) {
    detachInterrupt(0);
    handleGesture();
    isr_flag = 0;
    attachInterrupt(0, interruptRoutine, FALLING);
  }
  
  switch (gestureFlag) {
      case 0:
        fadeToBlackBy( leds, NUM_LEDS, 20);
        break;
        
      case 1:
        blueUp();
        break;
        
      case 2:
        purpleDown();
        break;
        
      case 3:
        greenLeft();
        break;
        
      case 4:
        redRight();
        break;
        
      case 5:
        brightNear();
        break;
        
      case 6:
        dimFar();
        break;
        
      case 7:
        FastLED.clear();
        break;
        
      default:
        break;
  }
  
  // send the 'leds' array out to the actual LED strip
  FastLED.show();  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND); 

  // do some periodic updates
  EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
}

void interruptRoutine() {
  isr_flag = 1;
}

void handleGesture() {
    if ( apds.isGestureAvailable() ) {

        switch ( apds.readGesture() ) {
          case DIR_UP:
            Serial.println("UP");
            against = beat16(LED_SPEED);
            gestureFlag = 1;
            break;
          case DIR_DOWN:
            Serial.println("DOWN");
            against = beat16(LED_SPEED);
            gestureFlag = 2;
            break;
            
          case DIR_LEFT:
            Serial.println("LEFT");
            against = beat16(LED_SPEED);
            gestureFlag = 3; 
            break;
            
          case DIR_RIGHT:
            Serial.println("RIGHT");
            against = beat16(LED_SPEED);
            gestureFlag = 4; 
            break;
            
          case DIR_NEAR:
            Serial.println("NEAR");
            gestureFlag = 5;
            for(int x = 0; x < NUM_LEDS; x++){
                leds[x].setColorCode(CRGB::White);
            }
            FastLED.setBrightness(0);
            against = beat16(LED_SPEED);
            break;
            
          case DIR_FAR:
            Serial.println("FAR");
            gestureFlag = 6; 
            for(int x = 0; x < NUM_LEDS; x++){
                leds[x].setColorCode(CRGB::White);
            }
            FastLED.setBrightness(BRIGHT_LIM);
            against = beat16(LED_SPEED);
            break;
            
          default:
            Serial.println("NONE");
            gestureFlag = 7; 
            against = 0;
            break;
            
        }
  }
}

void greenLeft() {
	  fadeToBlackBy( leds, NUM_LEDS, 20);
    //   int pos = beatsin16(50, 0, NUM_LEDS, against);
      int pos = scale16(beat16(LED_SPEED) - against, NUM_LEDS );
      
      pos = (NUM_LEDS - pos) > 0 ? NUM_LEDS - pos - 1 : 0;
      leds[pos] += CHSV( HUE_GREEN, 255, 192);
      if (pos <= DETECT_END) {
        gestureFlag = 0;
      }
}

void redRight() {
	  fadeToBlackBy( leds, NUM_LEDS, 20);
      int pos = scale16(beat16(LED_SPEED) - against, NUM_LEDS );
      
      pos = (NUM_LEDS - pos) > 0 ? pos : 0;
      leds[pos] += CHSV( HUE_RED , 255, 192);

      if (pos >= NUM_LEDS - DETECT_END) {
        gestureFlag = 0;
      }
}

void blueUp() {
	  fadeToBlackBy( leds, NUM_LEDS, 20);
      int pos = scale16(beat16(LED_SPEED) - against, NUM_LEDS / 2 );
      pos = ((NUM_LEDS / 2) - pos) > 0 ? pos : 0;


      leds[(NUM_LEDS / 2) + pos] += CHSV( HUE_BLUE , 255, 192);
      leds[(NUM_LEDS / 2) - pos] += CHSV( HUE_BLUE , 255, 192);
      
      int top = (NUM_LEDS / 2) + pos;
      int bot = (NUM_LEDS / 2) - pos;

      if (top >= NUM_LEDS - (DETECT_END / 2) || bot <= (DETECT_END / 2)) {
        gestureFlag = 0;
      }
}


void purpleDown() {
	  fadeToBlackBy( leds, NUM_LEDS, 20);
      int pos = scale16(beat16(LED_SPEED) - against, NUM_LEDS / 2 );
      pos = ((NUM_LEDS / 2) - pos) > 0 ? pos : 0;
      
      leds[pos] += CHSV( HUE_PURPLE , 255, 192);
      leds[NUM_LEDS - pos] += CHSV( HUE_PURPLE , 255, 192);
      
      int top = NUM_LEDS - pos;
      int bot = pos;
      

      if ((bot >= ((NUM_LEDS / 2) - (DETECT_END / 2))) || (top <= ((NUM_LEDS / 2) + (DETECT_END / 2)))) {
        gestureFlag = 0;
      }
}

void brightNear() {
      int lumens = scale16(beat16(LED_SPEED) - against, BRIGHT_LIM );
      FastLED.setBrightness(lumens);
      

      if (lumens >= BRIGHT_LIM - DETECT_END) {
        FastLED.setBrightness(BRIGHT_NORM);
        FastLED.clear();
        FastLED.show();
        gestureFlag = 0;
      }
}

void dimFar() {
      int lumens = scale16(beat16(LED_SPEED) - against, BRIGHT_LIM );
      lumens = BRIGHT_LIM - lumens;
      FastLED.setBrightness(lumens);

      if (lumens <= DETECT_END) {
        FastLED.setBrightness(BRIGHT_NORM);
        FastLED.clear();
        FastLED.show();
        gestureFlag = 0;
      }
}

