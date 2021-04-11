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
Attached to the Floaters, it communicates with STM-32 with I2C protocol. It samples the GPS, the accelerometer and the proximity sensor. After sampling and some data-processing, it sends data to the STM-32 board.

#### STM-32
Attached to the Floaters, it communicates with Arduino-UNO with I2C protocol. It manages actuators, which are the DC motor, the Servo motor and buzzers. It also uses Lora-WAN protocol to communicate with the Sea Station, in order to send data received by Arduino-UNO.

#### Sea Station (with Raspberry pi 4)
We decided to use a Raspberry pi for the Sea Station, because it is a portable device that offers great performances similar to a PC with low power consumption. It uses Lora-WAN protocol to communicate with the STM-32 board in order to receive reports and to take pictures, with the goal of verifying wheter there was a true violation or not, by using a Neural Network.

#### Cloud service
There are no changes for this part.

### Evaluation
No changes has been done.
