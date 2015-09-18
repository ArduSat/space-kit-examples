/*
 * =====================================================================================
 *
 *       Filename:  g_force_test.ino
 *
 *    Description:  Measures the maximum G Force created when shaking an
 *                  accelerometer for 3 seconds and plays notes to the
 *                  Ardusat Experiment Platform drum kit
 *                  http://experiments.ardusat.com/#/sensors/drum-kit
 *
 *                  This example uses many third-party libraries available from
 *                  Adafruit (https://github.com/adafruit). These libraries are
 *                  mostly under an Apache License, Version 2.0.
 *
 *                  http://www.apache.org/licenses/LICENSE-2.0
 *
 *        Version:  1.0
 *        Created:  09/04/2015
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
const float wakeForce = 1.5;  // G-Force required to wake
const float earthGrav = 9.8;  // Gravitational force exerted by Earth
const int delayTime = 500;    // Time in milliseconds to check for wake shake
const float shakeTime = 3.0;  // Time in seconds to listen for max shake 
acceleration_t accel;
float g_force;

char wholeBuf[5] = {0};
char pointBuf[5] = {0};


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
  beginAccelerationSensor();

  /* We're ready to go! */
  serialConnection.println("");
  serialConnection.println("\nHello! Welcome to the G-Force test.\n");
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
  readAcceleration(accel);

  // Calculate the acceleration magnitude
  g_force = sqrt(accel.x * accel.x + accel.y * accel.y + accel.z * accel.z) / earthGrav;


  if (g_force > wakeForce) {
    record_max_shake();
  }

  delay(delayTime);
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  record_max_shake
 *  Description:  This function prompts the user to shake the demosat for a const
 *                number of seconds and the returns the max magnitude of their
 *                shaking during that time
 * =====================================================================================
 */
void record_max_shake(void)
{
  unsigned long startTime = 0;
  int listenTime = 1000 * shakeTime;
  float gForce = 0.0;
  float maxShake = 0.0;

  serialConnection.print("\nPlease prepare to shake the DemoSat for ");
  serialConnection.print(shakeTime);
  serialConnection.println(" seconds.");
  delay(1000);
  serialConnection.print("Ready... ");
  delay(1000);
  serialConnection.print("Set... ");
  delay(1000);
  serialConnection.println("GO!");
  startTime = millis();
    
  while ((millis() - startTime) < listenTime) {
    readAcceleration(accel);
    gForce = sqrt(accel.x * accel.x + accel.y * accel.y + accel.z * accel.z) / earthGrav;
    maxShake = max(gForce, maxShake);
  }

  serialConnection.print("Nice! I measured a maximum reading of ");
  serialConnection.print(maxShake);
  serialConnection.println(" Gs!");
  play_max_shake(maxShake);

  serialConnection.println("Just give me another shake run a new experiment!\n");
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  play_max_shake
 *  Description:  Parses out the maxShake value and prints JSON values corresponding
 *                to different sounds on the Ardusat Experiment Platform
 *    Parameter:  The magnitude of the most forceful shake
 * =====================================================================================
 */
void play_max_shake(float maxShake)
{
  memset(wholeBuf, 0, sizeof(wholeBuf));
  memset(pointBuf, 0, sizeof(pointBuf));

  char buf[6] = {0};
  dtostrf(maxShake, 0, 2, buf);
  split(buf, '.', wholeBuf, pointBuf);

  serialConnection.print(valueToJSON(wholeBuf, DATA_UNIT_NONE, 1));
  serialConnection.print(valueToJSON(pointBuf, DATA_UNIT_NONE, 1));
  serialConnection.println(valueToJSON("gs", DATA_UNIT_NONE, 1));
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  split
 *  Description:  Splits a string into two separate parts using a provided delimiter
 *    Parameter:  toSplit: The string to try split
 *                splitOn: The character used to split the string
 *                r1: The first part before the split character
 *                r2: The second part after the split character
 * =====================================================================================
 */
void split(char * toSplit, char splitOn, char * r1, char * r2) {
  int i = 0;
  int j = 0;
  int k = 0;
  bool splitFound = false;

  while (toSplit[i] != 0) {
    if (toSplit[i] == splitOn) {
      splitFound = true;
      r2[k++] = 'p';
    } else if (!splitFound) {
      r1[j++] = toSplit[i];
    } else {
      r2[k++] = toSplit[i];
    }
    i++;
  }
}
