//Color and Patern Mode selector for NeoPixels
//Uses C_BUTTON to sycle through 9 colors and P_BUTTON to select any of the 8 patern sequanses.
// 64 total options are avalable
//code by Richard Albritton

#include <Adafruit_NeoPixel.h>
#include <avr/power.h>
#include <EEPROM.h>

// Interupt Code start
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif
// Interupt Code end

#define PIN 0
#define Sensor 1
#define C_BUTTON 4
#define P_BUTTON 3
#define Pixels 60

Adafruit_NeoPixel strip = Adafruit_NeoPixel(Pixels, PIN, NEO_GRB + NEO_KHZ800);

int C_MODE; // Current color mode.
int P_MODE; // Current pattern mode.
int Cn = 9; //The number of Color options.
int Pn = 10; //The number of Pattern options.
int STrigger = 0; // This tells us if the sensor interupt was triggered.
int LEDs;
int R;
int G;
int B;
int Twinkle;
int FadeR;
int FadeG;
int FadeB;
long Lux;
int wait = 10;

void setup(){
     // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif
  // End of trinket special code

  pinMode(C_BUTTON, INPUT_PULLUP);
  pinMode(P_BUTTON, INPUT_PULLUP);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  LEDs = EEPROM.read(0);
  Lux = EEPROM.read(1);
  C_MODE = EEPROM.read(2);
  P_MODE = EEPROM.read(3);
  if(LEDs<0 || LEDs>Pixels){
    LEDs = 5;
    EEPROM.update(0, LEDs);
    Lux = 135;
    EEPROM.update(1, Lux);
    C_MODE = 1;
    EEPROM.update(2, C_MODE);
    P_MODE = 7;
    EEPROM.update(3, P_MODE);
  }
    if (!digitalRead(P_BUTTON)){
    colorChange(strip.Color(50, 50, 50));
    delay(100);
    LEDSsetup(1);
  }
    if (!digitalRead(C_BUTTON)){
    colorChange(strip.Color(50, 50, 50));
    delay(100);
    LUXsetup(1);
  }
  
  pinMode(Sensor,INPUT); // Interupt Code
  sbi(GIMSK,PCIE); // Turn on Pin Change interrupt
  sbi(PCMSK,PCINT1); // Which pins are affected by the interrupt

    colorMode(C_MODE);
  colorChange(strip.Color(R, G, B));
}

void loop(){
  ColorSelect(); 
  PatternSelect();
  patternMode(P_MODE);
  STrigger = 0;
}
void ColorSelect(){
  while (digitalRead(P_BUTTON) == LOW) {
    colorChange(strip.Color(R/4, G/4, B/4));
    while (digitalRead(C_BUTTON) == LOW) {
      delay(500);
      C_MODE += 1;
      if (C_MODE > Cn) {
        C_MODE = 1; 
      }
      colorMode(C_MODE);
      strip.setBrightness(Lux/2);
      colorChange(strip.Color(R, G, B));
      strip.setBrightness(Lux);
    } 
  }
  EEPROM.update(2, C_MODE);
} 
void PatternSelect(){
  while(digitalRead(C_BUTTON) == LOW) {
    while(digitalRead(P_BUTTON) == LOW) {
      delay(500);
      P_MODE += 1;
      if (P_MODE > Pn) {
        P_MODE = 1; 
      }
      patternMode(P_MODE);
    }
  }
  EEPROM.update(3, P_MODE);
} 
// Fill the dots one after the other with a color
void colorChange(uint32_t c) {
  for(uint16_t i=0; i<LEDs; i++) {
      strip.setPixelColor(i, c);
  }
  strip.show();
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c) {
  for(uint16_t i=0; i<LEDs; i++) {
      strip.setPixelColor(i, c);
      strip.show();
      
      delay(50);
  }
}

// Fill the dots one after the other with a color
void Sparkle(uint32_t c) {
  for(uint16_t i=0; i<LEDs; i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(50);
  }
}

void colorMode(uint32_t m) {
  switch (m) {
    case 1: // Red
      R = 255;
      G = 0;
      B = 1;
      break;
    case 2: // Orange
      R = 170;
      G = 85;
      B = 1;
      break;
    case 3: // Yellow
      R = 128;
      G = 128;
      B = 1;
      break;
    case 4: // Green
      R = 0;
      G = 255;
      B = 1;
      break;
    case 5: // Sky Blue
      R = 0;
      G = 128;
      B = 128;
      break;
    case 6: // Blue
      R = 0;
      G = 0;
      B = 100;
      break;
    case 7: // Violet
      R = 85;
      G = 0;
      B = 170;
      break;
    case 8: // Pink
      R = 128;
      G = 0;
      B = 128;
      break; 
    case 9: // White
      R = 85;
      G = 85;
      B = 85;
      break;     
  }
}
void patternMode(uint32_t p) {
    strip.setBrightness(Lux);
  switch (p) {
case 1: // Solid bright
      colorChange(strip.Color(R, G, B));
      break;
    case 2: // Solid dim
      colorChange(strip.Color(R/2, G/2, B/2));
      break;
    case 3: // Slow strobe
      wait = 800;
      colorChange(strip.Color(R, G, B));
      delay(wait);
      colorChange(strip.Color(0, 0, 0));
      delay(wait);
      break;
    case 4: // Fast strobe
      wait = 300;
      colorChange(strip.Color(R, G, B));
      delay(wait);
      colorChange(strip.Color(0, 0, 0));
      delay(wait);
      break;
    case 5: // Pulsate
      wait = 100;
      uint16_t i;
      for(i=0; i<7; i++) {
        if (STrigger) wait = 20;
        strip.setBrightness(0+((Lux/7)*i));
        colorChange(strip.Color(R, G, B));
        delay(wait);
      }
      for(i=0; i<7; i++) {
        if (STrigger) wait = 20;
        strip.setBrightness(Lux-((Lux/7)*i));
        colorChange(strip.Color(R, G, B));
        delay(wait);
      }
      break;
    case 6: // Tracer
    if(!digitalRead(P_BUTTON)){
      wait = 1;
    } else {
      wait = 50;
    }
    for(uint16_t i=0; i<LEDs; i++) {
      if (STrigger) wait = 10;
       strip.setPixelColor(i, strip.Color(0, 0, 0));
       strip.show();
       PatternSelect();
       if (P_MODE != p) break;
       delay(wait);
    }
    for(uint16_t i=0; i<LEDs; i++) {
      if (STrigger) wait = 10;
      strip.setPixelColor(i, strip.Color(R, G, B));
      strip.show();
      PatternSelect();
      if (P_MODE != p) break;
      delay(wait);
    } 
      break;
    case 7: // Sparkle
      Twinkle = 30;
      if (STrigger) Twinkle = 2970;
      for(uint16_t i=0; i<LEDs; i++) {
        if (random(3000) < Twinkle) {
          FadeR = R;
          FadeG = G;
          FadeB = B;
        } else {
          FadeR =(strip.getPixelColor(i) >> 16);
          delay(1);
          FadeG =(strip.getPixelColor(i) >> 8);
          delay(1);
          FadeB =(strip.getPixelColor(i));
          delay(1); 
          FadeR = FadeR-(R/30);
          if(FadeR < 0) {
            FadeR = 0;
          }
          FadeG = FadeG-(G/30);
          if(FadeG < 0) {
            FadeG = 0;
          }
          FadeB = FadeB-(B/30);
          if(FadeB < 0) {
            FadeB = 0;
          }
        }
        strip.setPixelColor(i, FadeR, FadeG, FadeB);
      }
        strip.show();
        PatternSelect();
        if (P_MODE != p) break;
        break;
    case 8: // Rainbow (This will not use the selected colors)
        uint16_t j;
        for(j=0; j<256; j++) {
          for(i=0; i<LEDs; i++) {
            strip.setPixelColor(i, Wheel(((i * 256 / LEDs) +j) & 255));
          }
          strip.show();
          PatternSelect();
          if (P_MODE != p) break;
          delay(20);
        }
      break;
  case 9: // Color Pulsate
      wait = 100;
      if (STrigger){
        C_MODE += 1;
        if (C_MODE > 10)C_MODE = 1;
        colorMode(C_MODE);
        wait = 20;
      }
      for(i=0; i<7; i++) {
        strip.setBrightness(0+((Lux/7)*i));
        colorChange(strip.Color(R, G, B));
        delay(wait);
      }
      for(i=0; i<7; i++) {
        strip.setBrightness(Lux-((Lux/7)*i));
        colorChange(strip.Color(R, G, B));
        delay(wait);
      }
      break;  
  case 10: // red/white
      wait = 100;
      //colorMode(1);
      for(i=0; i<7; i++) {
        strip.setBrightness(0+((Lux/7)*i));
        colorChange(strip.Color(R, G, B));
        delay(wait);
      }
      for(i=0; i<7; i++) {
        strip.setBrightness(Lux-((Lux/7)*i));
        colorChange(strip.Color(R, G, B));
        delay(wait);
      }
      for(i=0; i<7; i++) {
        strip.setBrightness(0+((Lux/7)*i));
        colorChange(strip.Color(33, 33, 33));
        delay(wait);
      }
      for(i=0; i<7; i++) {
        strip.setBrightness(Lux-((Lux/7)*i));
        colorChange(strip.Color(33, 33, 33));
        delay(wait);
      }
      break;  
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else if(WheelPos < 170) {
    WheelPos -= 85;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}

// Set the number of LEDs that will be used
void LEDSsetup(int l) {
  uint16_t t=0;
  while(l){
    t++;
    if (digitalRead(C_BUTTON) == LOW && LEDs < Pixels) {
        delay(250);
        LEDs += 1;
        t=0;
    } 
    if (digitalRead(P_BUTTON) == LOW && LEDs > 0) {
      delay(250);
      LEDs -= 1;
      t=0;
    } 
    for(uint16_t i=0; i<strip.numPixels(); i++) {
      if (i<LEDs){
        strip.setPixelColor(i, strip.Color(50, 50, 50));
      }else{
        strip.setPixelColor(i, strip.Color(0, 0, 0));
      }
    }
    strip.show();
    if (t>1000){
      EEPROM.update(0, LEDs);
      l=0;
    }
  }
}

// Set the number of LEDs that will be used
void LUXsetup(int l) {
  uint16_t t=0;
  while(l){
    t++;
    if (digitalRead(C_BUTTON) == LOW && Lux < 240) {
        delay(250);
        Lux += 15;
        t=0;
    } 
    if (digitalRead(P_BUTTON) == LOW && Lux > 15) {
      delay(250);
      Lux -= 15;
      t=0;
    } 
    strip.setBrightness(Lux);
    strip.show();
    if (t>1000){
      EEPROM.update(1, Lux);
      l=0;
    }
  }
}

ISR(PCINT0_vect) {
        STrigger = 1;
}
