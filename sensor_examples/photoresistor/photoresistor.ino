/*
 * =====================================================================================
 *
 *       Filename:  phtoresistor.ino
 *
 *    Description:  Basic demonstration of how to read data from the photoresistor. After
 *                  a delay, takes an initial reading from the photoresistor to serve as 
 *                  a "baseline" value, then activates an LED if the value goes 
 *                  substantially below that (e.g. if the resistor becomes shadowed) 
 *
 *                  This example uses many third-party libraries available from
 *                  Adafruit (https://github.com/adafruit). These libraries are
 *                  mostly under an Apache License, Version 2.0.
 *
 *                  http://www.apache.org/licenses/LICENSE-2.0
 *
 *         Author:  Ben Peters (ben@ardusat.com)
 *
 * =====================================================================================
 */

#define LED_PIN 4 //Turn LED connected to pin 4 on when low light conditions are detected
#define LDR_PIN A2 //Analog pin LDR photoresistor is connected to.

int initial_value;

/*
 * setup initializes the pins for reading values from the LDR and LED on/off output.
 */
void setup()
{
  Serial.begin(9600);
  Serial.println("Ardusat Photoresistor Demo");
  Serial.println("In 5 seconds, we will get a baseline light reading.");
  Serial.println("Make sure the Arduino is not shadowed.");

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  delay(5000);

  initial_value = analogRead(LDR_PIN); 
  Serial.print("Initial reading of LDR photoresistor obtained: ");
  Serial.println(initial_value);
}

/*
 * loop looks at the current value of the LDR pin and compares it to the value we 
 * saved in setup(). Note that these values are just relative to each other - they 
 * are based on the voltage seen by the ADC in the Arduino and are not in meaningful 
 * units.
 */
void loop()
{
  int current_value = analogRead(LDR_PIN);

  if (current_value / (float) initial_value < 0.5) {
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }

  delay(100);
}
