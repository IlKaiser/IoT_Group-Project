# Evaluation

## Network technologies
Given that Safety Floater will communicate with the Sea Station via LoraWAN, for our prototype we used a **Bluetooth module** in order to achieve an accurate measure of the **latency**. When we will perform final tests, we will take into account the technological constraints of LoraWan, such as **throughput, latency and bandwith**.
<br><br>For this delivery, we also estimated the **latency** in the whole system by looking at: 
1. the total time needed to detect a **violation** on the **Nucleo board** for the activation of the **buzzer**
2. the total time needed to have the **boat picture** available on the Dashboard starting by the moment when the buzzer activates

1. For the first measurement, we need to consider two delays:
   - the **I2C** delay for the communication between the Nucleo board and the Arduino board: about **0.001 seconds**
   - the delay to detect **false positives**: **1 second**

   So we have the following **estimates**:
   - Stm request of the current situation to the Arduino by I2C -> about **0.001 s**
   - Arduino response to the Stm by I2C -> about **0.001 s**
   - Delay to detect false positives -> **1 s**
   - Another request from the Stm of the current situation to the Arduino by I2C -> about **0.001 s**
   - Arduino response to the Stm by I2C -> about **0.001 s**

   **Total estimated time: 1.004 seconds.**

2. For the second measurement, we have the following **estimates**:
   - Stm request to the Arduino for the second violation check by I2C -> about **0.001 s**
   - Raspberry retrieves gps data from the Arduino by the Bluetooth module -> about **0.150 s**
   - Convolutional Neural Network evaluation -> about **3 s**
   - Sending of the picture url from the Raspberry to the AWS cloud by MQTT -> about **0.050 s**
   - HTTP/REST request from the NodeJS server to the cloud to retrieve the picture url in order to show the picture on the Dashboard -> about **0.050 s**

   **Total estimated time: 3.251 seconds.**
   
**Final estimated time: (1.004 seconds + 3.251) seconds = 4.255 seconds.**

## Embedded devices
The power consumption will be monitored in all possible states of the Safety Floater, and in order to do this we will use a **power meter**. In particular, we will measure the battery consumption and correction accuracy of the stepper motor.

## Detection accuracy with CNN
We measured the accuracy and performances of the **Convolutional Neural Network** we trained for boat recognition, and the results are in the picture below:

![img](https://github.com/IlKaiser/IoT_Group-Project/blob/main/ML/plots.jpeg)

## End-user point of view
From an end-user point of view the response time is conditioned by **LoraWAN**, by the **bandwith of the channel linking the Sea Station to the cloud**, and by the type of **protocol** used (**MQTT**). From a quantitative point of view, we will use **timestamps** at the generation of the packet and one at the receiving of the packet.  

## Previous version
The previous version of the document can be found in the [previous release](https://github.com/IlKaiser/IoT_Group-Project/releases/tag/1.0) of the project.
