from modules.bluetooth_module         import bt_recieveLine
from modules.drive_uploader_module    import drive_upload
from modules.mqtt_module              import mqtt_publish
from modules.safety_floater_check     import sf_check
from modules.last_violation_id_module import get_violation_id



import time

photo_counter = 0

print("##########################")
print("# Safety Monitor Started #")
print("##########################")

continue_counter = 0
while(True):
    print("Waiting for message to come...")
    try:
        #lat,long,speed,floater_id = bt_recieveLine()
        
        lat,long,speed,floater_id = 41.963581,12.798270,0.01,71
        print("Checking if there is an actual violation...")
        
        ### Incremental Name
        photo_counter += 1
        photo_counter = photo_counter % 10
        photo_name     = "pic"+str(photo_counter)+".jpg"
         
        is_boat = sf_check(photo_name=photo_name)
        
        if(is_boat):
            print("It is a Boat!")
            
            
            ## Upload Pic
            print("Uploading pic to Google Drive...")
            file_id = drive_upload(photo_name=photo_name)
            
            ## Everything should be fine
            assert(file_id != -1)
            ## Publish to AWS mqtt
            ### Create url
            url = "https://drive.google.com/uc?id="+str(file_id)
            ### Updating id to be used
            curr_id = get_violation_id()
            
            print("Notifying to DB...")
            mqtt_publish(lat,long,speed,floater_id,url,curr_id)
            
        else:
            print("No actual Boat detected")
            
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
