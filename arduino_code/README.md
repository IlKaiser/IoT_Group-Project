# Arduino code

## Libraries

- **Wire.h**: used to send messagges over the I2C bus towards the Nucleo board
- **Stepper.h**: used to manage the Stepper motor
- **TinyGPS.h**: used to parse data received from the gps module (you have to download it from [here](https://github.com/mikalhart/TinyGPSPlus/archive/refs/tags/v1.0.2b.zip), rename the folder in the zip as "TinyGPSPlus" and put it in the libraries folder of the Arduino folder)
- **SoftwareSerial.h**: used for the serial connection with the GPS device and the Bluetooth module

In order to use them, we need to include them on the top of the [sensors.ino](https://github.com/IlKaiser/IoT_Group-Project/blob/main/arduino_code/sensors.ino):

```cpp
#include <Wire.h>
#include <Stepper.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
```

## Macro

- **trigPin**: it specifies the trig pin of the HCSR04 ultrasonic proximity sensor, whose purpose is to trigger the ultrasound
- **echoPin**: it specifies the echo pin of the HCSR04 ultrasonic proximity sensor, whose purpose is to receive the signal back from the object detected
- **SMART_DELAY_GPS**: it indicates (in ms) the delay after which the gps object tries to read new data
- **STEPS**: number of steps performed by the Stepper motor
- **MAX_DISTANCE_FROM_FLOATER**: under this threshold (in cm), the Arduino board send a notification of a possible detection to the Nucleo board

So we need do define these constants in the [sensors.ino](https://github.com/IlKaiser/IoT_Group-Project/blob/main/arduino_code/sensors.ino):

```cpp
#define trigPin                       12
#define echoPin                       13
#define SMART_DELAY_GPS               50
#define STEPS                         48
#define MAX_DISTANCE_FROM_FLOATER     170   
```

## Global variables

- **mode**: auto or test, default is auto
- **ID**: the floater id
- **x**: a byte used to discriminate between the different type of requests recieved by the Nucleo board
- **durata, cm**: used to store
