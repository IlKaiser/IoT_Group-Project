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
- **durata, cm**: used to store, respectively, the time it takes for the impulse to go and return and the distance (in cm) measured.
- **RXPin, TXPin**: RXPin and TXPin for the GPS
- **infoGps**: used to store info retrieved from the bn-880n gps module
- **ss**: SoftwareSerial instance for the serial connection to the GPS device
- **EEBlue**: SoftwareSerial instance for the serial connection to the Bluetooth module
- **gps**: the TinyGPS++ object
- **stepper**: instance of the stepper class

So we need do define these global variables in the [sensors.ino](https://github.com/IlKaiser/IoT_Group-Project/blob/main/arduino_code/sensors.ino):

```cpp
String mode = "auto";
String ID="1";
byte x = 0;
long durata, cm;
static const int RXPin = 7, TXPin = 3;
static const uint32_t GPSBaud = 9600;
String infoGps;
SoftwareSerial ss(RXPin, TXPin);
SoftwareSerial EEBlue(3, 2);
TinyGPSPlus gps;
Stepper stepper(STEPS, 8, 10, 9, 11);
```

## Setup function
We need to set the Arduino address in order to correctly establish the I2C communication with the Nucleo board:

```cpp
Wire.begin(0x04);
```

Then we need to set the data rata of both serial connections to 9600 bps:

```cpp
EEBlue.begin(9600);
ss.begin(9600);
```

Then we need to specify that whenever on the I2C bus there is some data to read, we need to handle this event by executing the *receiveEvent* function:

```cpp
Wire.onReceive(receiveEvent);
```

Also, we need to specify that whenever the Nucleo board makes a specific request to the Arduino, we need to handle this event by executing the *requestEvent* function:

```cpp
Wire.onRequest(requestEvent);
```

Then we need to initialize the pins for the HCSR04 sensor:

```cpp
pinMode(trigPin, OUTPUT);
pinMode(echoPin, INPUT);
```

## Loop function
The loop function is used only to periodically update (by using the *smartDelay* function) the position of the floater, by using the *getGpsInfo* function. Below I report these two functions:

```cpp
String getGpsInfo(){
  if(gps.location.isValid() && gps.speed.isValid()){

    float lat = gps.location.lat(), lon = gps.location.lng(), speed_kmh = gps.speed.kmph();
   
    String lat_s = fromFloatToString(lat, 4);
    String lon_s = fromFloatToString(lon, 4);
    float speed_ms = speed_kmh / 3.6;

    String info = "";
    info += String(lat_s);
    info += ";";
    info += String(lon_s);
    info += ";";
    info += String(speed_ms);
    
    return info;
  }

  else{
      return "/";
  }
}

static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}
```

## receiveEvent function
This function just store the type of request received by the Nucleo board that has to be done:

```cpp
void receiveEvent(int data){
  x = Wire.read();
}
```

## requestEvent function
This function manages the vaious type of requests received by the Nucleo board, with the possibility to use an automatic and a test mode. By looking the auto mode, we present the various type of requests' management.

### Boat detection (x==1)
By using the HCSR04 sensor, we send to the Nucleo board an indication of a possible detection if we are under the MAX_DISTANCE_FROM_FLOATER threshold, otherwise an indication that everything is ok. Below we report the code:

```cpp
if(x == 1){
        
        measureDistanceBy_HCSR04();
  
        Serial.print("Cm = ");
        Serial.println(cm);
        Serial.println();

        if(cm <= MAX_DISTANCE_FROM_FLOATER){
          Serial.println("Possible detection!");
          Wire.write("1");  
        }

        else{
          Serial.println("No boats detected!");
          Wire.write("0");    
        }
        
        
}
```

Also, we report the *measureDistanceBy_HCSR04* function:

```cpp
void measureDistanceBy_HCSR04(){
   
   digitalWrite(trigPin, LOW);
   delayMicroseconds(2);
   digitalWrite(trigPin, HIGH);
   delayMicroseconds(10);
   digitalWrite(trigPin, LOW);
   durata = pulseIn(echoPin, HIGH);
   cm = durata / 58;    // for inches the formula is duration / 148;
   
}
```

### Proximity sensor correction (x==2 || x==3)
By using the Stepper motor, we apply a correction of the proximiy sensor towards the right if x is 2, otherwise towards the left if x is 3. Below we report the code for the correction towards the right, because the other one is similar:

```cpp
else if(x == 2){
        Serial.println("Received 2");

        int direction_ = -1, speed_ = 0;
        int val = 1023;

        speed_ = map(val, 0, 1023, 2, 500);
        // set the speed of the motor
        stepper.setSpeed(speed_);
  
 
        // move the stepper motor

        stepper.step(direction_);
  


        Wire.write("123");
        
}
```

### GPS (x==4)
By using the GPS module, we send to the Nucleo board, if available, the last known position of the floater. Below we show the code:

```cpp
else if (x == 4){
        if(infoGps == "/"){
          Wire.write("/");  
        }
    
        else{
          Wire.write(infoGps.c_str());  
        }
}
```

### Bluetooth (x==5)
By using the Bluetooth module, we send, if available, the last known position of the floater. Below we show the code:

```cpp
else if (x==5){
        if(infoGps!="/"){
          String toSend=infoGps+";"+ID;
          EEBlue.write(toSend.c_str());
        }
        else{
          EEBlue.write(ID.c_str());
        }
        Wire.write(x);
          
}
```
