#include <LEDiva_NeoPixel.h>
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
#define Pixels 80

LEDiva_NeoPixel strip = LEDiva_NeoPixel(Pixels, PIN, NEO_GRB + NEO_KHZ800);

int C_MODE; // Current color mode.
int P_MODE; // Current pattern mode.
int Cn = 9; //The number of Color options.
int Pn = 13; //The number of Pattern options.
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
int LuxD;
int wait = 10;
int waitRel = 0;
int ran;
int jump = 0;

void setup(){
     // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif
  // End of trinket special code

 strip.begin();
  
  pinMode(C_BUTTON, INPUT_PULLUP);
  pinMode(P_BUTTON, INPUT_PULLUP);

  LEDs = EEPROM.read(0);
  Lux = EEPROM.read(1);
  C_MODE = EEPROM.read(2);
  P_MODE = EEPROM.read(3);
  if(LEDs<0 || LEDs>Pixels){
    colorChange(strip.Color(50, 0, 0));
    delay(500);
    colorChange(strip.Color(0, 50, 0));
    delay(500);
    colorChange(strip.Color(0, 0, 50));
    delay(500);
    colorChange(strip.Color(0, 0, 0));
    LEDs = 8;
    EEPROM.update(0, LEDs);
    Lux = 255;
    EEPROM.update(1, Lux);
    C_MODE = 4;
    EEPROM.update(2, C_MODE);
    P_MODE = 5;
    EEPROM.update(3, P_MODE);
  }

    
    strip.setBrightness(Lux);
    colorChange(strip.Color(0, 0, 0));
    strip.show(); // Initialize all pixels to 'off'
  
    if (!digitalRead(P_BUTTON)){
    colorChange(strip.Color(50, 50, 50));
    delay(100);
    LEDSsetup(1);
  }
    if (!digitalRead(C_BUTTON)){
    colorChange(strip.Color(255, 255, 255));
    delay(100);
    LUXsetup(1);
  }
  
  pinMode(Sensor,INPUT); // Interupt Code
  sbi(GIMSK,PCIE); // Turn on Pin Change interrupt
  sbi(PCMSK,PCINT1); // Which pins are affected by the interrupt

    colorMode(C_MODE);
  colorChange(strip.Color(R, G, B));
  LuxD = 255 - Lux;

        FadeR = R;
      FadeG = G;
      FadeB = B;
}

void loop(){
  ColorSelect(); 
  PatternSelect();
  patternMode(P_MODE);

}


void colorMode(uint32_t m) {
  switch (m) {
    case 1: // Red
      R = 255;
      G = 0;
      B = 0;
      break;
    case 2: // Orange
      R = 255;
      G = 128;
      B = 0;
      break;
    case 3: // Yellow
      R = 255;
      G = 255;
      B = 0;
      break;
    case 4: // Green
      R = 0;
      G = 255;
      B = 0;
      break;
    case 5: // Sky Blue
      R = 0;
      G = 255;
      B = 255;
      break;
    case 6: // Blue
      R = 0;
      G = 0;
      B = 255;
      break;  
    case 7: // Violet
      R = 80;
      G = 0;
      B = 200;
      break;
    case 8: // Pink
      R = 255;
      G = 0;
      B = 255;
      break; 
    case 9: // White
      R = 255;
      G = 255;
      B = 255;
      break; 
  }
}
void patternMode(uint32_t p) {
  switch (p) {
    case 1: // Solid bright
        colorChange(strip.Color(R, G, B));
        PatternSelect();
        ColorSelect();
        break;
    case 2: // Fast strobe
      colorChange(strip.Color(R, G, B));
      delay(300);
      PatternSelect();
      ColorSelect();
      colorChange(strip.Color(0, 0, 0));
      delay(300);
      PatternSelect();
      ColorSelect();
      break;
    case 3: // Tracer
      if (P_MODE != p) break;
      colorWipe(strip.Color(0, 0, 0),20);
      if (P_MODE != p) break;
      colorWipe(strip.Color(R, G, B),20);
      break;
    case 4: // Step Pulsate
      wait = 50;
      if (STrigger){
        wait = 20;
        STrigger = 0;
      }
      FadeCycle(R, G, B, wait);
      PatternSelect();
      ColorSelect();
      break;
    case 5: // Arc Reactor
          ran = random(40);
      if(ran>20){ 
        if(FadeR<=(R-2))FadeR += 2;
        if(FadeG<=(G-2))FadeG += 2;
        if(FadeB<=(B-2))FadeB += 2;

      } else {  // Fade down
          if(FadeR>=20)FadeR -= 2;
          if(FadeG>=20)FadeG -= 2;
          if(FadeB>=20)FadeB -= 2;
      }
      //if(FadeR>=0 && FadeG>=0 && FadeB>=0 && FadeR<=R && FadeG<=G && FadeB<=B)
      colorChange(strip.Color(FadeR, FadeG, FadeB));
      delay(10);
      PatternSelect();
      ColorSelect();
           break;
    case 6: // Pulsate
      wait = 20;
      if (STrigger){
        FadeCycle(R, G, B, wait);
        STrigger = 0;
      }
      PatternSelect();
      ColorSelect();
      break;
    case 7: // Arc Reactor movment fade
          ran = random(40);
      if (STrigger){
//        colorChange(strip.Color(R, G, B));
        if(FadeR<=(R-10))FadeR += 10;
        if(FadeG<=(G-10))FadeG += 10;
        if(FadeB<=(B-10))FadeB += 10;
               STrigger = 0;
       //ran = 30;
      }
//      FadeR = strip.getR(0);
//      FadeG = strip.getG(0);
//      FadeB = strip.getB(0);
      if(ran>20){ 
        if(FadeR<=(R-2))FadeR += 2;
        if(FadeG<=(G-2))FadeG += 2;
        if(FadeB<=(B-2))FadeB += 2;

      } else {  // Fade down
          if(FadeR>=20)FadeR -= 2;
          if(FadeG>=20)FadeG -= 2;
          if(FadeB>=20)FadeB -= 2;
      }
      //if(FadeR>=0 && FadeG>=0 && FadeB>=0 && FadeR<=R && FadeG<=G && FadeB<=B)
      colorChange(strip.Color(FadeR, FadeG, FadeB));
      delay(10);
      PatternSelect();
      ColorSelect();
           break;
   case 8: // Sparkle
      if (STrigger){
        colorChange(strip.Color(R, G, B));
          STrigger = 0;
      }
      // Sparkle(TwinkleLikelyhood, StepsToTake, LowerBrightness, UpperBrightness)
      Sparkle(100, 20, 0, 1); 
      PatternSelect();
      ColorSelect();
           break;
case 9: // Rainbow (This will not use the selected colors)
        uint16_t j;
        uint16_t i;
        for(j=0; j<256; j++) {
          for(i=0; i<80; i++) {
            strip.setPixelColor(i, Wheel(((i * 256 / 255) +j) & 255));
          }
          strip.show();
          PatternSelect();
          delay(20);
          if (P_MODE != p) break;
        }
      break;
   case 10: // Rave Mode
      wait = 50;
      if (STrigger){
        ran = random(8);
        if(C_MODE==ran){
          ran++;
          if(ran>8)ran=1;
        }
        colorMode(ran);
        wait = 20;
        STrigger = 0;
      }
      FadeCycle(R, G, B, wait);
      PatternSelect();
      ColorSelect();
      break;  
    case 11: // Rave Sparkle
      if (STrigger){
              ran = random(8);
        if(C_MODE==ran){
          ran++;
          if(ran>8)ran=1;
        }
        colorMode(ran);
        colorChange(strip.Color(R, G, B));
        wait = 50;
          STrigger = 0;
      }
      // Sparkle(TwinkleLikelyhood, StepsToTake, LowerBrightness, UpperBrightness)
      Sparkle(100, 20, 0, 1); 
      PatternSelect();
      ColorSelect();
           break; 
case 12: // Rave Sparkle
      if (STrigger){
        colorChange(strip.Color(R, G, B));
        wait = 1000;
          STrigger = 0;
      }
      // Sparkle(TwinkleLikelyhood, StepsToTake, LowerBrightness, UpperBrightness)
      ran = random(8);
        if(C_MODE==ran){
          ran++;
          if(ran>8)ran=1;
        }
        colorMode(ran);
      Sparkle(100, 20, 0, 1); 
      PatternSelect();
      ColorSelect();
           break; 
  case 13: // Light saber mode
    wait = 30;
        PatternSelect();
    delay(5);
    if(STrigger){
      for(uint16_t i=LEDs; i>0; i--) {
         strip.setPixelColor(i, strip.Color(0, 0, 0));
         strip.show();
         PatternSelect();
         //if (P_MODE != p) break;
         delay(wait);
      }
      for(uint16_t i=0; i<LEDs; i++) {
        strip.setPixelColor(i, strip.Color(R, G, B));
        strip.show();
        PatternSelect();
        //if (P_MODE != p) break;
        delay(wait);
      } 
    }
    STrigger = 0;
      break;
}
}

// Fill the dots one after the other with a color
void colorChange(uint32_t c) {
  for(uint16_t i=0; i<LEDs; i++) {
      strip.setPixelColor(i, c);
  }
  strip.show();
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint16_t w) {
  for(uint16_t i=0; i<LEDs; i++) {
      if (STrigger){ 
          w = w/2;
          STrigger = 0;
      }
      strip.setPixelColor(i, c);
      strip.show();
      PatternSelect();
      ColorSelect();
      delay(w);
  }
}
void FadeUp(uint16_t r, uint16_t g, uint16_t b, uint16_t s){
  // r = Red Value
  // G = Green Value
  // B = Blue Value
  // s = the number of steps to the bace color
  for(uint16_t i=0; i<s; i++) {
    FadeR = 0+((r/s)*i);
    FadeG = 0+((g/s)*i);
    FadeB = 0+((b/s)*i);
    colorChange(strip.Color(FadeR, FadeG, FadeB));
    PatternSelect();
    ColorSelect();
    delay(10);
  }
}
void FadeDown(uint16_t r, uint16_t g, uint16_t b, uint16_t s){
  // r = Red Value
  // G = Green Value
  // B = Blue Value
  // s = the number of steps to the bace color
  for(uint16_t i=0; i<s; i++) {
    FadeR = r-((r/s)*i);
    FadeG = g-((g/s)*i);
    FadeB = b-((b/s)*i);
    colorChange(strip.Color(FadeR, FadeG, FadeB));
    PatternSelect();
    ColorSelect();
    delay(10);
  }
}
void FadeCycle(uint16_t r, uint16_t g, uint16_t b, uint16_t s){
  // r = Red Value
  // G = Green Value
  // B = Blue Value
  // s = the number of steps to the bace color
  FadeUp(r, g, b, s);
  FadeDown(r, g, b, s);
}

void Sparkle(uint16_t t, uint16_t s, uint16_t l, uint16_t d){
  // t = How likly is it that a light will Twinkle (0 - 3000)
  // s = the number of steps to the bace color
  // l = the lowest number to reach
  // d = Devide the origonal color value by x. This can be used to temporarily start with a dimmer color.
  for(uint16_t i=0; i<Pixels; i++) {
    if (random(3000) < t) {
      FadeR = R/d;
      FadeG = G/d;
      FadeB = B/d;
    } else {  
      //l=l-Lux;
      FadeR = strip.getR(i)-(R/s);
      FadeG = strip.getG(i)-(G/s);
      FadeB = strip.getB(i)-(B/s);
      if(FadeR < 0) FadeR = 0;
      if(FadeG < 0) FadeG = 0;
      if(FadeB < 0) FadeB = 0;
      if(FadeR < l && R>0) FadeR = l;
      if(FadeG < l && G>0) FadeG = l;
      if(FadeB < l && B>0) FadeB = l;
    }
    strip.setPixelColor(i, FadeR, FadeG, FadeB);
  }
    strip.show();
    delay(waitRel); 
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

void ColorSelect(){
  while (digitalRead(C_BUTTON) == LOW) {
    if(jump){
      colorChange(strip.Color(R/4, G/4, B/4));
      while (digitalRead(P_BUTTON) == LOW) {
        delay(500);
        C_MODE += 1;
        if (C_MODE > Cn) {
          C_MODE = 1; 
        }
        colorMode(C_MODE);
        //strip.setBrightness(Lux/2);
        colorChange(strip.Color(R, G, B));
        strip.setBrightness(Lux);
      }
      EEPROM.update(2, C_MODE);
    } else {
        delay(500);
        C_MODE += 1;
        if (C_MODE > Cn) {
          C_MODE = 1; 
        }
        colorMode(C_MODE);
        colorChange(strip.Color(R, G, B));
        strip.setBrightness(Lux);
    }
    if (digitalRead(C_BUTTON) == HIGH) EEPROM.update(2, C_MODE);
  }
  
} 
void PatternSelect(){
  while(digitalRead(P_BUTTON) == LOW) {
    if(jump){
      strip.setBrightness(Lux);
      while(digitalRead(C_BUTTON) == LOW) {
        strip.setBrightness(Lux);
        delay(500);
        P_MODE += 1;
        if (P_MODE > Pn) {
          P_MODE = 1; 
        }
        colorChange(strip.Color(120, 120, 120));
        delay(250);
        colorChange(strip.Color(0, 0, 0));
        patternMode(P_MODE);
      }
      EEPROM.update(3, P_MODE);
    }else{
        strip.setBrightness(Lux);
        delay(500);
        P_MODE += 1;
        if (P_MODE > Pn) {
          P_MODE = 1; 
        }
        colorChange(strip.Color(120, 120, 120));
        delay(250);
        colorChange(strip.Color(0, 0, 0));
        patternMode(P_MODE);
    }
     if (digitalRead(P_BUTTON) == HIGH) EEPROM.update(3, P_MODE);
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
    if (digitalRead(P_BUTTON) == LOW && LEDs > 1) {
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
    if (digitalRead(P_BUTTON) == LOW && Lux < 239) {
        delay(250);
        Lux += 5;
        t=0;
    } 
    if (digitalRead(C_BUTTON) == LOW && Lux > 19) {
      delay(250);
      Lux -= 5;
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
