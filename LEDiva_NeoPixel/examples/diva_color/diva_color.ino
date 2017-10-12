
#include <LEDiva_NeoPixel.h>
#include <avr/power.h>

#define PIN 0

int LEDs = 12;

LEDiva_NeoPixel strip = LEDiva_NeoPixel(LEDs, PIN, NEO_GRB + NEO_KHZ800);

int R;
int G;
int B;
int Twinkle;
int FadeR;
int FadeG;
int FadeB;
int Lux = 135;
int wait = 10;

void setup(){
     // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif
  // End of trinket special code

  strip.begin();
    strip.show(); // Initialize all pixels to 'off'

    R = 200;
    G = 255;
    B = 0;
    strip.setBrightness(Lux);
}

void loop(){
  Twinkle = 30;
      for(uint16_t i=0; i<LEDs; i++) {
        if (random(3000) < Twinkle) {
          FadeR = R;
          FadeG = G;
          FadeB = B;
        } else {
          delay(10); 
          FadeR = strip.getR(i)-(R/60);
            if(FadeR < 0) {
            FadeR = 0;
          }
          FadeG = strip.getG(i)-(G/60);
            if(FadeG < 0) {
            FadeG = 0;
          }
          FadeB = strip.getB(i)-(B/60);
            if(FadeB < 0) {
            FadeB = 0;
          }

        }
        strip.setPixelColor(i, FadeR, FadeG, FadeB);
      }
        strip.show();
}

