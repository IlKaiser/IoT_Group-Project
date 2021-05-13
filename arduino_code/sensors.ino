#include <Wire.h>
#include <Stepper.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

// MACRO

// HC SR04 ultrasound sensor pins definition
#define trigPin                       12
#define echoPin                       13
#define SMART_DELAY_GPS               50

// change this to the number of steps on your motor
#define STEPS                         48

// Distance threshold
#define MAX_DISTANCE_FROM_FLOATER     100                     // It should be about 170-180 cm


// GLOBAL VARIABLES
String mode = "auto";  // auto || test

String ID="1";

byte x = 0;
long durata, cm;

static const int RXPin = 7, TXPin = 3; //Connect only pin 7 to GPS TX
static const uint32_t GPSBaud = 9600; //Modified to be used with BN-880 or uBlox NEO-7M / M8N
String infoGps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

// The serial connection with the Bluetooth module
SoftwareSerial EEBlue(3, 2); // RX | TX

// The TinyGPS++ object
TinyGPSPlus gps;

// create an instance of the stepper class, specifying
// the number of steps of the motor and the pins it's
// attached to
Stepper stepper(STEPS, 8, 10, 9, 11);

void setup() {
  // Setting the address 
  Wire.begin(0x04);
  Serial.begin(9600);
  EEBlue.begin(9600);  //Default Baud for comm, it may be different for your Module. 
  Serial.println("The bluetooth gates are open.\n Connect to HC-05 from any other bluetooth device with 1234 as pairing key!.");
  ss.begin(GPSBaud);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);

  // HC SR04 ultrasound sensor pins initialization
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
  infoGps = getGpsInfo();
  
  smartDelay(SMART_DELAY_GPS);  
}

void receiveEvent(int data){
  x = Wire.read();
}

void requestEvent() {
  
  // Auto mode
  if(mode == "auto"){

      // Ultrasound sensor
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
      
      // Rotate the proximity sensor to the right
      else if(x == 2){
       

        int direction_ = -1, speed_ = 0;
        int val = 1023;

        speed_ = map(val, 0, 1023, 2, 500);
        // set the speed of the motor
        stepper.setSpeed(speed_);
  
 
        // move the stepper motor

        stepper.step(direction_);
  


        Wire.write("123");
        
      }

     // Rotate the proximity sensor to the left
      else if(x == 3){
        
        int direction_ = 1, speed_ = 0;
        int val = 1023;

        speed_ = map(val, 0, 1023, 2, 500);
        // set the speed of the motor
        stepper.setSpeed(speed_);
  
 
        // move the stepper motor

        stepper.step(direction_);

        Wire.write("321"); 
      }

      // Gps
      else if (x == 4){
        if(infoGps == "/"){
          Wire.write("/");  
        }
    
        else{
          Wire.write(infoGps.c_str());  
        }
      }

      // Bluetooth
      else if (x==5)
      {

        
        
        if(infoGps!="/"){
          String toSend=infoGps+";"+ID+"\n";
          EEBlue.print(toSend);
          Serial.println("BLUETOOTH TIME 1! \n");
        }
        else{
          EEBlue.print(ID+'\n');
          Serial.println("BLUETOOTH TIME 2! \n");
        }
        Wire.write(x);
          
      }
       
  }

  // Test mode
  else if(mode == "test"){
    infoGps="/";
    if(x == 1){
          Wire.write("1"); 
      }
      else if (x==5)
      {
        if(infoGps!="/"){
          String toSend=infoGps+";"+ID;
          EEBlue.write(toSend.c_str());
        }
        else{
          EEBlue.write(ID.c_str());
        }
        Wire.write(x);
          
      }
  }
   
}

// Function that measures the distance from a nearby object using HCSR04 sensor
void measureDistanceBy_HCSR04(){
   
   digitalWrite(trigPin, LOW);
   delayMicroseconds(2);
   digitalWrite(trigPin, HIGH);
   delayMicroseconds(10);
   digitalWrite(trigPin, LOW);
   durata = pulseIn(echoPin, HIGH);
   cm = durata / 58;    // for inches the formula is duration / 148;
   
}

// This custom version of delay() ensures that the gps object
// is being "fed".
static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}

// Utility function to convert a float into String
String fromFloatToString(float val, int precision){
  String buf;
  buf += String(val, precision);

  return buf;
}

// Function that reads info from bn-880 gps module
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

    Serial.println(info);
    
    return info;
  }

  else{
      return "/";
  }
}
