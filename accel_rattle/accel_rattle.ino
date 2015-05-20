/*
 * =====================================================================================
 *
 *       Filename:  accel_rattle.ino
 *
 *    Description:  Simple example for using the Space Kit to play audio using the 
 *                  Sensor Drum Kit Example (https://demo.ardusat.com/#/sensors/drum-kit).
 *
 *                  Example "listens" for shakes using the accelerometer and will play
 *                  from a set of sounds depending on the state. A simple push button is
 *                  used to cycle through four states.
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
#define BEAT 300         // Time, in ms, to wait between logging
#define NUM_ELEMENTS 6   // Number of sounds per instrument
#define SWITCH 2         // Number of the pushbutton pin
#define LEDGRN 5         // Number of the LED pin
#define LEDBLU 3         // Number of the LED pin
#define LEDRED 4         // Number of the LED pin

// See link for PROGMEM documentation:
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
prog_char iB1[] = "Resonant_FM_laser2";
prog_char iB2[] = "babylaugh2";
prog_char iB3[] = "cowbell1_big";
prog_char iB4[] = "cat3";
prog_char iB5[] = "woodblock_pitched-ti";
prog_char iB6[] = "babyuhoh";

// Instrument three sounds
prog_char iC1[] = "c1";
prog_char iC2[] = "f1";
prog_char iC3[] = "d1";
prog_char iC4[] = "e1";
prog_char iC5[] = "g1";
prog_char iC6[] = "a1";

// Instrument four sounds
prog_char iD1[] = "sitar1";
prog_char iD2[] = "sitar2";
prog_char iD3[] = "sitar3";
prog_char iD4[] = "sitar4";
prog_char iD5[] = "sitar5";
prog_char iD6[] = "sitar6";

// Combine available sounds into less verbose structure
const char* const instr1[NUM_ELEMENTS] PROGMEM = {iA1, iA2, iA3, iA4, iA5, iA6};
const char* const instr2[NUM_ELEMENTS] PROGMEM = {iB1, iB2, iB3, iB4, iB5, iB6};
const char* const instr3[NUM_ELEMENTS] PROGMEM = {iC1, iC2, iC3, iC4, iC5, iC6};
const char* const instr4[NUM_ELEMENTS] PROGMEM = {iD1, iD2, iD3, iD4, iD5, iD6};

int state = 0; // Keeps track of which instrument set is being used
unsigned long last_state_change = 0;
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
  beginAccelerationSensor();

  // Initialize the LED pins as an output:
  pinMode(LEDGRN, OUTPUT);
  pinMode(LEDBLU, OUTPUT);
  pinMode(LEDRED, OUTPUT);
  
  // Initialize button switch and attach interrupt callback
  pinMode(SWITCH, INPUT_PULLUP);
  attachInterrupt(0, change_state, CHANGE); // INTERRUPT 0 is for PIN 2

  /* We're ready to go! */
  serialConnection.println("");
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  change_state
 *  Description:  This function runs when the button is pressed to cycle to the next
 *                set of sounds. It will change the state and update corresponding
 *                LED pins.
 * =====================================================================================
 */
void change_state() {
  // We only care when the button is no longer pressed down
  if (digitalRead(SWITCH) == HIGH) {
    // Prevent rapid cycling from sensitive buttons
    if (millis() - last_state_change > 250) {
      // Cycle the state through 
      state = (state + 1) % 4;
      
      // Turn off all LEDs
      digitalWrite(LEDGRN, LOW);
      digitalWrite(LEDBLU, LOW);
      digitalWrite(LEDRED, LOW);
      
      // Turn on the corresponding LED
      if (state == 1) {
        digitalWrite(LEDGRN, HIGH);
      } else if (state == 2) {
        digitalWrite(LEDBLU, HIGH);
      } else if (state == 3) {
        digitalWrite(LEDRED, HIGH);
      }
      
      last_state_change = millis();
    }
  }
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  make_sound
 *   Parameters:  instr: The array of sounds for a specific instrument
 *                idx: The index for a specific sound in the array
 *  Description:  This function takes an instrument array and an index and writes the
 *                corresponding sound to the serial connection in JSON.
 * =====================================================================================
 */
void make_sound(const char* const* instr, int idx) {
  char buf[30];

  // copy the string from flash memory to the stack
  strcpy_P(buf, (char*)pgm_read_word(&(instr[idx])));
  serialConnection.println(valueToJSON(buf, DATA_UNIT_NONE, 1));
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  get_instrument
 *  Description:  This function returns the instrument array that corresponds to the
 *                state of the instrument switch button.
 * =====================================================================================
 */
const char* const* get_instrument() {
  if (state == 0) {
    return instr1;
  } else if (state == 1) {
    return instr2;
  } else if (state == 2) {
    return instr3;
  } else {
    return instr4;
  }
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
  acceleration_t accel;
  float accel_mag;
  const char* const* instr;
    
  // Read Accelerometer
  readAcceleration(&accel);

  // Calculate the acceleration magnitude
  accel_mag = sqrt(accel.x * accel.x + accel.y * accel.y + accel.z * accel.z);

  // Play the sound if the total acceleration magnitude is high enough
  // We only want to play the sound a single time within the duration
  // of a BEAT to prevent a rapid fire playing of sounds.
  if (accel_mag > 12.5 && !played) {
    instr = get_instrument(); // Gets the instrument depending on the state

    // Plays a sound depending on the angle of the shake
    if (accel.x > 8) {
      make_sound(instr, 0);
    } else if (accel.y > 8) {
      make_sound(instr, 1);
    } else if (accel.z > 8) {
      make_sound(instr, 2);
    } else if (accel.x < -8) {
      make_sound(instr, 3);
    } else if (accel.y < -8) {
      make_sound(instr, 4);
    } else {
      make_sound(instr, 5);
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
