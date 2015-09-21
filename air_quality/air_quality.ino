/*
 * =====================================================================================
 *
 *       Filename:  air_quality.ino
 *
 *    Description:  Example driver code for measuring and logging air quality data
 *                  with PPD60 sensor. Calibration of PPD60 provided by Many Labs.
 *
 *                  Use the Ardusat Experiment Platform to visualize your data!
 *                  http://experiments.ardusat.com
 *
 *                  This example uses many third-party libraries available from
 *                  Adafruit (https://github.com/adafruit). These libraries are
 *                  mostly under an Apache License, Version 2.0.
 *
 *                  http://www.apache.org/licenses/LICENSE-2.0
 *
 *        Version:  1.0
 *        Created:  02/09/2015
 *       Revision:  none
 *       Compiler:  Arduino
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
#include <ArdusatLogging.h> // Necessary for SD card logging

/*-----------------------------------------------------------------------------
 *  Setup Software Serial to allow for both RF communication and USB communication
 *    RX is digital pin 10 (connect to TX/DOUT of RF Device)
 *    TX is digital pin 11 (connect to RX/DIN of RF Device)
 *-----------------------------------------------------------------------------*/
ArdusatSerial serialConnection(SERIAL_MODE_HARDWARE_AND_SOFTWARE, 8, 9);

/*-----------------------------------------------------------------------------
 *  Constant Definitions
 *-----------------------------------------------------------------------------*/
// Note interrupts don't match DIO pins on Arduino
// See http://arduino.cc/en/Reference/attachInterrupt
const short PULSE_PIN = 2;
const short INTERRUPT = 0; // Interrupt 0 is DIO pin 2

// Time to count pulses before calculating a value
const int AVERAGING_MS = 60000; // 1 minute

// Data will be saved on SD card in sequential files:
// /DATA/AIR_Q0.CSV, /DATA/AIR_Q1.CSV, /DATA/AIR_Q2.CSV...
static char LOG_FILE_PREFIX[] = "AIR_Q";

/*-----------------------------------------------------------------------------
 *  Variable Definitions
 *-----------------------------------------------------------------------------*/
// Used to keep track of the last time we calculated a value from the pulses
// This enables us to keep track of the averaging "interval"
unsigned long lastAvgTime = 0;
volatile unsigned long pulseStart = 0;

// accumulator variable
volatile unsigned long totalPulseDuration = 0;

typedef struct {
  float dustDetectedRatio;
  float count;
  float pm25;
} airQualityData_t;

airQualityData_t airData = { 0.0 };

char outputBuffer[64];
int outputBufferPosition;


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  setup
 *  Description:  This function runs when the Arduino first turns on/resets. This is
 *                our chance to take care of all one-time configuration tasks to get
 *                the program ready to begin logging data.
 *
 *                Start Serial communication for debugging, set pin modes, and start
 *                the ArdusatSDK off logging to the SD card. A new log file will be
 *                created every time the Arduino restarts.
 * =====================================================================================
 */
void setup(void)
{
  // change this to false to log binary data instead of CSV format
  // (note that this change will require post-processing decoding to read,
  // but uses much less SD card space)
  bool logAsCSV = true;

  serialConnection.begin(9600);

  pinMode(PULSE_PIN, INPUT_PULLUP);

  // this attaches the callback function "timeDustPulse" to
  // the interrupt 0 (DIO pin 2)
  // everytime that the interrupt pin value changes "timeDustPulse" is
  // called, regardless of where in the loop() the Arduino is
  attachInterrupt(INTERRUPT, timeDustPulse, CHANGE);

  if (!beginDataLog(10, LOG_FILE_PREFIX, logAsCSV)) {
    serialConnection.println("Failed to initialize SD card...");
    while (true);
  }

  serialConnection.println("Ratio\t\tCount\t\tPM 2.5");
  serialConnection.println("*******************************");
}


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  loop
 *  Description:  After setup runs, this loop function runs until the Arduino loses
 *                power or resets. Loop runs continuously (no delay), but only goes
 *                into the main "if" statement when AVERAGING_MS time has passed.
 *                Inside, it calculates the ratio (0.0 - 1.0) of dust pulses to non
 *                pulses over the last time period, then resets the accumulator. This
 *                ratio is used with a quadratic fit function to get an estimate of
 *                dust particle count (provided by Many Labs). This dust particle
 *                count is finally combined with humidity data and an accepted
 *                formula to get PM2.5 values.
 * =====================================================================================
 */
void loop(void)
{
  unsigned long time = millis();
  if (time - lastAvgTime > AVERAGING_MS) {

    // Compute low pulse occupancy - multiply by 1000 to convert to microseconds
    airData.dustDetectedRatio = totalPulseDuration / (float)((time - lastAvgTime) * 1000);
    totalPulseDuration = 0;
    lastAvgTime = time;

    // This count estimate comes from co-locating the PPD60 with a Dylos DC1700
    // and fitting a quadratic curve to the data
    airData.count = 79740.9269 * (airData.dustDetectedRatio * airData.dustDetectedRatio) +
                    10175.6562 * airData.dustDetectedRatio + 213.9981;

    float humidity = 50.0; // Replace with a real humidity sensor value
    airData.pm25 = dustCountToPM25(airData.count, humidity);

    // Save values to SD card
    logString(valuesToCSV(NULL, airData.dustDetectedRatio, 3, lastAvgTime));

    // Print values to Serial console for debugging
    serialConnection.print(airData.dustDetectedRatio, 4);
    serialConnection.print("\t\t");
    serialConnection.print(airData.count, 4);
    serialConnection.print("\t");
    serialConnection.println(airData.pm25, 4);
  }
}


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  timeDustPulse
 *  Description:  Times the duration of a dust pulse and adds the value to accumulator
 *                variables. Called everytime the interrupt pin changes (low -> high
 *                or high -> low)
 * =====================================================================================
 */
void timeDustPulse(void)
{
  // Check if Pin 2 is high
  if (digitalRead(2) == HIGH) {
    // Compute duration
    // NOTE: micros() overflows every ~70 minutes on Arduino. That could introduce
    // some error here, although with averaging it's probably a non issue
    totalPulseDuration += micros() - pulseStart;
  } else {
    // Start timing when it's low
    pulseStart = micros();
  }
}


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  dustCountToPM25
 *  Description:  Converts raw dust count data (ratio) + humidity values to PM2.5
 *                measurements.
 *
 *                Modified from
 *                http://wireless.ece.drexel.edu/research/sd_air_quality.pdf
 *                This is always assuming no rain. If you have a method to check for
 *                rain, there is another factor table in the same paper.
 *  Parameters:   dustCount: dust count
 *                humidity: humidity
 * =====================================================================================
 */
float dustCountToPM25(float dustCount, float humidity)
{
  int humidityInt = (int) round(humidity);
  double r25 = 0.44 * pow(10.0, -6.0);  // um, reference Lee paper
  double r10 = 2.6 * pow(10.0, -6.0);   // um, reference Lee paper
  double vol25 = (4.0 / 3.0) * PI * pow(r25, 3.0);
  double density = 1.65 * pow(10.0, 12.0); // ug/m3, reference titarelli paper
  double mass25 = density * vol25;      // ug
  double K = 3531.5;                    // per m^3

  // PM2.5 concentration
  double conc25First = dustCount * K * mass25; // ug/m^3
  double conc25 = -1;

  // NO Rain Table - Improvement Factor
  double factor0 = 13;    // 0-39%
  double factor40 = 8;    // 40-49%
  double factor50 = 6;    // 50-59%
  double factor60 = 4;    // 60-69%
  double factor70 = 1.75; // 70-79%
  double factor80 = 1.5;  // 80-89%
  double factor90 = 1;    // 90-100%

  if (humidityInt > 0 && humidityInt <= 39) {
    // Humidity between 0 - 39%
    conc25 = conc25First * (humidity / 100) * factor0;
  } else if(humidityInt >= 40 && humidityInt <= 49) {
    // Humidity between 40 - 49%
    conc25 = conc25First * (humidity / 100) * factor40;
  } else if(humidityInt >= 50 && humidityInt <= 59){
    // Humidity between 50 - 59%
    conc25 = conc25First * (humidity / 100) * factor50;
  } else if(humidityInt >= 60 && humidityInt <= 69){
    // Humidity between 60 - 69%
    conc25 = conc25First * (humidity / 100) * factor60;
  } else if(humidityInt >= 70 && humidityInt <= 79){
    // Humidity between 70 - 79%
    conc25 = conc25First * (humidity / 100) * factor70;
  } else if(humidityInt >= 80 && humidityInt <= 89){
    // Humidity between 80 - 89%
    conc25 = conc25First * (humidity / 100) * factor80;
  } else if(humidityInt >= 90 && humidityInt <= 100){
    // Humidity between 90 - 100%
    conc25 = conc25First * (humidity / 100) * factor90;
  }

  return conc25;
}


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  airQualityValuesToCSV
 *  Description:  Writes air quality values into the output buffer.
 *
 *                Note that we *could* use sprintf for this functionality (and
 *                probably would, on a computer), but Arduino's sprintf implementation
 *                is very inefficient for floating point numbers (and support isn't
 *                even enabled without including an extra library)
 *  Parameters:   timestamp: time
 *                ratio: ratio
 *                count: count
 *                pm25: pm25
 * =====================================================================================
 */
char * airQualityValuesToCSV(long timestamp, float ratio, float count, float pm25)
{
  const char SEPARATOR = ',';
  // reset the outputBuffer to all nulls
  memset(outputBuffer, 0, 128);
  outputBufferPosition = 0;

  // Write the timestamp to the output buffer (built-in, memory-efficient non
  // ANSI-C function to write out unsigned long as char *)
  ultoa(timestamp, outputBuffer, 10);
  outputBufferPosition = strlen(outputBuffer);
  outputBuffer[outputBufferPosition++] = SEPARATOR;

  // Write in each of the float values using dtostrf function
  dtostrf(ratio, 2, 4, outputBuffer + outputBufferPosition);
  outputBufferPosition = strlen(outputBuffer);
  outputBuffer[outputBufferPosition++] = SEPARATOR;

  dtostrf(count, 2, 4, outputBuffer + outputBufferPosition);
  outputBufferPosition = strlen(outputBuffer);
  outputBuffer[outputBufferPosition++] = SEPARATOR;

  dtostrf(pm25, 2, 4, outputBuffer + outputBufferPosition);
  outputBufferPosition = strlen(outputBuffer);
  outputBuffer[outputBufferPosition++] = '\n';

  return outputBuffer;
}
