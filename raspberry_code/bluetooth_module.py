import serial
from time import sleep

###########################################################################
#                  Before starting this script run:                       #
#              sudo rfcomm connect 0 98:D3:31:F9:95:52                    #
###########################################################################

def recieveLine():
    
    bluetoothSerial = serial.Serial( "/dev/rfcomm0", baudrate=9600 )
    line = bluetoothSerial.readline()
   
    string = line.decode("utf-8")
    
    lat   = string.split(";")[0]
    long  = string.split(";")[1]
    speed = string.split(";")[2]
    id    = string.split(";")[3]
    
    print("Recieved "+lat,long,speed,id)
    return lat,long,speed,id