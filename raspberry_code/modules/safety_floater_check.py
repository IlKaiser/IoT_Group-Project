import os
from picamera import PiCamera
import tensorflow as tf
from time import sleep

# Stfu Keras
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3'

classes = ["No_Boat","Boat"]

'''
    True if it is a Boat, False elsewhere
'''
def sf_check(photo_name="pic.jpg"):
    #Import model
    model = tf.keras.models.load_model('../model/model.h5')
    print("Model correctly loaded...")

    #Start camera
    camera = PiCamera()
    camera.resolution = (2592, 1944)
    #Take pic
    print("Taking pic...")
    # Camera warm-up time
    sleep(2)
    camera.capture(photo_name)
    print("Pic taken,start processing...")

    #Tf analysis
    img = tf.keras.preprocessing.image.load_img('pic.jpg',target_size=(128,128))
    img_array = tf.keras.preprocessing.image.img_to_array(img)
    img_array = tf.expand_dims(img_array,0)
    predictions_single = model.predict(img_array)
    print(predictions_single)
    response = int(predictions_single[0][0])
    print(classes[response])
    print("Done!")
    if response == 0:
        return False
    return True