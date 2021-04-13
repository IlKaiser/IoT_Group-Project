#include <Wire.h>

String mode = "auto";  // auto || test

byte x = 0;

void setup() {
  Wire.begin(0x04);
  Serial.begin(9600);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
}

void loop() {
}

void receiveEvent(int data){
  x = Wire.read();
  Serial.print("Received: ");
  Serial.println(x);
}

void requestEvent() {
  
  // Auto mode
  if(mode == "auto"){

      // Ultrasound sensor
      if(x == 1){
        Wire.write("11");  
      }

      // If needed, gyroscope + accelerometer
      else if(x == 2){
        Wire.write("22");  
      }

      // Gps
      else if(x == 3){
        Wire.write("33");  
      }
       
  }

  // Test mode
  else if(mode == "test"){
    Serial.println("Now in test mode");
    Serial.println("Sending random generated data to STM-32...");  
  }
  
}
