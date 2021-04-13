#include <Wire.h>

// Global variables
String mode = "auto";  // auto || test

byte x = 0;
long durata, cm;

// HC SR04 ultrasound sensor pins definition
#define trigPin                       6
#define echoPin                       7

#define MAX_DISTANCE_FROM_FLOATER     100                     // It should be about 170-180 cm
#define BOAT_TIME                     4000                    // ms
#define POSSIBLE_DETECTION_DELAY      0                       // ms

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

void loop() {

  /*measureDistanceBy_HCSR04();
  
  Serial.print("Cm = ");
  Serial.println(cm);
  Serial.println();

  if(cm <= MAX_DISTANCE_FROM_FLOATER){

    unsigned start = millis(), elapsed;
    boolean detected = false;

    while(1){
      Serial.println("Misuro");
      measureDistanceBy_HCSR04();

      elapsed = millis() - start;

      Serial.print("elapsed: ");
      Serial.println(elapsed);

      if(cm <= MAX_DISTANCE_FROM_FLOATER && elapsed  > BOAT_TIME ){  
        Serial.println("DETECTED!!!");
        detected = true;
        break;
      }

      else if(cm > MAX_DISTANCE_FROM_FLOATER){
        Serial.println("FALSE POSITIVE!!!");
        detected = false;
        break; 
      }
      
      delay(POSSIBLE_DETECTION_DELAY);
    }

    if(detected){
      Serial.println("Boat detected!");
      //Wire.write("1");    
    }

    else{
      Serial.println("False positive!");  
      //Wire.write("0");
    }
          
  }
  
  else{
    Serial.println("No boats detected!");
    //Wire.write("0");
  }

  delay(1000);
  */
}

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
      
      // If needed, gyroscope + accelerometer module
      else if(x == 2){
        Serial.println("Received 2");
        //Wire.write("Correction");  
      }

      // Gps module
      else if(x == 3){
        Serial.println("Received 3");
        //Wire.write("Position");  
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

