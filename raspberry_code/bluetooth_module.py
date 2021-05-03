import serial
from time import sleep
###########################################################################
#                  Before starting this script run:                       #
#              sudo rfcomm connect 0 xx:xx:xx:xx:xx:xx                    #
###########################################################################
def recieveLine():
    bluetoothSerial = serial.Serial( "/dev/rfcomm0", baudrate=9600 )
    ##bluetoothSerial.write( str(count) )
    print(bluetoothSerial.readline())