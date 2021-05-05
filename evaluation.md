# Evaluation

## Network technologies
Given that Safety Floater will communicate with the Sea Station via LoraWAN, the evaluation of the performance of the network technologies will be measured with Iot-lab, because there we have the right tools compared to a generic user. We want to notice that we are not able to analyze the network consumption of physical devices of our personal Safety Floater.

## Embedded devices
During the development phase we will use Iot-lab to have an estimation of the power consumption of the embedded device (Nucleo f401re), because even if on Iot-lab are not available our specific sensors we can obtain a rough result. During the final testing we will use a voltage measure sensor in order to monitor the real consumption.

## End-user point of view
From an end-user point of view the response time is conditioned by LoraWAN, by the bandwith of the channel linking the Sea Station to the cloud, and by the type of protocol used (e.g.: HTTP-REST / MQTT). From a quantitative point of view, we will use timestamps at the generation of the packet and one at the receiving of the packet.  

## Previous version
The previous version of the document can be find in the [previous release](https://github.com/IlKaiser/IoT_Group-Project/releases/tag/1.0) of the project.
