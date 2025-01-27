/*  ; MIT License
; 
; Copyright (c) 2025 William Manganaro sMs Retro Electronics
; 
; Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
; and associated documentation files (the "Software"), to deal in the Software without 
; restriction, including without limitation the rights to use, copy, modify, merge, publish, 
; distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom 
; the Software is furnished to do so, subject to the following conditions:
; 
; The above copyright notice and this permission notice shall be included in all copies or 
; substantial portions of the Software.
; 
; THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING 
; BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
; NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
; DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
; OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <Arduino.h>
#include <TinyButton.h>
#include <FastLED.h>

// pin defines
#define DIG_RATE_SEL 2
#define FIRE_OUT 0
#define FIRE_BUTTON 1
#define DATA_PIN 4
#define LED_TYPE    WS2812
#define COLOR_ORDER RGB
#define NUM_LEDS    1
#define BRIGHTNESS  128 // default brightness
CRGB leds[NUM_LEDS];

// variables
bool analog_rate_flag = false; // if this is false, then fixed rate controls are used
bool autofire_enable_flag = true; // when flag is true, autofire is enabled.
bool was_in_analog_mode = false; // this tells the process that the joystick was in analog more and needs the leds to revert back to original color
int ana_rate_in = A3;   // select the input pin for the potentiometer
int rateValue; // analog autofire rate value scaled from 2 to 25 pulses per second
int fixed_delay; // represents the delay used if in autorate and fixed modes 5-10-20 PPS 
byte fade_loop_divider = 4;  
byte brightness_level = 0;
signed int n=1; 
byte mode_counter = 3; // mode counter will span 0-3 that represent three spped variables and disable. 
// init the fire rate to the lowest to 5 pulse per second
// 0 = 5 pps 1 = 10 pps 2 = 15 pps 3 = disabled, no autofire
int delay_val[] = {250,167,125,100,84,72,63,56,50,46,42,39,36,33,31,29,27,26,25,24,23,22,21,20}; // pps = 1/(2*delay_val(n)) where n= eleement of array in milliseconds

// object definitions
TinyButton myButton(DIG_RATE_SEL); // create the object myButton on port b.2 and name the button myButton

// functions


void set_led_color () 

{
  FastLED.setBrightness(128);
  if (mode_counter==0 ) {leds[0] = CRGB::Blue; //glow 1st led as blue, the slowest auto fire rate
    FastLED.show(); 
    fixed_delay=100;}// apply the function on led strip}
    if (mode_counter==1 ) {leds[0] = CRGB::Green; //glow 1st led as green, the slowest auto fire rate
    FastLED.show();
    fixed_delay=50; }// apply the function on led strip}
    if (mode_counter==2 ) {leds[0] = CRGB::Red; //glow 1st led as red, the slowest auto fire rate
    FastLED.show();
    fixed_delay=25; }// apply the function on led strip}
    if (mode_counter==3 ) {leds[0] = CRGB::White; //glow 1st led as white, the slowest auto fire rate
    FastLED.show(); }// apply the function on led strip}}
}

// read a/d potentiometer and scale the rate to 2 - 20 pps
void read_potentiometer() {
    rateValue = map(analogRead(ana_rate_in), 0, 1023, 0, 23); // scale a/d value to a range of autorate vales from 0-1023 to 2-20 PPS
    
}
// Function that is triggered by a normal click
void mode_change_fixed_rates()
{           
            mode_counter++;
            set_led_color();
            if (mode_counter>3) {
              mode_counter=0;
            set_led_color();
              }
}


// Function that is triggered by a long click
// if this flag is TRUE, then rate control is input from the analog input 
// if the flag is FALSE then the control is handed to the DIG_RATE_SEL input
void analog_rate_control_enable()
{
        analog_rate_flag = !analog_rate_flag; // toggle the flag
        if (analog_rate_flag== false && was_in_analog_mode == true) {
            set_led_color();
                }
}


void setup() {
    pinMode(5,OUTPUT); // port b pin 5 is not used but is still prudent o define a state
    digitalWrite(5, HIGH); // init the portb.5 output to logic high
    pinMode(FIRE_OUT, OUTPUT); // define the fire output signal (active low)
    digitalWrite(FIRE_OUT, HIGH); // init the fire output signal to logic high
    pinMode(FIRE_BUTTON, INPUT); // fire button input 
    pinMode(DIG_RATE_SEL, INPUT); // digital rate select input 
    PORTB |= (1 << PB1 | 1 << PB2);  //activate pull-up resistor for PB1 & PB2 inputs
    ADCSRA|= (1 << ADPS0 | 1 << ADPS2); // A/D clk div 32 125 ksps conversion time = 102 uS 
    myButton.attachClick(mode_change_fixed_rates); // This is the function for the normal click
    myButton.attachLongClick(analog_rate_control_enable);   // This is the function for the long click
    FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS);
    FastLED.setBrightness(BRIGHTNESS);
    leds[0] = CRGB::White; //glow 1st led as white, no autofire enabled
    FastLED.show(); // apply the function on led strip
}


void loop() {
 myButton.tick();
 while (digitalRead(FIRE_BUTTON)==0)
 {
  if (autofire_enable_flag==true && analog_rate_flag == false && mode_counter<3) // are we in auto fire mode and not in analog mode, then strobe the fire scontrol signal at fixed rate
  {
   digitalWrite(FIRE_OUT, LOW); // fire
   delay(fixed_delay);
   digitalWrite(FIRE_OUT, HIGH); // release
   delay(fixed_delay);
  }
  else 
  { digitalWrite(FIRE_OUT, LOW);  } // fire control ignal ust follows the fire button

if (autofire_enable_flag==true && analog_rate_flag == true && mode_counter<3) // are we in auto fire mode and not in analog mode, then strobe the fire scontrol signal at fixed rate
  {
   read_potentiometer();
   digitalWrite(FIRE_OUT, LOW); // fire
   delay(delay_val[rateValue]);
   digitalWrite(FIRE_OUT, HIGH); // release
   delay(delay_val [rateValue]);
  }
  else 
  { digitalWrite(FIRE_OUT, LOW);  } // fire control ignal ust follows the fire button

     }
   digitalWrite(FIRE_OUT, HIGH); // place fire control signal in inactive state

//   while (analog_rate_flag == true && loop_count==0)
   if (analog_rate_flag == true)
   {
     was_in_analog_mode= true;
     FastLED.setBrightness(brightness_level);
     leds[0] = CRGB::Purple; //glow 1st led as white, no autofire enabled
     FastLED.show(); // apply the color to led
     fade_loop_divider++;
     if (fade_loop_divider==3){
           brightness_level+=n;
     if (brightness_level==200) {n*=-1;}
     if (brightness_level==0) {n*=-1;}
     fade_loop_divider=0;
     }
      }


   }
