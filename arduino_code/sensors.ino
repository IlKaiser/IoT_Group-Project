#include <Wire.h>

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
