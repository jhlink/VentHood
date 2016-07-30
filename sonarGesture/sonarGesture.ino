#include <dmx.h>
#include <fastpin.h>
#include <fastspi.h>
#include <cpp_compat.h>
#include <pixeltypes.h>
#include <colorutils.h>
#include <chipsets.h>
#include <noise.h>
#include <colorpalettes.h>
#include <fastled_progmem.h>
#include <led_sysdefs.h>
#include <lib8tion.h>
#include <bitswap.h>
#include <hsv2rgb.h>
#include <fastspi_types.h>
#include <fastled_config.h>
#include <fastspi_dma.h>
#include <controller.h>
#include <color.h>
#include <pixelset.h>
#include <platforms.h>
#include <fastled_delay.h>
#include <fastspi_nop.h>
#include <fastspi_bitbang.h>
#include <fastspi_ref.h>
#include <FastLED.h>
#include <power_mgt.h>

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define pwPin1 3
#define DATA_PIN    2
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    144
#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120

CRGB leds[NUM_LEDS];
long sensor, mm, inches;
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

void setup() {
    delay(3000); // 3 second delay for recovery
    // tell FastLED about the LED strip configuration
    FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    
    // set master brightness control
    FastLED.setBrightness(BRIGHTNESS);
    Serial.begin(9600);
    pinMode(pwPin1, INPUT);
    pinMode(A0, OUTPUT);
}

void read_sensor() {
    sensor = pulseIn(pwPin1, HIGH, 500000);
    mm = sensor;
    inches = mm/25.4;
}

void print_range() {
    Serial.print("S1");
    Serial.print("=");
    Serial.print(mm);
    Serial.print(" ");
    Serial.println(inches);
}

void rainbow(int uptoVal) {
    // FastLED's built-in rainbow generator 
    fill_rainbow( &leds[uptoVal - 1], 144 - uptoVal, gHue, 7);  
}

void loop() {
    static long uptoLED = 0;
    static unsigned long trackTime = millis();

    if ((millis() - trackTime) > 75) {
        digitalWrite(A0, HIGH);
        read_sensor();
//        digitalWrite(A0, LOW);
        
        Serial.println(mm);
        FastLED.clear();
        uptoLED = map(mm, 900, 6500, 0, 144);
        rainbow(uptoLED);
        trackTime = millis();
    }
    
    // send the 'leds' array out to the actual LED strip
    FastLED.show();  
    // insert a delay to keep the framerate modest
    FastLED.delay(1000/FRAMES_PER_SECOND); 
    
    EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
}


