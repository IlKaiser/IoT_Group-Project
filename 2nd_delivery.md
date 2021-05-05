# Second delivery

## Comments from first delivery
1. Studying sensors' performances in variuos situations, e.g. ultrasonic proximity sensor with temperature and humidity of air
2. Lora-WAN antenna dimensioning and positioning (how does the sea movement affects the performances of Lora-WAN protocol?)
3. How to solve the problem of detecting false positives that could derive from our solution
4. On Iot-lab, we need to develop a testing plan

Now for each of the previous points we explain our decisions:

1. In order to monitor the temperature and humidity of air we will use Weather Services API, notifying in case of very adverse measurements our floaters to enable the power safe mode.
2. Our use of Lora-WAN will be in the range of 150 metres, so the positioning and dimensioning of the antenna is not a relevant problem.
3. In order to solve this problem, the first approach is verifying if the detected object remains in the detection field for more than a fixed amount of seconds.
4. More info later..

## Changes

### Concept
No changes has been done.

### Architecture and technology
For the first delivery, our architecture was totally based on STM-32 devices. Now, since we wanted to use specific sensors (GPS, camera, other sensors) and we wanted to minimize the latency of the report notification, we changed slightly the architecture and the technology. Below we show a picture:

<img src="https://github.com/IlKaiser/IoT_Group-Project/blob/main/imgs/SCHEMA.png"  width=1500/>

Now we present the specific components.

#### Arduino-UNO
Attached to the Floaters, the [Arduino Uno](https://store.arduino.cc/arduino-uno-rev3) communicates with STM-32 with I2C protocol. It samples the GPS, the accelerometer and the proximity sensor. After sampling and some data-processing, it sends data to the STM-32 board.

#### STM-32
Attached to the Floaters, the [STM-32](https://www.st.com/en/evaluation-tools/nucleo-f401re.html) communicates with Arduino-UNO with I2C protocol. It manages actuators, which are the Stepper motor and the buzzer. It also uses Lora-WAN protocol to communicate with the Sea Station, in order to send data received by Arduino-UNO.

#### Sea Station (with Raspberry pi 4)
We decided to use a [Raspberry pi](https://www.raspberrypi.org/products/raspberry-pi-4-model-b/) for the Sea Station, because it is a portable device that offers great performances similar to a PC with low power consumption. It uses Lora-WAN protocol to communicate with the STM-32 board in order to receive reports and to take pictures, with the goal of verifying wheter there was a true violation or not, by using a Neural Network.

#### Cloud service
There are no changes for this part.

### Evaluation
No changes has been done.

## Presentation of technical work
For the technical work, we present a list of the functionalities we implemented in different devices.

### Safety Floater
Safety Floater has been divided in two parts, so in two devices, that are the Arduino Uno board and the Nucleo Stm-32 board. The communication between them has been implemented with the I2C protocol, where the Nucleo board is the master and the Arduino Uno is the slave: in particular, the Nucleo board periosically sends request messages to the Arduino board, in order to retrieve informations about sensors and system. The functionalities provided are:

- **boat detection mechanism**: by using the ultrasonic proximity sensor and the buzzer, the system can detect whether a boat is transiting near the safety line or not. In order to avoid false positives, we introduced a smart measuring system based on delay
- **orientation control mechanism**: by using the gyroscope/compass and the stepper motor, we implemented a control system that is able to correct the orientation of the proximity sensor
- **power management mechanism**: by using the accelerometer, we implemented a power management system based on varying the sampling periods for sensors with respect to the acceleration measured by the accelerometer
- **system info mechanism**: by using the gps, we implemented a mechanism able to retrieve the last known position of the floater
- **sea-ground communication system**: we implemented the communication between the Safety Floater and the Sea Station by using a bluetooth module, in order to emulate Lorawan communication 

### Sea station
Sea station has been implemented with Raspberry pi, because we have no power management problems, and we need more computational power to handle the camera and the image recognition system. We have three main modules:

- **Ground-sea communication** via serial bluetooth, which receives data of a possible violation
- **Boat recognition system**, that takes pictures of the current situation and analyzes them with a Convolutional Neural Network in order to ensure that there is an actual violation and avoid false positives
- **Ground-cloud communication**, that periodically sends data of sensors to the cloud via MQTT if there is a violation

## Missing functionalities
- Optimizing power management
- Improving latency
- Development of a better web interface

## Evaluation
For the evaluation:
- we estimate the power consumption of single subsytems
- we evaluated performances of the CNN
- we estimate the latency in the whole system
- we estimate the battery consumption and correction accuracy of the stepper motor.

## Evaluation for the 3rd delivery
We want to obtain the correct estimation of the previous parameters with the real prototype.

