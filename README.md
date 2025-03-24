
![SparkFun Pulsed Coherent Radar Sensor – Acconeer XM125 (Qwiic)](docs/images/gh-banner-2025-xm125.png "SparkFun Pulsed Coherent Radar Sensor")

# SparkFun Pulsed Coherent Radar Sensor – Acconeer XM125

Arduino Library for the SparkFun Pulsed Coherent Radar Sensor

![License](https://img.shields.io/github/license/sparkfun/SparkFun_Qwiic_XM125_Arduino_Library)
![Release](https://img.shields.io/github/v/release/sparkfun/SparkFun_Qwiic_XM125_Arduino_Library)
![Release Date](https://img.shields.io/github/release-date/sparkfun/SparkFun_Qwiic_XM125_Arduino_Library)
![Documentation - build](https://img.shields.io/github/actions/workflow/status/sparkfun/SparkFun_Qwiic_XM125_Arduino_Library/build-deploy-ghpages.yml?label=doc%20build)
![Compile - Test](https://img.shields.io/github/actions/workflow/status/sparkfun/SparkFun_Qwiic_XM125_Arduino_Library/compile-sketch.yml?label=compile%20test)
![GitHub issues](https://img.shields.io/github/issues/sparkfun/SparkFun_Qwiic_XM125_Arduino_Library)

The [SparkFun Pulsed Coherent Radar Sensor - Acconeer XM125 (SEN-24540)](https://www.sparkfun.com/sparkfun-pulsed-coherent-radar-sensor-acconeer-xm125-qwiic.html) brings powerful 60 GHz radar technology to your projects. This sensor isn't limited to surface detection; it can see through walls, cabinets, and even pockets (depending on the material), making it perfect for unique applications. Measure distances with millimeter precision, detect motion, the speed of an object, or even gestures!

The XM125 boasts an impressive range of up to 20 meters, allowing you to create long-range sensing projects. The actual measurable distance is dependent on the object size, shape, dielectric properties, and lens (e.g. water level measurements up to 20 meters with lens utilization, human presence detection up to 7 meters with lens-free utilization). Despite its power, the sensor has remarkably low in power consumption, which is ideal for battery-powered applications. The real magic lies in the sensor's ability to do more than measure distance; the XM125 can differentiate between stationary objects and moving targets using pulsed coherent radar. This means you can sense an object's presence and how fast something is moving!

Looking for the board that matches this library - pick up a [SparkFun Pulsed Coherent Radar Sensor - Acconeer XM125 (Qwiic)](https://www.sparkfun.com/sparkfun-pulsed-coherent-radar-sensor-acconeer-xm125-qwiic.html) at <www.sparkfun.com>.

> [!NOTE]
>
> If upgrading from **Version 1** of this library, some changes are required before moving to **Version 2.0+**. The detals of this are outlined in the section titled [Upgrading to Version 2 from Version 1](#upgrading-to-version-2-from-version-1) later in this document.

## Functionality

The SparkFun Pulsed Coherent Radar sensor can run as an I2C client device, or as a standalone development board. This library is used when the sensor is operating as a standalone I2C device.

When running as a I2C client device, the Acconeer XM125 is loaded with a specific firmware application from Acconeer.

The *SparkFun Pulsed Coherent Radar Sensor - Acconeer XM125* comes with the ***Presence Detector*** firmware loaded, which is accessible using the ***Presence*** focused methods in this library.

This library also supports the ***Distance Detection*** application firmware from Acconeer. To enable this functionally, the ***Distance Detection*** firmware must be loaded onto the *SparkFun Pulsed Coherent Radar Sensor - Acconeer XM125* board. The firmware is available from [Acconeer](https://developer.acconeer.com/home/a121-docs-software/xm125-xe125/) as part of the XM125/A121 SDK. The files are located in the `out/` folder - with naming patter of `i2c_*_detector.bin`. Install instructions are noted in our [Hook Up Guide](https://docs.sparkfun.com/SparkFun_Qwiic_Pulsed_Radar_Sensor_XM125/introduction/) for the product.

This library provides a extensive interface that enables the following functionality/interaction with the SparkFun Pulsed Coherent Radar Sensor when the sensor is operating as an I2C sensor device:

|Function|Firmware| Library Class To Use|
|---|---|--|
|Distance Detection | `i2c_distance_detector` | `SparkFunXM125Distance`|
|Presence Detection | `i2c_presence_detector` | `SparkFunXM125Presence`|

## Documentation

|Reference | Description |
|---|---|
|[Quick Reference](https://docs.sparkfun.com/SparkFun_Qwiic_XM125_Arduino_Library/class_qw_dev_x_m125.html)| A quick reference API for the the main library object ```QwDevXM125```|
|[Full Documentation](https://docs.sparkfun.com/SparkFun_Qwiic_XM125_Arduino_Library/)| The full documentation and API for this Arduino library|
|[SparkFun Pulsed Coherent Radar Sensor](https://github.com/sparkfun/SparkFun_Qwiic_Pulsed_Radar_Sensor_XM125)| Hardware GitHub Repository|
|[Hook Up Guide](https://docs.sparkfun.com/SparkFun_Qwiic_Pulsed_Radar_Sensor_XM125/introduction/) | Hardware Overview and Quick Start for the XM125 Qwiic Development Board |



## Examples

The following examples are provided with the library

| Example | Description |
|---|---|
|[Presence Basic Readings](examples/Example01_PresenceBasicReadings/Example01_PresenceBasicReadings.ino)| The sensor is initialized, then the presence distance values will print out to the terminal.|
| [Presence GPIO 0 Usage](examples/Example02_PresenceGPIO0Usage/Example02_PresenceGPIO0Usage.ino)|The sensor is initialized, then the presence values will print out to the terminal and trigger the GPIO0 pin high when there is a presence detected. |
|[Presence Serial Plotter](examples/Example03_PresenceSerialPlotter/Example03_PresenceSerialPlotter.ino)|The sensor is initialized, then the presence values will print out to the terminal and the serial monitor.|
|[Presence Advanced Readings](examples/Example04_PresenceAdvancedReadings/Example04_PresenceAdvancedReadings.ino)|The sensor is initialized, then the presence distance, intra-presence, and inter-presence values will be printed to the terminal.|
|[Presence Advanced Settings](examples/Example05_PresenceAdvancedSettings/Example05_PresenceAdvancedSettings.ino)|The sensor is initialized, then the presence distance values will print out to the terminal just as they do in example one. If you wish to change the settings of the device, do so before applying the configuration.|
|[Distance Basic Readings](examples/Example06_DistanceBasicReadings/Example06_DistanceBasicReadings.ino)|The sensor is initialized, then the distance values will print out to the terminal in mm. |
|[Distance Threshold Settings](examples/Example07_DistanceThresholdSettings/Example07_DistanceThresholdSettings.ino)|The sensor is initialized, then the distance amplitude, and strength , fixed amplitude, and sensitivity thresholds are set. |
|[Distance Serial Plotter](examples/Example08_DistanceSerialPlotter/Example08_DistanceSerialPlotter.ino)|This example prints out the distance values of the 0 distance channels to the serial plotter tool in Arduino.|
|[Distance Advanced Settings](examples/Example09_DistanceAdvancedSettings/Example09_DistanceAdvancedSettings.ino)|The sensor is initialized, then the distance (mm) and advanced values are output to the terminal. |
  

## Documentation

The full API and use documentation for this library is provided [here](https://docs.sparkfun.com/SparkFun_Qwiic_XM125_Arduino_Library/). For a quick reference, the main methods available in the library are listed [here](https://docs.sparkfun.com/SparkFun_Qwiic_XM125_Arduino_Library/class_qw_dev_x_m125.html).

Curious about the hardware this board works with - visit the SparkFun Pulsed Coherent Radar Sensor [hardware repository](https://github.com/sparkfun/SparkFun_Qwiic_Pulsed_Radar_Sensor_XM125).

The Hookup Guide for the SparkFun Qwiic Soil Moisture Sensor is available [here](https://docs.sparkfun.com/SparkFun_Qwiic_Pulsed_Radar_Sensor_XM125/introduction/).

## License Information

This product is ***open source***!

This product is licensed using the [MIT Open Source License](https://opensource.org/license/mit).

## Upgrading to Version 2 from Version 1

Version 2.0+ of this library is not compatible with Version 1.* implementations/use. For Version 2.0, the single class defined by the library, `SfeXM125`, was divided into two distinct classes for schemantic clarity and code simplification.

### Version 2.0 Changes

#### Classes

The device single class was devided into two classes - one for each specific I2C application that the Acconeer XM125 can run. These classes are:

* SparkFunXM125Distance - Used when the XM125 is running the ***Distance*** application
* SparkFunXM125Presence - Used when the XM125 is running the ***Presence*** application

#### Method Names

Additionally, the method naming pattern for each application was simplified - removing *Distance* and *Presence* from method names.  

### Supporting Version 1.* Implementations

To simplify the transition from an application written using the Version 1.* library, two compatiblity classes are provided:

* SparkFunXM125DistanceV1 - Used when the XM125 is running the ***Distance*** application
* SparkFunXM125PresenceV1 - Used when the XM125 is running the ***Presence*** application

These classes implement the version 1.*method names for the library, to minimize the porting requirements when upgrading to Version 2.*.

To upgrade to Version 2.0 of the library, perform one of the following:

* If the Acconeer XM125 is running the I2C Distance Application: Rename the device class from `SfeXM125` to `SparkFunXM125DistanceV1`
* If the Acconeer XM125 is running the I2C Presence Application: Rename the device class from `SfeXM125` to `SparkFunXM125PresenceV1`

Once the class name is changed, your application is ready to use version 2.* of this library.
