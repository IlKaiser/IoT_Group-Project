# Evaluation

## Network technologies
Given that Safety Floater will communicate with the Sea Station via LoraWAN, for our prototype we used a **Bluetooth module** in order to achieve an accurate measure of the **latency**. We have took into account the technological constraints of **LoraWan**, such as **throughput, latency and bandwith**.
<br><br>We also estimated the **latency** in the whole system by looking at: 
1. the total time needed to detect a **violation** on the **Nucleo board** for the activation of the **buzzer**
2. the total time needed to have the **boat picture** available on the Dashboard starting by the moment when the buzzer activates

To estimate these values we've used timestamps.

For the first measurement, we need to consider two delays:
   - the **I2C** delay for the communication between the Nucleo board and the Arduino board: about **0.001 seconds**
   - the delay to detect **false positives**: **1 second**

   So we have the following **estimates**:
   - Stm request of the current situation to the Arduino by I2C -> about **0.001 s**
   - Arduino response to the Stm by I2C -> about **0.001 s**
   - Delay to detect false positives -> **1 s**
   - Another request from the Stm of the current situation to the Arduino by I2C -> about **0.001 s**
   - Arduino response to the Stm by I2C -> about **0.001 s**

   **Total estimated time: 1.004 seconds.**

For the second measurement, we have the following **estimates**:
   - Stm request to the Arduino for the second violation check by I2C -> about **0.001 s**
   - Raspberry retrieves gps data from the Arduino by the Bluetooth module -> about **0.150 s**
   - Convolutional Neural Network evaluation -> about **3 s**
   - Sending of the picture url from the Raspberry to the AWS cloud by MQTT -> about **0.050 s**
   - HTTP/REST request from the NodeJS server to the cloud to retrieve the picture url in order to show the picture on the Dashboard -> about **0.050 s**

   **Total estimated time: 3.251 seconds.**
   
**Final estimated time: (1.004 seconds + 3.251 seconds)  = 4.255 seconds.**

For the second measurement, we've considered also a **LoraWAN communication** instead of a Bluetooth communication, so the second part of the estimation changed. The amount of bytes that we've sent from the Safety Floater to the Sea Station is **(Identifier of the floater, latitude and longitude)**, then we send **12 bytes** using LoraWAN. 
   
   - Stm request to the Arduino for the second violation check by I2C -> about **0.001 s**
   - Raspberry retrieves gps data from the Arduino by the LoraWAN module ( [Source](http://alessandroblason.it/2017/07/12/ttn-la-politica-accesso-equo/)) -> about **1.051 s**
   - Convolutional Neural Network evaluation -> about **3.000 s**
   - Sending of the picture url from the Raspberry to the AWS cloud by MQTT -> about **0.050 s**
   - HTTP/REST request from the NodeJS server to the cloud to retrieve the picture url in order to show the picture on the Dashboard -> about **0.050 s**

**Final estimated time: (1.004 seconds + 4.152 seconds) = 5.156 seconds.**


## Embedded devices

### IOT-LAB ESTIMATION
We measured the **power consumption** and **correction accuracy** of the stepper motor. The power consumption has been monitored on [Iot-lab](https://www.iot-lab.info/), on the basis of the **power management system** that we have defined [here](https://github.com/IlKaiser/IoT_Group-Project/blob/main/2nd_delivery.md#presentation-of-technical-work). Our system will be able to adapt itself with respect to the **acceleration** measured from the **MPU-9250** accelerometer along the **z-axis**, by switching in three possible modalities:

- **high mode**: high power consumption mode means that **the sea is calm** and it's possible to associate this power mode to the **white flag**
- **medium mode**: medium power consumption mode means that **the sea is in a intermediate status** and it's possible to associate this power mode to the **yellow flag**
- **low mode**: low power consumption mode means that **the sea is rough** and it's possible to associate this power mode to the **red flag**

During these experiments we kept the system running for **30 minutes** each, and we simulated the passive situation on which there wasn't a boat that crosses the safety line. 

Below we report the results obtained for each mode, also with a zoomed view, and also a final picture that shows the switching between modes, in particular from the high mode to the medium mode first, and from the medium mode to the low mode later:

### High mode

![img](https://github.com/IlKaiser/IoT_Group-Project/blob/main/evaluation/images/power-consumption-pm-1.png)

![img](https://github.com/IlKaiser/IoT_Group-Project/blob/main/evaluation/images/power-consumption-pm-1-zoom.png)

### Medium mode

![img](https://github.com/IlKaiser/IoT_Group-Project/blob/main/evaluation/images/power-consumption-pm-2.png)

![img](https://github.com/IlKaiser/IoT_Group-Project/blob/main/evaluation/images/power-consumption-pm-2-zoom.png)

### Low mode

![img](https://github.com/IlKaiser/IoT_Group-Project/blob/main/evaluation/images/power-consumption-pm-3.png)

![img](https://github.com/IlKaiser/IoT_Group-Project/blob/main/evaluation/images/power-consumption-pm-3-zoom.png)

### General mode (with switching)

![img](https://github.com/IlKaiser/IoT_Group-Project/blob/main/evaluation/images/pm-general.png)

More details on the evaluation of the power consumption can be found [here](https://github.com/IlKaiser/IoT_Group-Project/blob/main/evaluation/README.md).

### SIMULATED ENVIRONMENT WITH THE PROTOTYPE 

For the third deliver we've realized a **prototype** an we decided to measure the power consumption. We kept running the floater for **60 minutes** for each **power consumption mode** and for the power supply we used a powerbank and we've simulated a boat that crossed the safety line every **5 minutes**.

This is the setup:

![img](https://github.com/IlKaiser/IoT_Group-Project/blob/main/imgs/prototype.jpeg)

Below we can see results for each power consumption mode:

**HIGH MODE**

Voltage: 5 V <br>
Current Consumption: 153 mAh <br>
**Power Consumption: 7 hours 50 minutes using a 9V 1200 mAh battery without considering solar panel**

**MEDIUM MODE**

Voltage: 5 V <br>
Current Consumption: 138,65 mAh <br>
**Power Consumption: 8 hours 45 minutes using a 9V 1200 mAh battery without considering solar panel**

**LOW MODE**

Voltage: 5 V <br>
Current Consumption: 137,5 mAh <br>
**Power Consumption: 8 hours 55 minutes using a 9V 1200 mAh battery without considering solar panel**

## Detection accuracy with CNN
We measured the accuracy and performances of the **Convolutional Neural Network** we trained for boat recognition. The dataset used is **ARGOS**, and the results are in the picture below:

<img src="https://github.com/IlKaiser/IoT_Group-Project/blob/main/image_recognition/plots.jpeg"  width=500/>

## End-user point of view
From an end-user point of view the response time is conditioned by **LoraWAN**, by the **bandwith of the channel linking the Sea Station to the cloud**, and by the type of **protocol** used (**MQTT**). From a quantitative point of view, we will use **timestamps** at the generation of the packet and one at the receiving of the packet.  

## Previous version
The previous version of the document can be found in the [previous release](https://github.com/IlKaiser/IoT_Group-Project/releases/tag/2.0) of the project.
