Ardusat Space Kit Examples
==========================

This repository contains Arduino example sketches for use with the 
[Ardusat Space Kit](http://www.ardusat.com/products). This kit contains a wide assortment of
sensors, an Arduino chip, and all the breadboards and components needed to get started prototyping
and collecting data. The examples in this repository will help you get up and running with the
sensors as quickly as possible.

# Configuring Arduino and Running Examples

The first step to getting going is to install the Arduino Integrated Development Environment (IDE).
Downloads for all major OS versions can be found at
[http://arduino.cc/en/Main/Software](http://arduino.cc/en/Main/Software). 

After installing the IDE, use the link on the right side of this page to download the example
repository in the format of your choice, and make sure you know where you save it on your hard
drive. Then, open up the Arduino IDE, go into the Preferences, and change the Sketchbook location to
this repository. After you save this change, you should be able to open one of the example sketches
and hit the "check" button to compile it or the "arrow" button to compile and send to the Arduino.

Start small - the single sensor examples found in `sensor_examples` are a great place to begin! Each
of these focuses on a single sensor and demonstrates how to initialize the sensor and beging gathering
data. 

# Examples

Check back frequently, since we'll be adding examples regularly as we expand our offerings and
online curriculum. Here's a brief description of examples provided in this repository:

- **drum_kit_multiaxis** - This example sketch uses accelerometer data from the 9DOF IMU to make
    music. It does this by detecting "shake" events as shown by the total magnitude of acceleration,
    then assigning a different instrument from a 5 piece drum kit sound library to each physical
    axis. This demo can be used with <http://experiments.ardusat.com/#/sensors/drum-kit> to make
    music by shaking the IMU!
- **air_quality** - Example sketch that uses a PPD60 air quality sensor and calibration performed by
    our friends at [Many Labs](http://www.manylabs.org/) to measure and log PM2.5 air quality.
- **basic_serial_test** - This example shows the most basic example of sending text data over the
    serial port. If successful, the Arduino Serial Monitor should print "Hello" over and over.
- **sensor_examples** - This is a set of example sketches designed to demonstrate playing with individual
    sensors in isolation. Each sketch measures a particular property (e.g. acceleration, temperature, etc.)
    and does some form of manipulation with it, from turning LEDs on/off based on sensor values to 
    outputting sensor data in JSON format readable by <http://experiments.ardusat.com>
- **space_kit_tester** - This example reads from every sensor included in the Space Kit and outputs
    the results in JSON format for easy machine parsing/consumption. It also has temperature and
    light sensitive LEDs that turn on and off based on temperature readings.
- **g_force_demo** - This example simply reads the `x`, `y`, and `z` axis data from the `LSM303`
    accelerometer at a configurable interval and outputs the result as a JSON formated string that
    is compatible with <http://demo.ardusat.com>
- **imu_example** - This example shows using the [Ardusat SDK](http://github.com/ArduSat/ArduSatSDK)
    to read all 9 axes of the IMU. It should print values to the Serial Monitor every 5 seconds and 
    is meant as an example to get started using the IMU data.
- **ir_temp_example** - This sketch takes an IR temperature reading every 5 seconds and prints the 
    results to the Serial Monitor. If the temperature detected is over 28.5 C, it turns on a pin 13
    LED.
- **accel_rattle** - This example "listens" for shakes using the accelerometer and will play from a
    set of sounds depending on the state. A simple push button is used to cycle through four states.
    This sketch is designed to work with the
    [Experiment Platform Drum Kit](http://experiments.ardusat.com/#/sensors/drum-kit).
- **gyro_scales** - This example plays scales of a specific instrument depending on the gyroscope
    readings for rotational velocities. Negative velocities play down the scale and positive velocities
    play up the scale. This sketch is designed to work with the
    [Experiment Platform Drum Kit](http://experiments.ardusat.com/#/sensors/drum-kit).
- **infrared_band** - This example plays selected sounds depending on accelerometer and infrared
    thermometer readings. The accelerometer is used to select an "instrument" and the temperature from
    the infrared thermometer selects a sound from that instrument. This sketch is designed to work with
    the [Experiment Platform Drum Kit](http://experiments.ardusat.com/#/sensors/drum-kit).
- **magnet_strum** - This example plays a sitar strum sound when within the presence of a strong enough
    magnetic force. The magnetometer was placed above a rig with a magnet taped to the center of a
    stretched rubber band. This sketch is designed to work with the
    [Experiment Platform Drum Kit](http://experiments.ardusat.com/#/sensors/drum-kit).

# Getting Help

If you're having trouble running the examples, chances are something is messed up with the external
library locations in your Arduino IDE. Double check that the Sketchbook location is pointed at the
repository folder (e.g. `/Users/me/Documents/space-kit-examples`). If the sketches are compiling and
uploading but not behaving as expected, make sure you double check your wiring, it's always easy to
accidentally plug something in wrong! 

If you get really stuck, feel free to reach out at <support@ardusat.com>, or the "Issues" section of
this repository.
