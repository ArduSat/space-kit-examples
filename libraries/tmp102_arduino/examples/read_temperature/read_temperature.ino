#include <Wire.h>
#include <tmp102.h>
#define TMP102_ADDR 0x48

Tmp102 tmp102(&Wire, TMP102_ADDR);

void setup(){
	Serial.begin(9600);
}

void loop(){

	Serial.print("Temp:\t");
	Serial.print(tmp102.read());
	Serial.println("\tdegrees.");

	delay(1000);
}
