/*
 * =====================================================================================
 *
 *       Filename:  accel_rattle.ino
 *
 *    Description:  Simple example for using the Space Kit to play audio using the 
 *                  Sensor Drum Kit Example
 *                  http://experiments.ardusat.com/#/sensors/drum-kit
 *
 *                  Example "listens" for shakes using the accelerometer and will play
 *                  from a set of sounds.
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
const int BEAT = 300;           // Time, in ms, to wait between logging
const short NUM_ELEMENTS = 6;   // Number of sounds per instrument

// Make sure to have all but one of the following instruments commented out (using "//" at the beginning)
const char* instr[NUM_ELEMENTS] = {"snare", "hihat", "tom1", "tom2", "tom3", "kick"};
//const char* instr[NUM_ELEMENTS] = {"cow", "duck1", "cat3", "dog1", "bird2", "chicken"};
//const char* instr[NUM_ELEMENTS] = {"laser1", "laser2", "cowbell1", "cowbell2", "babyha1", "babyha2"};
//const char* instr[NUM_ELEMENTS] = {"zelda1", "zelda2", "zelda3", "zelda4", "zelda5", "zelda1"};

unsigned long lastBeat = 0;
bool played = false;
Acceleration accel;
float accel_mag;


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  setup
 *  Description:  This function runs when the Arduino first turns on/resets. This is 
 *                our chance to take care of all one-time configuration tasks to get
 *                the program ready to begin logging data.
 * =====================================================================================
 */
void setup(void)
{
  serialConnection.begin(9600);
  accel.begin();

  /* We're ready to go! */
  serialConnection.println("");
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
void loop(void)
{
  // Read Accelerometer
  accel.read();

  // Calculate the acceleration magnitude
  accel_mag = sqrt(accel.x * accel.x + accel.y * accel.y + accel.z * accel.z);

  // Play the sound if the total acceleration magnitude is high enough
  // We only want to play the sound a single time within the duration
  // of a BEAT to prevent a rapid fire playing of sounds.
  if (accel_mag > 12.5 && !played) {
    // Plays a sound depending on the angle of the shake
    if (accel.x > 8) {
      make_sound(instr[0]);
    } else if (accel.y > 8) {
      make_sound(instr[1]);
    } else if (accel.z > 8) {
      make_sound(instr[2]);
    } else if (accel.x < -8) {
      make_sound(instr[3]);
    } else if (accel.y < -8) {
      make_sound(instr[4]);
    } else {
      make_sound(instr[5]);
    }
    
    played = true;
  }
  
  // The "lastBeat" and "played" global variables helps to only play a sound
  // once during a BEAT.
  if (millis() - lastBeat > BEAT) {
    lastBeat = millis();
    played = false;
  }
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  make_sound
 *  Description:  This function takes an instrument sound and writes it to the serial
 *                connection in JSON.
 *   Parameters:  sound: sound for a specific instrument
 * =====================================================================================
 */
void make_sound(const char * sound)
{
  serialConnection.println(valueToJSON(sound, DATA_UNIT_NONE, 1));
}
