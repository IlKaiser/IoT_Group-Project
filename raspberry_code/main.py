import bluetooth_module
import safety_floater_check
import mqtt_module

import time

print("##########################")
print("# Safety Monitor Started #")
print("##########################")

continue_counter = 0
while(True):
    print("Waiting for message to come...")
    try:
        lat,long,speed,id = bluetooth_module.recieveLine()
        
        print("Checking if there is an actual violation...")
        is_boat = safety_floater_check.check()
        
        if(is_boat):
            print("It is a Boat! Notifying to DB...")
            '''
                TBD -> code for image uploading
            '''
            mqtt_module.mqtt_publish(lat,long,speed,id)
        else:
            print("No actual Boat detected")
            
    except:
        continue_counter += 1
        if(continue_counter is 3):
            print("Too Many Errors...")
            print("Exiting")
            quit()
        else:
            print("Error Detected! Continue... "+ str(continue_counter))
            print("Check if 'sudo rfcomm connect 0 <MAC of Bt>' is running")
            time.sleep(5)
            continue
