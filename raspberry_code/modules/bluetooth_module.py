import serial
from time import sleep

###########################################################################
#                  Before starting this script run:                       #
#              sudo rfcomm connect 0 98:D3:31:F9:95:52                    #
###########################################################################

def bt_recieveLine():
    
    bluetoothSerial = serial.Serial( "/dev/rfcomm0", baudrate=9600 )
    line = bluetoothSerial.readline()
   
    string = line.decode("utf-8")
    
    lat   = float(string.split(";")[0])
    long  = float(string.split(";")[1])
    speed = float(string.split(";")[2])
    floater_id    = int(string.split(";")[3])
    
    print("Recieved "+str(lat),str(long),str(speed),str(floater_id))
    return lat,long,speed,floater_id