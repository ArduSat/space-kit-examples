/**
 * @file   marry_had_a_little_lamb
 * @Author Kevin Cocco (kevin@ardusat.com)
 * @date   May 18, 2015
 * @brief  Playing Mary had a Little Lamb with drum kit Ardusat experiment platform.
 * @learn  playing notes, tempo, EPdrum kit, delay
 */

#include <Arduino.h>
#include <Wire.h>
#include <ArdusatSDK.h>
#include <math.h>

ArdusatSerial serialConnection(SERIAL_MODE_HARDWARE_AND_SOFTWARE, 8,9);

/*-----------------------------------------------------------------------------
 *  Constant Definitions
 *-----------------------------------------------------------------------------*/
#define NUM_Y_ELEMENTS 12 // Number of sounds per x instrument

/* All arrays must have NUM_ELEMENTS elements */
char* instr[NUM_Y_ELEMENTS] = {"c1", "d1", "e1", "f1", "g1", "a1", "b1","c2"};  // Do Re Mi Fa So La Ti Do  -  C major scale
int quarterBeat = 500; // Milli Sec delay. BPM(Beats Per Min) 1 beat per quarter note.  120 BPM  4/4 tempo.  


void setup()
{ 
  serialConnection.begin(9600);
}

void loop()
{ 
  // Play MA-RY HAD A LIT-TLE LAMB
  Serial.println(valueToJSON(instr[2], DATA_UNIT_NONE, 1));
  delay(quarterBeat);
  Serial.println(valueToJSON(instr[1], DATA_UNIT_NONE, 1));
  delay(quarterBeat);
  Serial.println(valueToJSON(instr[0], DATA_UNIT_NONE, 1));
  delay(quarterBeat);
  Serial.println(valueToJSON(instr[1], DATA_UNIT_NONE, 1));
  delay(quarterBeat);
  Serial.println(valueToJSON(instr[2], DATA_UNIT_NONE, 1));
  delay(quarterBeat);
  Serial.println(valueToJSON(instr[2], DATA_UNIT_NONE, 1));
  delay(quarterBeat);
  Serial.println(valueToJSON(instr[2], DATA_UNIT_NONE, 1));
  delay(quarterBeat*2);
  // Play LIT-TLE LAMB, LIT-TLE LAMB
  Serial.println(valueToJSON(instr[1], DATA_UNIT_NONE, 1));
  delay(quarterBeat);
  Serial.println(valueToJSON(instr[1], DATA_UNIT_NONE, 1));
  delay(quarterBeat);
  Serial.println(valueToJSON(instr[1], DATA_UNIT_NONE, 1));
  delay(quarterBeat*2);
  Serial.println(valueToJSON(instr[2], DATA_UNIT_NONE, 1));
  delay(quarterBeat);
  Serial.println(valueToJSON(instr[2], DATA_UNIT_NONE, 1));
  delay(quarterBeat);
  Serial.println(valueToJSON(instr[2], DATA_UNIT_NONE, 1));
  delay(quarterBeat*2);  
}
