#include <TinyGPS++.h>
#include <SoftwareSerial.h>

static const int RXPin = 7, TXPin = 3; //Connect only pin 4 to GPS TX
static const uint32_t GPSBaud = 9600; //Modified to be used with BN-880 or uBlox NEO-7M / M8N

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);


void setup() {
  Serial.begin(9600);
  ss.begin(GPSBaud);

}

void loop() {
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
    Serial.print("INFO: ");
    Serial.println(info);

    //Wire.write(info.c_str());
    
          
    }

    else{
        Serial.println("Not able to read coordinates/speed.");  
        //Wire.write("/");
    }

    smartDelay(1000);

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

String fromFloatToString(float val, int precision){
  String buf;
  buf += String(val, precision);

  return buf;
}
