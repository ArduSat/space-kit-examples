/*
 * =====================================================================================
 *
 *       Filename:  gyro_scales.ino
 *
 *    Description:  Simple example for using the Space Kit to play audio using the 
 *                  Sensor Drum Kit Example (https://demo.ardusat.com/#/sensors/drum-kit).
 *
 *                  Example plays scales of a specific instrument depending on the
 *                  gyroscope readings for rotational velocities. Negative velocities
 *                  play down the scale and positive velocities play up the scale.
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
#define NUM_X_ELEMENTS 6    // Number of sounds per instrument X
#define NUM_Y_ELEMENTS 12   // Number of sounds per instrument Y
#define NUM_Z_ELEMENTS 11   // Number of sounds per instrument Z

// See link for PROGMEM documentation:
// http://www.arduino.cc/en/Reference/PROGMEM
#define prog_char const char PROGMEM // Used to store strings in flash memory, not SRAM

// Instrument X sounds
prog_char iA1[] = "AltoSaxF4";
prog_char iA2[] = "AltoSaxE4";
prog_char iA3[] = "AltoSaxD4";
prog_char iA4[] = "AltoSaxC4";
prog_char iA5[] = "AltoSaxB4";
prog_char iA6[] = "AltoSaxAb4";

// Instrument Y sounds
prog_char iB1[] = "c1";
prog_char iB2[] = "c1s";
prog_char iB3[] = "d1";
prog_char iB4[] = "d1s";
prog_char iB5[] = "e1";
prog_char iB6[] = "f1";
prog_char iB7[] = "f1s";
prog_char iB8[] = "g1";
prog_char iB9[] = "g1s";
prog_char iB10[] = "a1";
prog_char iB11[] = "a1s";
prog_char iB12[] = "b1";

// Instrument Z sounds
prog_char iC1[] = "MusicalGlasses2";
prog_char iC2[] = "MusicalGlasses3";
prog_char iC3[] = "MusicalGlasses4";
prog_char iC4[] = "MusicalGlasses5";
prog_char iC5[] = "MusicalGlasses6";
prog_char iC6[] = "MusicalGlasses7";
prog_char iC7[] = "MusicalGlasses8";
prog_char iC8[] = "MusicalGlasses9";
prog_char iC9[] = "MusicalGlasses10";
prog_char iC10[] = "MusicalGlasses11";
prog_char iC11[] = "MusicalGlasses12";

// Combine available sounds into less verbose structure
const char* const instrx[NUM_X_ELEMENTS] PROGMEM = {iA1, iA2, iA3, iA4, iA5, iA6};
const char* const instry[NUM_Y_ELEMENTS] PROGMEM = {iB1, iB2, iB3, iB4, iB5, iB6, iB7, iB8, iB9, iB10, iB11, iB12};
const char* const instrz[NUM_Z_ELEMENTS] PROGMEM = {iC1, iC2, iC3, iC4, iC5, iC6, iC7, iC8, iC9, iC10, iC11};

// Assumed max and min gyro spin reading in radians per second
#define EXP_MAX 5.0
#define EXP_MIN -5.0


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
  beginGyroSensor();

  /* We're ready to go! */
  serialConnection.println("");
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  get_sound_index
 *   Parameters:  spin: The gyroscope reading for the magnitude of the spin
 *                instr_len: The number of available sounds per instrument
 *  Description:  This function takes the spin magnitude and the number of sounds in
 *                an instrument array and scales the spin to an index.
 * =====================================================================================
 */
int get_sound_index(float spin, int instr_len) {
  int idx;
  int len = instr_len - 1;
  float intervals;

  // Only need to calculate an index if there is more than one value in array
  if (instr_len > 1) {
    // Scale down spin reading to be within available index
    intervals = (EXP_MAX - EXP_MIN) / (float) len;
    if (intervals <= 1.0) {
      idx = (spin - EXP_MIN + 1.0) / intervals;
    } else {
      idx = (spin - EXP_MIN) / intervals;
    }
  }
    
  idx = min(idx, len);  // Make sure index is not above the array size
  idx = max(idx, 0);    // Make sure index is not below 0
  
  return idx;
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  make_sound
 *   Parameters:  instr: The array of available sounds per instrument
 *                instr_len: The number of available sounds per instrument
 *                spin: The gyroscope reading for the magnitude of the spin
 *  Description:  This function takes an instrument array and writes one of the sounds
 *                in the array to the serial connection. The sound written corresponds
 *                to the magnitude of the spin.
 * =====================================================================================
 */
void make_sound(const char* const* instr, int instr_len, float spin) {
  char buf[30];
  int idx = get_sound_index(spin, instr_len);
  
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
  gyro_t gyro;
  float max_spin_mag;
  readGyro(&gyro);

  // Get the max absolute value among the gyro readings per axis
  max_spin_mag = max(max(abs(gyro.x), abs(gyro.y)), max(abs(gyro.x), abs(gyro.z)));

  // Only care if max rotational speed is above a small amount to ignore small
  // shakes and other vibrations
  if (max_spin_mag > 0.5) {
    // Only play sounds for the axis that has the most rotational speed
    if (abs(gyro.x) == max_spin_mag) {
      make_sound(instrx, NUM_X_ELEMENTS, gyro.x);
    } else if (abs(gyro.y) == max_spin_mag) {
      make_sound(instry, NUM_Y_ELEMENTS, gyro.y);
    } else {
      make_sound(instrz, NUM_Z_ELEMENTS, gyro.z);
    }
  }
}
