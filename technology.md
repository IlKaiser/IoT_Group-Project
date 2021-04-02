# Technology

## Core: Three main parts
- Smart Floater
- Sea Station
- Web Dashboard

### Smart Floater
It is the main component of our project: its main purpose is to detect illegal transit of boats over the safety line. When a violation is detected, the sensors data are sent to the **Sea Station (Ground server)**.
Are used for the detections **proximity sensors**, a **dc motor** grants that the floater has the sensor correctly oriented, **gps sensor** detects the current position of the floater, **solar panels** permit a longer battery life.
A **radio module** would be used in real life scenario to transmit data (its model in practice is a common bluetooth module).
The sensors are controlled by a Nucleo Board (nucelo-f401re) with [RIOT OS](https://www.riot-os.org/).

### Sea station
It is the **ground server** of our system: it is a content aggregator of many Smart Floaters in a fixed range.It elaborates data from floaters and sends it to the remote server and database. Every Floater has a unique id for identification. It also notify the eventual violation to the Coast Guard.

### Web Dashboard
It is the highest level component of our project. It is composed by the remote server (using **Aws services and nodejs**) and by the frontend (web page and eventually an app). It displays the datas retriven from database (**DynamoDB**).

## Sensors
We chose our sensors based on RIOT OS API compatibilty.

### MakerHawk MPU-9250 9DOF Module 9 Axis Gyroscope Accelerometer Magnetic Field Sensor
![Img](https://images-na.ssl-images-amazon.com/images/I/61qPZjhQq2L._AC_SL1200_.jpg)
- 9-axis: 3-axis gyroscope + 3-axis accelerometer + 3-axis magnetic field sensor
- Power supply: 3-5v (low dropout internal regulator)
- Communication: standard IIC / SPI communication protocol
- 16-bit AD data output of the 16-bit chip converter
- Acceleration range: ± 2 ± 4 ± 8 ± 16g
- Magnetic field: ± 4800uT
- Using PCB Immersion Gold, soldering process to ensure quality
- Pin spacing: 2.54mm
- Module size: 15mm * 25mm

It is used for movement detection and orientation.

### GPS ?
### Solar Panel
![Img](https://images-na.ssl-images-amazon.com/images/I/51t9Z3nh4uL._SL1001_.jpg)

## Communication
- **Smart Floater-to-Sea Station Communication** is via Radio in our original design, in practice we will use Bluetooth with this antenna. Prediodically are sent packets with the status of the sensor. 
- **Sea Station-to-Cloud Communication**  via simple wi-fi network

## The final picture

