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
 *  Setup Software Serial to allow for both RF communication and USB communication
 * RX is digital pin 10 (connect to TX/DOUT of RF Device)
 * TX is digital pin 11 (connect to RX/DIN of RF Device)
 *-----------------------------------------------------------------------------*/
ArdusatSerial serialConnection(SERIAL_MODE_HARDWARE_AND_SOFTWARE, 8,9);

/*-----------------------------------------------------------------------------
 *  Constant Definitions
 *-----------------------------------------------------------------------------*/
#define BEAT 500 // 4/4 8ths interval, in ms, to wait between readings

unsigned long lastBeat = 0;
bool playedSnare = false;
float infrared_temp_last = 0;

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
  serialConnection.println("Ardusat Drum Kit Example");

  beginAccelerationSensor();
  beginInfraredTemperatureSensor();

  /* We're ready to go! */
  // Serial.println("");
  serialConnection.println("");
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
  temperature_t temp;
  float infrared_temp;

  // Read MLX Infrared temp sensor
  readInfraredTemperature(&temp);
  //serialConnection.println(temperatureToJSON("infraredTemp", &temp));
  infrared_temp = temp.t;
  if ((infrared_temp_last +4) < infrared_temp) {
    serialConnection.println(valueToJSON("snare", DATA_UNIT_NONE, 1));
  }
  infrared_temp_last = infrared_temp;

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
      if (accel.x > 8 ) {
        // Serial.println(valueToJSON("snare", DATA_UNIT_NONE, 1));
        serialConnection.println(valueToJSON("snare", DATA_UNIT_NONE, 1));
      }
      else if (accel.y > 8 ) {
        // Serial.println(valueToJSON("hihat", DATA_UNIT_NONE, 1));
        serialConnection.println(valueToJSON("hihat", DATA_UNIT_NONE, 1));
      }
      else if (accel.z > 8 ) {
        // Serial.println(valueToJSON("tom1", DATA_UNIT_NONE, 1));
        serialConnection.println(valueToJSON("tom1", DATA_UNIT_NONE, 1));
      }
      else if (accel.x < -8 ) {
        // Serial.println(valueToJSON("tom2", DATA_UNIT_NONE, 1));
        serialConnection.println(valueToJSON("tom2", DATA_UNIT_NONE, 1));
      }
      else if (accel.y < -8 ) {
        // Serial.println(valueToJSON("tom3", DATA_UNIT_NONE, 1));
        serialConnection.println(valueToJSON("tom3", DATA_UNIT_NONE, 1));
      }
      else {
        // Serial.println(valueToJSON("kick", DATA_UNIT_NONE, 1));
        serialConnection.println(valueToJSON("kick", DATA_UNIT_NONE, 1));
      }
    }
    playedSnare = true;
  }
  else {
    playedSnare = false;
  }

  // We want to listen for "shakes" all the time for the snare, so rather than
  // use the delay() function (which waits for a specified period of time before
  // continuing the program) to lay down the "kick" beat, we use the millis()
  // and a global variable function to keep track of when the last "kick" happened.
  if ((time = millis()) - lastBeat > BEAT) {
    // Serial.println(valueToJSON("kick", DATA_UNIT_NONE, 1));
    serialConnection.println(valueToJSON("kick", DATA_UNIT_NONE, 1));
    lastBeat = time;
  }
}
