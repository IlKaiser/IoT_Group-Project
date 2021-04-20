#include <Wire.h>
#include <Stepper.h>

// Global variables
String mode = "auto";  // auto || test

byte x = 0;
long durata, cm;

// HC SR04 ultrasound sensor pins definition
#define trigPin                       12
#define echoPin                       13

// change this to the number of steps on your motor
#define STEPS                         48

// create an instance of the stepper class, specifying
// the number of steps of the motor and the pins it's
// attached to
Stepper stepper(STEPS, 8, 10, 9, 11);

// DISTANCE THRESHOLD
#define MAX_DISTANCE_FROM_FLOATER     100                     // It should be about 170-180 cm

void setup() {
  // Setting the address 
  Wire.begin(0x04);
  Serial.begin(9600);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);

  // HC SR04 ultrasound sensor pins initialization
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {}

void receiveEvent(int data){
  x = Wire.read();
  //Serial.print("Received: ");
  //Serial.println(x);
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

     // Rotate the proximity sensor to the left
      else if(x == 3){
        Serial.println("Received 3");

        int direction_ = 1, speed_ = 0;
        int val = 1023;

        speed_ = map(val, 0, 1023, 2, 500);
        // set the speed of the motor
        stepper.setSpeed(speed_);
  
 
        // move the stepper motor

        stepper.step(direction_);

        Wire.write("321"); 
      }
       
  }

  // Test mode
  else if(mode == "test"){
    Serial.println("Now in test mode");
    Serial.println("Sending random generated data to STM-32...");  
  }
   
}

void measureDistanceBy_HCSR04(){
   
   digitalWrite(trigPin, LOW);
   delayMicroseconds(2);
   digitalWrite(trigPin, HIGH);
   delayMicroseconds(10);
   digitalWrite(trigPin, LOW);
   durata = pulseIn(echoPin, HIGH);
   cm = durata / 58;    // for inches the formula is duration / 148;
   
}
