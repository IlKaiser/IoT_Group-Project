from modules.bluetooth_module      import bt_recieveLine
from modules.drive_uploader_module import drive_upload
from modules.mqtt_module           import mqtt_publish
from modules.safety_floater_check  import sf_check



import threading
import time

photo_counter = 0

print("##########################")
print("# Safety Monitor Started #")
print("##########################")

continue_counter = 0
lat,long,speed,floater_id = 0,0,0,0

def bt_reciever_forever():
    while(True):
        print("Waiting for message to come...")
    
        lat,long,speed,floater_id = bt_recieveLine()
        
        print("Checking if there is an actual violation...")
        
        threading.Thread(target=checker).start()
        

def checker(): 
    ### Incremental Name
    photo_counter += 1
    photo_name     = "pic_{photo_counter}.jpg"
        
    is_boat = sf_check(photo_name=photo_name)
    
    if(is_boat):
        print("It is a Boat!")
        
        
        ## Upload Pic
        print("Uploading pic to Google Drive...")
        file_id = drive_upload(photo_name=photo_name)
        
        ## Everything should be fine
        assert(file_id != -1)
        ## Publish to AWS mqtt
        print("Notifying to DB...")
        mqtt_publish(lat,long,speed,floater_id,file_id)
        
    else:
        print("No actual Boat detected")
        
            

    
while(True):
    try:
        th = threading.Thread(target=bt_reciever_forever)
        th.start()
        th.join()
        
    except Exception as e:
        
        continue_counter += 1
        if(continue_counter is 3):
            print("Too Many Errors...")
            print("Exiting")
            quit()
        else:
            print("Error Detected! Continue... "+ str(continue_counter))
            print("\033[1;37;43m" +str(e)+"\033[0m")
            if(e.args[0]is 2):
                print("Check if 'sudo rfcomm connect 0 <MAC of Bt>' is running")
            time.sleep(5)
            continue
