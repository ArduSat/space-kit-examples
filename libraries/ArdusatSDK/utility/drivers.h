/*
 * =====================================================================================
 *
 *       Filename:  drivers.h
 *
 *    Description:  Implementations of sensor-specific initialization and read driver 
 *                  functions for use by the higher-level Ardusat SDK
 *
 *        Version:  1.0
 *        Created:  12/03/2014 14:47:14
 *
 *         Author:  Ben Peters (ben@ardusat.com)
 *
 * =====================================================================================
 */

#ifndef DRIVERS_H_
#define DRIVERS_H_

#include <Arduino.h>
#include <utility/mlx90614.h>
#include <utility/Adafruit_Sensor.h>
#include <utility/Adafruit_LSM303_U.h>
#include <utility/Adafruit_L3GD20_U.h>
#include <utility/Adafruit_SI1145.h>
#include <utility/TSL2561.h>

#define DRIVER_TSL2561_ADDR		0x39
#define DRIVER_TMP102_ADDR		0x48
#define DRIVER_MLX90614_ADDR	0x5A
#define DRIVER_SI1145_ADDR		0x60
#define DRIVER_ML8511_UV_PIN A0
#define DRIVER_ML8511_REF_PIN A1

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Adafruit 9DOF IMU breakout board features L3GD20H Gyroscope and LSM303 6-axis 
 * magnetometer + accelerometer.
 *
 * http://www.adafruit.com/product/1714
 */
boolean l3gd20h_init();
boolean lsm303_accel_init();
boolean lsm303_mag_init();
void l3gd20h_getOrientation(float *x, float *y, float *z);
void lsm303_getAccel(float * x, float * y, float * z);
void lsm303_getMag(float * x, float * y, float * z);

/**
 * ML8511 breakout board contains an MP8511 UV light sensor
 *
 * https://www.sparkfun.com/products/12705
 */
boolean ml8511_init();
float ml8511_getUV();

/**
 * SI1145 breakout board is a UV/Light sensor from Adafruit. It was included in earlier
 * versions of the SpaceKit and is still supported, though not enabled by default.
 *
 * https://learn.adafruit.com/adafruit-si1145-breakout-board-uv-ir-visible-sensor/overview
 */
boolean si1145_init();
float si1145_getUVIndex();

/**
 * MLX90614 IR Thermometer for non-contact temperature sensing.
 *
 * https://www.sparkfun.com/products/9570
 */
boolean mlx90614_init();
float mlx90614_getTempCelsius();

/**
 * TMP102 Temperature sensor
 *
 * https://www.sparkfun.com/products/11931
 */
boolean tmp102_init();
float tmp102_getTempCelsius();		

/**
 * TSL2561 Luminosity Sensor
 *
 * http://www.adafruit.com/product/439
 */
boolean tsl2561_init();
float tsl2561_getLux();

#ifdef __cplusplus
} // extern "C"
#endif

#endif
