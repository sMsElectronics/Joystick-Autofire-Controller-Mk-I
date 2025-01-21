/* sMs Retro Electronics
   Joystick Autofire Controller 
   Author: William Manganaro 2024
*/

#include <Arduino.h>
#include <TinyButton.h>
// #include <FastLED.h>

// pin defines
#define DIG_RATE_SEL 2
#define FIRE_OUT 0
#define FIRE_BUTTON 1

// variables
bool analog_rate_flag = false; // if this is false, then fixed rate controls are used
bool autofire_enable_flag = false; // when flag is true, autofire is enabled.
int ana_rate_in = A3;   // select the input pin for the potentiometer
int rateValue; // analog autofire rate vale scaled from 2 to 25 pulses per second
byte mode_counter = 0; // mode counter will span 0-2 that represent three spped variables. 
// init the fire rate to the lowest to 5 pulse per second
// 0 = 5 pps 1 = 10 pps 2 = 15 pps
int delay_val[] = {250,167,125,100,84,72,63,56,50,46,42,39,36,33,31,29,27,26,25,24,23,22,21,20}; // pps = 1/(2*delay_val(n)) where n= eleement of array in milliseconds
TinyButton myButton(DIG_RATE_SEL); // create the object myButton on port b.2 and name the button myButton

// functions

// read a/d potentiometer and scale the rate to 2 - 20 pps
void Read_Analog_Rate_Potentiometer() {
    rateValue = analogRead(ana_rate_in); // read a/d conv
    rateValue = map(rateValue, 0, 1023, 0, 23); // scale a/d value to a range of autorate vales from 0-1023 to 2-20 PPS
}
// Function that is triggered by a normal click
void mode_change_fixed_rates() {
    digitalWrite(FIRE_OUT, LOW); // turn test led on
  if (!analog_rate_flag) // mode_counter counts 0-1-2-0-1-2- etc if the analog rate flag is FALSE
    {
        mode_counter++;
   if (mode_counter==3)
  {
    mode_counter=0;
  }
    }
}

// Function that is triggered by a long click
// if this flag is TRUE, then rate control is input from the analog input 
// if the flag is FALSE then the control is handed to the DIG_RATE_SEL input
void analog_rate_control_enable() {
      digitalWrite(FIRE_OUT, HIGH); // turn test led off
      analog_rate_flag = !analog_rate_flag; // toggle the flag 
}


void setup() {
    myButton.attachClick(mode_change_fixed_rates); // This is the function for the normal click
    myButton.attachLongClick(analog_rate_control_enable);   // This is the function for the long click
    pinMode(5,OUTPUT); // port b pin 5 is not used but is still prudent o define a state
    digitalWrite(5, HIGH); // init the portb.5 output to logic high
    pinMode(FIRE_OUT, OUTPUT); // define the fire output signal (active low)
    digitalWrite(FIRE_OUT, HIGH); // init the fire output signal to logic high
    pinMode(FIRE_BUTTON, INPUT); // fire button input 
    pinMode(DIG_RATE_SEL, INPUT); // digital rate select input 
    PORTB |= (1 << PB1 | 1 << PB2);  //activate pull-up resistor for PB1 & PB2 inputs


}

void loop() {
 myButton.tick();
 while (digitalRead (FIRE_BUTTON)==LOW && autofire_enable_flag==false)
{
   digitalWrite(FIRE_OUT, LOW); 
   delay(25);
   digitalWrite(FIRE_OUT, HIGH); 
   delay(25);
}

}