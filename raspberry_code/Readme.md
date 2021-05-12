# Raspberry Setup[WIP]


## Before you start 
- Make sure you download the prebuilt .h5 model for machine learning from [here](https://drive.google.com/file/d/1-JYH7f6np6WcKjgFJ4iTFM7xC8h3vFz8/view?usp=sharing), you rename it **model.h5**
and you place it in the model folder
- Make sure to get aws **root certificate** (rootCA.pem), your **certificate** (cert.pem) and **private key** (private.key) for IoT Core and place it in certificates folder (more info [here](https://docs.aws.amazon.com/iot/latest/developerguide/iot-moisture-create-thing.html))
- Install pip requirements

## Install pip requirements
Run:
```
pip3 install -r requirements.txt
```
to automatically install all needed modules.
If you have issue installing tensorflow follow this [guide](https://qengineering.eu/install-tensorflow-2.4.0-on-raspberry-64-os.html), delete tensorflow from requirements and run the command again.

## Before you execute the program
### Bluetooth setup
Execute:
```
sudo rfcomm connect 0 <MAC of Bt adapter>
```
to connect the Arduino Bluetooth-Adapter tio Raspberry and Setup hc-05 serial protocol.

## Start program
```
python3 main.py
```
more info on the code [here](./Code.md).
