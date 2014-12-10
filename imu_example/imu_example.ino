#include <Arduino.h>
#include <Wire.h>
#include <ArdusatSDK.h>

void setup(void)
{
  Serial.begin(9600);

  if (!beginAccelerationSensor()) {
    Serial.println("Acceleration sensor failed to start");
  }

  if (!beginMagneticSensor()) {
    Serial.println("Magnetic sensor failed to start");
  }
  if (!beginOrientationSensor()) {
    Serial.println("Orientation sensor failed to start");
  }
}

void loop(void)
{
  acceleration_t accel;
  magnetic_t mag;
  orientation_t orientation;

  readAcceleration(&accel);
  readMagnetic(&mag);
  readOrientation(&orientation);

  Serial.println("************************");
  Serial.print("Acceleration (x, y, z): ");
  Serial.print(accel.x);
  Serial.print(", ");
  Serial.print(accel.y);
  Serial.print(", ");
  Serial.println(accel.z);

  Serial.print("Magnetic Field (x, y, z): ");
  Serial.print(mag.x);
  Serial.print(", ");
  Serial.print(mag.y);
  Serial.print(", ");
  Serial.println(mag.z);

  Serial.print("Orientation (roll, pitch, heading): ");
  Serial.print(orientation.roll);
  Serial.print(", ");
  Serial.print(orientation.pitch);
  Serial.print(", ");
  Serial.println(orientation.heading);
  Serial.println("");

  delay(5000);
}
