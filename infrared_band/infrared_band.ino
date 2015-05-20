/*
 * =====================================================================================
 *
 *       Filename:  infrared_band.ino
 *
 *    Description:  Simple example for using the Space Kit to play audio using the 
 *                  Sensor Drum Kit Example (https://demo.ardusat.com/#/sensors/drum-kit).
 *
 *                  Example plays selected sounds depending on accelerometer and
 *                  infrared thermometer readings. The accelerometer is used to
 *                  select an "instrument" and the temperature from the infrared
 *                  thermometer selects a sound from that instrument.
 *
 *                  This example uses many third-party libraries available from
 *                  Adafruit (https://github.com/adafruit). These libraries are
 *                  mostly under an Apache License, Version 2.0.
 *
 *                  http://www.apache.org/licenses/LICENSE-2.0
 *
 *        Version:  1.0
 *        Created:  5/14/2015
 *
 *         Author:  Sam Olds (sam@ardusat.com)
 *   Organization:  Ardusat
 *
 * =====================================================================================
 */

/*-----------------------------------------------------------------------------
 *  Includes
 *-----------------------------------------------------------------------------*/
#include <Arduino.h>
#include <Wire.h>
#include <ArdusatSDK.h>
#include <Math.h>

/*-----------------------------------------------------------------------------
 *  Setup Software Serial to allow for both RF communication and USB communication
 *    RX is digital pin 10 (connect to TX/DOUT of RF Device)
 *    TX is digital pin 11 (connect to RX/DIN of RF Device)
 *-----------------------------------------------------------------------------*/
ArdusatSerial serialConnection(SERIAL_MODE_HARDWARE_AND_SOFTWARE, 8, 9);

/*-----------------------------------------------------------------------------
 *  Constant Definitions
 *-----------------------------------------------------------------------------*/
#define BEAT 500         // Time, in ms, to wait between logging
#define NUM_ELEMENTS 6   // Number of sounds per instrument
#define EXP_MAX 65.0     // Assumed maximum temperature reading in Celsius
#define EXP_MIN 5.0      // Assumed minimum temperature reading in Celsius

// See link for PROGMEM documentation
// http://www.arduino.cc/en/Reference/PROGMEM
#define prog_char const char PROGMEM // Used to store strings in flash memory, not SRAM

// Instrument one sounds
prog_char iA1[] = "snare";
prog_char iA2[] = "hihat";
prog_char iA3[] = "tom1";
prog_char iA4[] = "tom2";
prog_char iA5[] = "tom3";
prog_char iA6[] = "kick";

// Instrument two sounds
prog_char iB1[] = "c1";
prog_char iB2[] = "c1s";
prog_char iB3[] = "d1";
prog_char iB4[] = "d1s";
prog_char iB5[] = "e1";
prog_char iB6[] = "f1";

// Instrument three sounds
prog_char iC1[] = "MusicalGlasses2";
prog_char iC2[] = "MusicalGlasses3";
prog_char iC3[] = "MusicalGlasses4";
prog_char iC4[] = "MusicalGlasses5";
prog_char iC5[] = "MusicalGlasses6";
prog_char iC6[] = "MusicalGlasses7";

// Instrument four sounds
prog_char iD1[] = "TubaC2";
prog_char iD2[] = "TubaD2";
prog_char iD3[] = "TubaE2";
prog_char iD4[] = "TubaA2";
prog_char iD5[] = "TubaAb2";
prog_char iD6[] = "TubaB2";

// Instrument five sounds
prog_char iE1[] = "AltoSaxF4";
prog_char iE2[] = "AltoSaxE4";
prog_char iE3[] = "AltoSaxD4";
prog_char iE4[] = "AltoSaxC4";
prog_char iE5[] = "AltoSaxB4";
prog_char iE6[] = "AltoSaxAb4";

// Combine available sounds into less verbose structure
// All arrays must have NUM_ELEMENTS elements
const char* const instr1[NUM_ELEMENTS] PROGMEM = {iA1, iA2, iA3, iA4, iA5, iA6};
const char* const instr2[NUM_ELEMENTS] PROGMEM = {iB1, iB2, iB3, iB4, iB5, iB6};
const char* const instr3[NUM_ELEMENTS] PROGMEM = {iC1, iC2, iC3, iC4, iC5, iC6};
const char* const instr4[NUM_ELEMENTS] PROGMEM = {iD1, iD2, iD3, iD4, iD5, iD6};
const char* const instr5[NUM_ELEMENTS] PROGMEM = {iE1, iE2, iE3, iE4, iE5, iE6};

unsigned long lastBeat = 0;
bool played = false;


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  setup
 *  Description:  This function runs when the Arduino first turns on/resets. This is 
 *                our chance to take care of all one-time configuration tasks to get
 *                the program ready to begin logging data.
 * =====================================================================================
 */
void setup() {
  serialConnection.begin(9600);
  beginInfraredTemperatureSensor();
  beginAccelerationSensor();

  /* We're ready to go! */
  serialConnection.println("");
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  get_sound_index
 *   Parameters:  temp: A temperature reading in Celsius
 *  Description:  This function takes a temperature value in Celsius and scales it
 *                down to, and returns, a valid index of the intrument array.
 * =====================================================================================
 */
int get_sound_index(float temp) {
  int idx;
  int len = NUM_ELEMENTS - 1;
  float intervals;

  // Scale down temperature to be within available index
  intervals = (EXP_MAX - EXP_MIN) / (float) len;
  idx = (temp - EXP_MIN) / intervals;
    
  idx = min(idx, len);  // Make sure index is not above size
  idx = max(idx, 0);    // Make sure index is not below 0
  
  return idx;
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  make_sound
 *   Parameters:  instr: The array of sounds for a specific instrument
 *                temp: A temperature reading in Celsius
 *  Description:  This function takes an instrument array and a temperature value and
 *                writes the corresponding index of the array to the serial connection
 *                in JSON.
 * =====================================================================================
 */
void make_sound(const char* const* instr, float temp) {
  char buf[30];
  int idx = get_sound_index(temp);

  // copy the string from flash memory to the stack
  strcpy_P(buf, (char*)pgm_read_word(&(instr[idx])));
  serialConnection.println(valueToJSON(buf, DATA_UNIT_NONE, 1));
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  loop
 *  Description:  After setup runs, this loop function runs until the Arduino loses 
 *                power or resets. We go through and read from each of the attached
 *                sensors, write out the corresponding sounds in JSON format, then
 *                delay before repeating the loop again.
 * =====================================================================================
 */
void loop() {
  temperature_t temp;
  acceleration_t accel;
  float infrared_temp;
  
  // Read infrared temp sensor and acceleration
  readInfraredTemperature(&temp);
  readAcceleration(&accel);
  infrared_temp = temp.t;
  
  // Only play a sound once per BEAT
  if (!played) {
    if (accel.z < -8) {
      // Infrared Sensor facing down
      make_sound(instr1, infrared_temp);
    } else if (accel.x < -8) {
      make_sound(instr2, infrared_temp);
    } else if (accel.y < -8) {
      make_sound(instr3, infrared_temp);
    } else if (accel.x > 8) {
      make_sound(instr4, infrared_temp);
    } else if (accel.y > 8) {
      make_sound(instr5, infrared_temp);
    }
    // Do nothing if the Demosat is sitting upright
    played = true;
  }

  // Slow down the playing of sounds
  if (millis() - lastBeat > BEAT) {
    // Only return temperature values if the Demosat is not upright
    if (accel.z < 8) {
      serialConnection.println(temperatureToJSON("temperature", &temp));
    }
    lastBeat = millis();
    played = false;
  }
}
