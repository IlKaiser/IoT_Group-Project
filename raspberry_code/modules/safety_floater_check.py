import os
from picamera import PiCamera
import tensorflow as tf
from time import sleep

camera = PiCamera()
camera.resolution = (2592, 1944)
camera.hflip = True
camera.vflip = True

# Stfu Keras
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3'

classes = ["No_Boat","Boat"]

'''
    True if it is a Boat, False elsewhere
'''
def sf_check(photo_name="pic.jpg"):
    #Import model
    model = tf.keras.models.load_model('model/model.h5')
    print("Model correctly loaded...")

    #Start camera
    
    #Take pic
    print("Taking pic...")
    
    # Camera warm-up time
    sleep(2)
    camera.capture(photo_name)
    print("Pic taken,start processing...")

    #Tf analysis
    img = tf.keras.preprocessing.image.load_img(photo_name,target_size=(128,128))
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

#Tf analysis
def quick_test():
    import matplotlib.pyplot as plt


    model = tf.keras.models.load_model('../model/model.h5')
    print("Model correctly loaded...")


    fig, axs = plt.subplots(3, 3)

    def quick_plot(name="boat.jpg",x=0,y=0):
        img = tf.keras.preprocessing.image.load_img(name,target_size=(128,128))
        img_array = tf.keras.preprocessing.image.img_to_array(img)
        img_array = tf.expand_dims(img_array,0)
        predictions_single = model.predict(img_array)
        print(predictions_single)
        response = int(predictions_single[0][0])
        print(classes[response])
        print("Done!")
        axs[x, y].set_title(classes[response])
        axs[x, y].imshow(img)


    quick_plot(name="../tests/boat.jpg",x=0,y=0)
    quick_plot(name="../tests/boat1.jpg",x=0,y=1)
    quick_plot(name="../tests/boat2.jpg",x=0,y=2)
    quick_plot(name="../tests/boat3.jpg",x=1,y=0)
    quick_plot(name="../tests/boat4.jpg",x=1,y=1)
    quick_plot(name="../tests/boat5.jpg",x=1,y=2)
    quick_plot(name="../tests/boat6.jpg",x=2,y=0)
    quick_plot(name="../tests/boat7.jpg",x=2,y=1)
    quick_plot(name="../tests/sea3.jpg",x=2,y=2)

    for ax in axs.flat:
        ax.set(xlabel='x-label', ylabel='y-label')

    # Hide x labels and tick labels for top plots and y ticks for right plots.
    for ax in axs.flat:
        ax.label_outer()

    plt.show()
quick_test()
