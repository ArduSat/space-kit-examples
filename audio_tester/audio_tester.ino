/*
 * =====================================================================================
 *
 *       Filename:  audio_tester.ino
 *
 *    Description:  Simple example for using the Space Kit to play audio using the 
 *                  Sensor Drum Kit Example (https://demo.ardusat.com/#/sensors/drum-kit).
 *
 *                  Example plays the "kick" at a specified beat interval, then listens
 *                  for "shakes" of the accelerometer to play the "snare" sound.
 *
 *                  This example uses many third-party libraries available from
 *                  Adafruit (https://github.com/adafruit). These libraries are
 *                  mostly under an Apache License, Version 2.0.
 *
 *                  http://www.apache.org/licenses/LICENSE-2.0
 *
 *        Version:  1.0
 *        Created:  1/5/2015
 *
 *         Author:  Ben Peters (ben@ardusat.com)
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
 *  Constant Definitions
 *-----------------------------------------------------------------------------*/
#define BEAT 500 // interval, in ms, to wait between readings

unsigned long lastBeat = 0;
bool playedSnare = false;

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  setup
 *  Description:  This function runs when the Arduino first turns on/resets. This is 
 *                our chance to take care of all one-time configuration tasks to get
 *                the program ready to begin logging data.
 * =====================================================================================
 */
void setup() {
  Serial.begin(115200);
  Serial.println("Ardusat Drum Kit Example"); 

  beginAccelerationSensor();
  
  /* We're ready to go! */
  Serial.println("");
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  loop
 *  Description:  After setup runs, this loop function runs until the Arduino loses 
 *                power or resets. We go through and update each of the attached sensors,
 *                write out the updated values in JSON format, then delay before repeating
 *                the loop again.
 * =====================================================================================
 */
void loop() {
  acceleration_t accel;
  float accel_mag;
  unsigned long time;

  // Read Accelerometer
  readAcceleration(&accel);

  // Calculate the acceleration magnitude
  accel_mag = sqrt(accel.x * accel.x + accel.y * accel.y + accel.z * accel.z);

  // Play the snare if the total acceleration magnitude is high enough
  // We have a global variable "playedSnare" to make sure we only fire a snare 
  // command when the magnitude first goes above 12.5, not every loop for the 
  // duration of that acceleration bump
  if (accel_mag > 12.5) {
    if (playedSnare == false) {
      Serial.println(valueToJSON("snare", DATA_UNIT_NONE, 1));
    }
    playedSnare = true;
  } else {
    playedSnare = false;
  }

  // We want to listen for "shakes" all the time for the snare, so rather than 
  // use the delay() function (which waits for a specified period of time before
  // continuing the program) to lay down the "kick" beat, we use the millis() 
  // and a global variable function to keep track of when the last "kick" happened.
  if ((time = millis()) - lastBeat > BEAT) {
    Serial.println(valueToJSON("kick", DATA_UNIT_NONE, 1));
    lastBeat = time;
  }
}
