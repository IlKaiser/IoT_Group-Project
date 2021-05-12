#  Raspberry code Explained

The code is dividied in modules, each for a specific task.

## Bluetooth module

Recieves an ecoded string with latiude, longitude, speed and floater id from Arduino with Bluetooth and hc05 protocol; decodes the string and returns the parsed value.
The main part is illustrated here:
```
    bluetoothSerial = serial.Serial( "/dev/rfcomm0", baudrate=9600 )
    line = bluetoothSerial.readline()
   
    string = line.decode("utf-8")
    
    lat   = float(string.split(";")[0])
    long  = float(string.split(";")[1])
    speed = float(string.split(";")[2])
    floater_id    = int(string.split(";")[3])
    
    return lat,long,speed,floater_id

```

## Boat Detection module

The most important part of the program. It takes a picture with the Raspberry Camera and analyzes it with the CNN. If a Boat is detected returns True, else False.
```
    # Start camera
    camera = PiCamera()
    camera.resolution = (2592, 1944)
    # Take pic
    print("Taking pic...")
    # Camera warm-up time
    sleep(2)
    camera.capture(photo_name)
    print("Pic taken,start processing...")

    # Tf analysis
    img = tf.keras.preprocessing.image.load_img('pic.jpg',target_size=(128,128))
    img_array = tf.keras.preprocessing.image.img_to_array(img)
    img_array = tf.expand_dims(img_array,0)
    predictions_single = model.predict(img_array)
    
    
    # Code Omitted [...]
    
    # Compute Response
    if response == 0:
        return False
    return True
```

## Drive Uploader module

Uploads taken picture on Drive and retrives link in order to add photo drive link to DB via Mqtt.
```
        file_metadata = {'name': photo_name,'parents':["1euot5RlQOXgl7bV8yZWktXFMeZJ8HsuG"]}
        media = MediaFileUpload(photo_name, mimetype='image/jpeg',resumable=True)
        file = service.files().create(body=file_metadata,
                                        media_body=media,
                                        fields='id').execute()

        print("Upload Complete!")
        
        file_id = file.get('id')
        print("Upload of "+ photo_name+" succeded!")
        print('File ID: %s' % file_id)
        print("Setting permssions...")
        
        # Code Omitted [...]
       
        return file_id
```
## MQTT Publisher module

Publishes data taken from Bluetooth (latiude, longitude, speed and floater id) and drive file id to AWS Mqtt.
```
    message = {"lat" : lat,"long": long,"speed" : speed,"floater_id": floater_id, "file_id": file_id }
    mqtt_connection.publish(topic=TOPIC, payload=json.dumps(message), qos=mqtt.QoS.AT_LEAST_ONCE)
    print("Published: '" + json.dumps(message) + "' to the topic: " + TOPIC)
   
```
## Main module
Puts all modules together:
- Waits for Bluetooth message to arrive for analysizing a possible violation
- Takes a picture and evaluates it with the pre-trained Neural Network
- If an actual violation is detected, Image of violation is uploaded to drive, and all useful informations are published to Mqtt
