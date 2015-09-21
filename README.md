# Ardusat Space Kit Examples

This repository contains Arduino example sketches for use with the 
[Ardusat Space Kit](http://www.ardusat.com/products). This kit contains a wide assortment of
sensors, an Arduino chip, and all the breadboards and components needed to get started prototyping
and collecting data.

The examples in this repository extend all of the basic examples provided with the
[Ardusat SDK](http://github.com/ardusat/ardusatsdk)! These examples are a bit more complex and
are designed to give you just a taste of what can be accomplished with Ardusat hardware and
software.


## Running Examples
First, make sure you have completed the 'Installing the SDK' section in our
[Ardusat SDK README](http://github.com/ardusat/ardusatsdk#readme).

Use the link on the right side of this page to download the example repository in the format of your
choice, and make sure you know where you save it on your hard drive. You should be able to open one
of the Space Kit Example sketches in the Arduino IDE. Hit the "check" button to compile it or the
"arrow" button to compile and send to the Arduino.


### Examples
Check back frequently, since we'll be adding examples regularly as we expand our offerings and
online curriculum. Here's a brief description of examples provided in this repository:

- **air_quality** - Example sketch that uses a PPD60 air quality sensor and calibration performed by
    our friends at [Many Labs](http://www.manylabs.org/) to measure and log PM2.5 air quality.
- **egg_drop** - Logs accelerometer data to an SD card for use with an egg drop experiment.
- **example_template** - An example style template to use if you are interested in contributing a
    sketch.
- **imu_all_sensors** - This example returns json values for all of the sensors available from just the IMU.
- **music_examples** - Examples that demonstrate playing music through the
    [Experiment Platform Drum Kit](http://experiments.ardusat.com/#/sensors/drum-kit).
  - **accel_rattle** - This example "listens" for shakes using the accelerometer and will play from a set
      of sounds.
  - **drum_kit_multiaxis** - This example sketch uses accelerometer data from the 9DOF IMU to make
      music. It does this by detecting "shake" events as shown by the total magnitude of acceleration,
      then assigning a different instrument from a 5 piece drum kit sound library to each physical
      axis.
  - **g_force_test** - Measures the maximum G Force created when shaking an accelerometer for 3 seconds and
      then plays notes corresponding to the highest force recorded.
  - **gyro_scales** - This example plays scales of a specific instrument depending on the gyroscope
      readings for rotational velocities. Negative velocities play down the scale and positive velocities
      play up the scale.
  - **infrared_band** - This example plays selected sounds depending on accelerometer and infrared
      thermometer readings. The accelerometer is used to select an "instrument" and the temperature from
      the infrared thermometer selects a sound from that instrument.
  - **magnet_strum** - This example plays a sitar strum sound when within the presence of a strong enough
      magnetic force. The magnetometer was placed above a rig with a magnet taped to the center of a
      stretched rubber band.
  - **mary_had_a_little_lamb** - This example doesn't use any sensors, but demonstrates how to play
      Mary Had a Little Lamb using an Arduino and the Ardusat Experiment Platform.
- **sine_wave** - This example plots a sine wave and uses the generic valueToJSON formatting. Requires no
    wiring of sensors to demonstrate the graphing, logging, and Theremin view.
- **zombie_detector** - Uses luminosity and infrared thermometer sensor readings to turn an LED light on or
    off.

## Getting Help
If you're having trouble running the examples, chances are something is messed up with the external
library locations in your Arduino IDE. Double check that the Sketchbook location is pointed at the
repository folder (e.g. `/Users/me/Documents/space-kit-examples`). If the sketches are compiling and
uploading but not behaving as expected, make sure you double check your wiring, it's always easy to
accidentally plug something in wrong!

If you get really stuck, feel free to reach out at <support@ardusat.com>, or the "Issues" section of
this repository.
