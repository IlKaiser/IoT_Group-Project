# Technology

## Core: Three main parts
- Smart Floater
- Sea Station
- Web Dashboard

### Smart Floater
It is the main component of our project: its main purpose is to detect illegal transit of boats over the safety line. When a violation is detected, the sensors data are sent to the **Sea Station (Ground server)**.
Are used for the detections **ultrasonic proximity sensors**, a **stepper motor** grants that the floater has the proximity sensor correctly oriented, **gps sensor** detects the current position of the floater, **solar panels** permit a longer battery life and **buzzer** for audio notification of swimmers.
A **LoraWan module** would be used in real life scenario to transmit data (its model in practice is a common bluetooth module).
The sensors are controlled by a [Nucleo Board](https://www.st.com/en/evaluation-tools/nucleo-f401re.html) (nucleo-f401re) with [RIOT OS](https://www.riot-os.org/), and measurements are made with an [Arduino Uno](https://store.arduino.cc/arduino-uno-rev3) board, because we have an easier integration of sensors.
The communication between the two boards is done with the I2C protocol: the Nucleo board is the master, it periodically asks for sensor data to the Arduino Uno, which is the slave. 
For more informations about the circuit, see [here](https://github.com/IlKaiser/IoT_Group-Project/blob/main/circuit/README.md).

### Sea station
It is the **ground server** of our system: it is a content aggregator of many Smart Floaters in a fixed range. It elaborates data from floaters and sends it to the remote server and database. Every Floater has a unique id for identification. It also notify the eventual violation to the Coast Guard.
We used a [Raspberry Pi](https://www.raspberrypi.org/products/raspberry-pi-4-model-b/) to take pictures of the possible violation, for a further evaluation with a Convolutional Neural Network in order to make sure wheter there is an actual boat over the safety line or not. If an actual violation is detected, all data from the Floater are sent to AWS via MQTT.

### Web Dashboard
It is the highest level component of our project. It is composed by the remote server (using **AWS services and nodejs**) and by the frontend (web page and eventually an app). It displays the datas retrived from database (**DynamoDB**).

## Sensors
We chose our sensors based on our system requirements.

### MakerHawk MPU-9250 9DOF Module 9 Axis Gyroscope Accelerometer Magnetic Field Sensor
<img src="https://images-na.ssl-images-amazon.com/images/I/61qPZjhQq2L._AC_SL1200_.jpg"  width=400/>

- 9-axis: 3-axis gyroscope + 3-axis accelerometer + 3-axis magnetic field sensor
- Power supply: 3-5v (low dropout internal regulator)
- Communication: standard IIC / SPI communication protocol
- 16-bit AD data output of the 16-bit chip converter
- Acceleration range: ± 2 ± 4 ± 8 ± 16g
- Magnetic field: ± 4800uT
- Using PCB Immersion Gold, soldering process to ensure quality
- Pin spacing: 2.54mm
- Module size: 15mm * 25mm

It is used for movement detection and orientation, and for power management.

### Ultrasonic Proximity Sensor HC-SR04
<img src="https://images-na.ssl-images-amazon.com/images/I/71Ht9ic07xL._AC_SL1500_.jpg"  width=400/>

- Working Voltage: DC 5V
- Working Current: 15mA
- Working Frequency: 40Hz
- Max Range: 4m
- Min Range: 2cm
- Measuring Angle: 15 degree
- Trigger Input Signal: 10µS TTL pulse
- Echo Output Signal Input TTL lever signal and the range in proportion
- Dimension 45 * 20 * 15mm

It is used for violation detection.

### GPS BN-880
<img src="https://github.com/IlKaiser/IoT_Group-Project/blob/main/imgs/gps.jpg"  width=400/>

- GPS Module,Dual Module Compass
- With electronic compass IC HMC5883L
- 1.25mm spacing between the 6pins patch seat
- Receiving Format: GPS,GLONASS,Galileo,BeiDou,QZSS and SBAS
- Receiving Channel: 72 Searching Channel
- Output Frequency: 1Hz-10Hz,Default 1Hz
- Support Rate:4800bps to 115200bps,Default 38400dps
- UART Interface: UART Port: TXDA and RXDA
- Level Positioning Precison: 2m At Open Wind
- Speed Precison: 0.1 m/s (Without Aid)
- Acceleration Precison: 0.1 m/s (Without Aid)
- Power Supply: DC Voltage 2.8V~6.0V,Typical:3.3V or 5.0V
- Consumption: Capture 50mA@5V
- Working Temperature:-40℃-+85℃
- Storage Temperature:-40℃-+85℃
- Size:28mm x 28mm x 10mm
- Weight:10g

It is used for getting the last known position of the floater.

### Solar Panel

<img src="https://images-na.ssl-images-amazon.com/images/I/51t9Z3nh4uL._SL1001_.jpg"  width=400/>

## Actuators

### Buzzer

<img src="https://images-na.ssl-images-amazon.com/images/I/51-E3qYe8DL._AC_SL1100_.jpg"  width=400/>

It is used for having a sound notification where a possible violation is detected.

### Stepper Motor

<img src="https://canary.contestimg.wish.com/api/webimage/5dbbef32dc5d840db6605304-large.jpg?cache_buster=85facc48bf5a57e2d4572fa334a31fb5"  width=400/>

It is used for correcting the orientation of the floater with respect to the gyroscope/compass data.

Below we show an image of the spotter of the floater:

<img src="https://github.com/IlKaiser/IoT_Group-Project/blob/main/imgs/1.png"  width=400/>


## Communication
- **Smart Floater-to-Sea Station Communication** is via LoraWan in our original design, in practice we will use Bluetooth. Packets are sent with the status of the sensor if a possible violation is detected. 
- **Sea Station-to-Cloud Communication** via MQTT communication.

## The final picture

![Img](https://github.com/IlKaiser/IoT_Group-Project/blob/main/imgs/picture.png )
