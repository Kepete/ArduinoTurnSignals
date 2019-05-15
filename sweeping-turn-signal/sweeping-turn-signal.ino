#include <Adafruit_NeoPixel.h>

#define LeftSignal 0     // (D3) Pin used as trigger for left turn-signal
#define RightSignal 4    // Pin used as trigger for right turn-signal
#define BrakeSignal 2  // Pin used as trigger for reverse light
#define PIN 5             // NeoPixel data pin  
#define BRIGHTNESS 255    // Full brightness is 255
#define NUM_PIXELS 40      // Total number of NeoPixels
#define WIPE_SPEED 20     // Sweep animation delay.
const int ledPin =  2;      // the number of the LED pin

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, PIN, NEO_GRBW + NEO_KHZ800);

                            // R   G   B   W
uint32_t AMBER = strip.Color(255, 130, 0, 0);
uint32_t RED   = strip.Color(255, 0, 0, 0);
uint32_t WHITE = strip.Color(255, 255, 255, 255);
uint32_t BLUE  = strip.Color(0, 0, 255, 0);
uint32_t OFF   = strip.Color(0, 0, 0, 0);

int LeftBlinkerState  = 0;
int RightBlinkerState = 0;
int BrakeLightState   = 0;
int runLightState     = 1; //defines if to got using a spread to run light or directly
int brakeState        = 0;

void setup() {
  Serial.begin(115200);
  pinMode(LeftSignal, INPUT_PULLUP); 
  pinMode(RightSignal, INPUT_PULLUP);   
  pinMode(BrakeSignal, INPUT_PULLUP);   
  strip.begin();
  strip.setBrightness(BRIGHTNESS);
  strip.show();  
  knightRider(2, WIPE_SPEED, 2, RED);  
  Serial.print("Setup done");    
}


void loop() {
  
  LeftBlinkerState = digitalRead(LeftSignal);
  RightBlinkerState = digitalRead(RightSignal);
  BrakeLightState = digitalRead(BrakeSignal);
  // if left turn
  if (LeftBlinkerState == LOW) {
    Serial.println("Left turn (pin LOW)");
    leftTurn();
    runLightState = 1; //spread to runlight
    
  } else if (RightBlinkerState == LOW) {
    rightTurn();
    Serial.println("Right turn (pin LOW)");
    runLightState = 1; //

  } else if (BrakeLightState == LOW) {
    brakeLight();
    Serial.println("Right turn (pin LOW)");
    runLightState = 0; //go directly to run light without spread
       
  } else {
    runLight();
  }

}


/**
 * Larson scan for Left turn.
 */
void leftTurn() {
  clearStrip(); //puhasta viimane seis ära
  strip.setBrightness(BRIGHTNESS);

  // Turn strip on.
  for (int16_t i = (NUM_PIXELS-1) /2 ; i > -1; i--) {
    strip.setPixelColor(i, AMBER);
    strip.show();
    delay(10);   // swipe right speed change
  }

  delay(150);

  // Turn strip off.
  for (int16_t i = NUM_PIXELS / 2; i > -1; i--) {
    strip.setPixelColor(i, OFF);
    strip.show();
    delay(10); // swipe right speed change
  }  
  
  delay(150);  // see aeglustab running lightsi algust peale suunda
}

/**
 * Larson scan for Right turn.
 */
void rightTurn() {
  clearStrip(); //puhasta viimane seis ära
  strip.setBrightness(BRIGHTNESS);
  
  // Turn strip on. 
  for (uint16_t i = NUM_PIXELS / 2; i < NUM_PIXELS; i++) {
    strip.setPixelColor(i, AMBER);
    strip.show();
    delay(10);
  }
  
  delay(150);
  
  // Turn strip off.
  for (uint16_t i = NUM_PIXELS / 2; i < NUM_PIXELS; i++) {
    strip.setPixelColor(i, OFF);
    strip.show();
    delay(10);
  }
  
  delay(150);
}


void brakeLight()
{
    strip.setBrightness(BRIGHTNESS);
    //BrakeLightState = 1;  
    all(RED);
    strip.show();
}


//RUN LIGHT
//has two versions depending on the state of runLightState
//0 = directly to run light without spread
//1 = spread and the run light
void runLight()
{
  if (runLightState == 1) {
    strip.setBrightness(BRIGHTNESS / 8);
    spread(WIPE_SPEED, RED);
    runLightState = 0;  
    all(RED); //helper function to turn all in one color
    strip.show();
  } else if (runLightState == 0) {
    strip.setBrightness(BRIGHTNESS / 8);
    all(RED);
    }
}

// Function for larson scan. 
void knightRider(uint16_t cycles, uint16_t speed, uint8_t width, uint32_t color) {
  uint32_t old_val[NUM_PIXELS]; // up to 256 lights!

  for(int i = 0; i < cycles; i++){
    for (int count = 1; count < NUM_PIXELS; count++) {
      strip.setPixelColor(count, color);
      old_val[count] = color;
      for(int x = count; x>0; x--) {
        old_val[x-1] = dimColor(old_val[x-1], width);
        strip.setPixelColor(x-1, old_val[x-1]); 
      }
      strip.show();
      delay(speed);
    }
    
    for (int count = NUM_PIXELS-1; count>=0; count--) {
      strip.setPixelColor(count, color);
      old_val[count] = color;
      for(int x = count; x<=NUM_PIXELS ;x++) {
        old_val[x-1] = dimColor(old_val[x-1], width);
        strip.setPixelColor(x+1, old_val[x+1]);
      }
      strip.show();
      delay(speed);
    }

  }
}

// Function to dim a color across a width of leds. 
uint32_t dimColor(uint32_t color, uint8_t width) {
   return (((color&0xFF0000)/width)&0xFF0000) + (((color&0x00FF00)/width)&0x00FF00) + (((color&0x0000FF)/width)&0x0000FF);
}

// Function to light LEDS from the center one at a time (spreading). 
void spread(uint16_t speed, uint32_t color) {
  clearStrip();
  
  delay(300);
  
  int center = NUM_PIXELS / 2;
  
  for (int x = 0; x < center; x++) {
    strip.setPixelColor(center + x, color); 
    strip.setPixelColor(center + (x*-1), color); 
    strip.show();
    delay(speed);
  }    
}

// Set all LEDs to off 
void clearStrip() {
  for (int i = 0; i<NUM_PIXELS; i++) {
    strip.setPixelColor(i, 0x000000); 
    strip.show();
  }
}

void all(uint32_t c) {
  for(uint16_t i=0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);  
  }
  strip.show();
}
