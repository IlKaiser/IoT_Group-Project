# Evaluation

## Network technologies
Given that Safety Floater will communicate with the Sea Station via LoraWAN, for our prototype we used a **Bluetooth module** in order to achieve an accurate measure of the **latency**. When we will perform final tests, we will take into account the technological constraints of LoraWan, such as **throughput, latency and bandwith**.

## Embedded devices
The power consumption will be monitored in all possible states of the Safety Floater, and in order to do this we will use a **power meter**. In particular, we will measure the battery consumption and correction accuracy of the stepper motor.

## Detection accuracy with CNN
We measured the accuracy and performances of the Convolutional Neural Network we trained for boat recognition, and the results are in the picture below:

![img](https://github.com/IlKaiser/IoT_Group-Project/blob/main/ML/plots.jpeg)

## End-user point of view
From an end-user point of view the response time is conditioned by **LoraWAN**, by the **bandwith of the channel linking the Sea Station to the cloud**, and by the type of **protocol** used (**MQTT**). From a quantitative point of view, we will use **timestamps** at the generation of the packet and one at the receiving of the packet.  

## Previous version
The previous version of the document can be find in the [previous release](https://github.com/IlKaiser/IoT_Group-Project/releases/tag/1.0) of the project.
