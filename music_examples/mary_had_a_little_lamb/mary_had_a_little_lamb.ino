/*
 * =====================================================================================
 *
 *       Filename:  mary_had_a_little_lamb.ino
 *
 *    Description:  Play 'Mary had a Little Lamb' with the Ardusat Experiment
 *                  Platform! http://experiments.ardusat.com
 *
 *                  This example uses many third-party libraries available from
 *                  Adafruit (https://github.com/adafruit). These libraries are
 *                  mostly under an Apache License, Version 2.0.
 *
 *                  http://www.apache.org/licenses/LICENSE-2.0
 *
 *        Version:  1.0
 *        Created:  05/18/2015
 *
 *         Author:  Kevin Cocco (kevin@ardusat.com)
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

/*-----------------------------------------------------------------------------
 *  Setup Software Serial to allow for both RF communication and USB communication
 *    RX is digital pin 10 (connect to TX/DOUT of RF Device)
 *    TX is digital pin 11 (connect to RX/DIN of RF Device)
 *-----------------------------------------------------------------------------*/
ArdusatSerial serialConnection(SERIAL_MODE_HARDWARE_AND_SOFTWARE, 8, 9);

/*-----------------------------------------------------------------------------
 *  Constant Definitions
 *-----------------------------------------------------------------------------*/
const short NUM_SOUNDS = 8;  // Number of sounds

// Do Re Mi Fa So La Ti Do - C major scale
const char * instr[NUM_SOUNDS] = {"c1", "d1", "e1", "f1", "g1", "a1", "b1", "c2"};
const int quarterBeat = 500; // Milli Sec delay. BPM (Beats Per Min) 1 beat
                             // per quarter note. 120 BPM 4/4 tempo


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
  // Play MA-RY HAD A LIT-TLE LAMB
  play_note(instr[2], 1);
  play_note(instr[1], 1);
  play_note(instr[0], 1);
  play_note(instr[1], 1);
  play_note(instr[2], 1);
  play_note(instr[2], 1);
  play_note(instr[2], 2);

  // Play LIT-TLE LAMB, LIT-TLE LAMB
  play_note(instr[1], 1);
  play_note(instr[1], 1);
  play_note(instr[1], 2);
  play_note(instr[2], 1);
  play_note(instr[2], 1);
  play_note(instr[2], 2);
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  play_note
 *  Description:  This function takes an instrument sound and writes it to the serial
 *                connection in JSON and then delays for 'beats' quarterBeats.
 *   Parameters:  sound: sound for a specific instrument
 *                beats: number of beats to delay
 * =====================================================================================
 */
void play_note(const char * sound, int beats)
{
  serialConnection.println(valueToJSON(sound, DATA_UNIT_NONE, 1));
  delay(quarterBeat * beats);
}
