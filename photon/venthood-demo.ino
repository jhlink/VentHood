#define LED_PIN     D0
#define MAX_ARGS    64

// This example shows several ways to set up and use 'palettes' of colors
// with FastLED.
//
// These compact palettes provide an easy way to re-colorize your 
// animation on the fly, quickly, easily, and with low overhead.
//
// USING palettes is MUCH simpler in practice than in theory, so first just
// run this sketch, and watch the pretty lights as you then read through 
// the code.  Although this sketch has eight (or more) different color schemes,
// the entire sketch compiles down to about 6.5K on AVR.
//
// FastLED provides a few pre-configured color palettes, and makes it
// extremely easy to make up your own color schemes with palettes.
//
// Some notes on the more abstract 'theory and practice' of 
// FastLED compact palettes are at the bottom of this file.

//  Lights, Fan pins, respectively
int venthoodPins[] = {
    D5, D4
};

int venthoodDefaultValues[] = {
    0, 0
};

void setup() {
    Serial.begin(9600);
    
    int count = sizeof(venthoodPins) / sizeof(int);
    
    for(int index = 0; index<count;  ++index) {
        pinMode(venthoodPins[index], OUTPUT);
    }
    
    pinMode(D0, OUTPUT);
    pinMode(D1, OUTPUT);
    pinMode(D2, OUTPUT);
    pinMode(D3, OUTPUT);
    
    Particle.function("setvalue", setPercentage);
    Particle.function("onoff", onoffDevice);
}


void loop()
{

}

int onoffDevice(String args){
    int index = args.toInt();
    int value = 0;
    char applianceArgs[MAX_ARGS];
    int count = sizeof(venthoodPins) / sizeof(int);
    
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
//      LOGIC HIGH -> Switches Relay to NO pin
//      LOGIC LOW -> Switches Relay to NC pin
	switch (index) {
	    case 0:
	        //  Turn on lights
	        digitalWrite(venthoodPins[index], value);
	        break;
	    case 1:
	        //  Turn on Fan
	        //  When value == 1, the relay switches to NO
	        //      which is open, or turns the switch/fan off
	        //      Vice versa applies. 
	        turnFanLevelOff();
	        int invertedRelayLogic = value ? 0 : 1;
	        
	        digitalWrite(venthoodPins[index], invertedRelayLogic);
	        break;
	}
}

int setPercentage(String args) {
    int index = 0;
    int value = 0;
    char applianceArgs[MAX_ARGS];
    int count = sizeof(venthoodPins) / sizeof(int);
    
    args.toCharArray(applianceArgs, MAX_ARGS);
    
    sscanf(applianceArgs, "%d=%d", &index, &value);
    
    int powerLevel = (int) value % 5;
    
    Serial.println();
    Serial.print("Percentage");
    Serial.println();
    Serial.print("Arguments: ");
    Serial.print(args);
    Serial.println();
    Serial.print("Device: ");
    Serial.print(index ? "Fan" : "NA");
    Serial.print("Power Level: ");
    Serial.print(powerLevel);
    Serial.println();
    
    //  Turning off HIGH state of fan to activate voice
    turnFanLevelOff();
    //  Just to dissipate any leftover power.
    switch (powerLevel) {
        case 0:
            Serial.println("FAN OFF");
            digitalWrite(D0, HIGH);
            break;  
            
        case 1:
            Serial.println("FAN LEVEL 1");
            digitalWrite(D1, HIGH);
            break;
            
        case 2:
            Serial.println("FAN LEVEL 2");
            digitalWrite(D2, HIGH);
            break;
            
        case 3:
            Serial.println("FAN LEVEL 3");
            digitalWrite(D3, HIGH);
            break;
            
        case 4:
            Serial.println("FAN LEVEL 4");
            digitalWrite(D4, LOW);
            break;
    }
    return powerLevel;
}

int turnFanLevelOff() {
    digitalWrite(D0, LOW);
    delay(50);
    digitalWrite(D1, LOW);
    delay(50);
    digitalWrite(D2, LOW);
    delay(50);
    digitalWrite(D3, LOW);
    delay(100);
    digitalWrite(D4, HIGH);
    delay(50);
    return 0;
}
