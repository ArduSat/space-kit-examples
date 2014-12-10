#include <Arduino.h>
#include <Wire.h>
#include <ArdusatSDK.h>

int led = 13;

void setup(void)
{
  Serial.begin(9600);

  if (!beginInfraredTemperatureSensor()) {
    Serial.println("IR Temp sensor failed to start");
  }

  pinMode(led, OUTPUT);
}

void loop(void)
{
  temperature_t temp;

  readInfraredTemperature(&temp);

  Serial.print("IR Temperature: ");
  Serial.print(temp.t);
  Serial.println(" C");
  Serial.println("");

  if (temp.t > 29.5) {
    digitalWrite(led, HIGH);
  } else {
    digitalWrite(led, LOW);
  }

  delay(5000);
}
