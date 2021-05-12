# Setup

## Devices

### STM32 Nucleo-64 F401RE board
You need to have available an **STM-32 Nucleo-64 F401RE board**, [here](https://www.st.com/en/evaluation-tools/nucleo-f401re.html) you can find info about it.

### Arduino Uno
You need to have available an **Arduino Uno board**, [here](https://store.arduino.cc/arduino-uno-rev3) you can find info about it.

### Raspberry pi 4
You need to have available an **Raspberry pi 4 board**, [here](https://www.raspberrypi.org/products/raspberry-pi-4-model-b/) you can find info about it.

## Arduino Uno setup

### Arduino IDE
You need to download the **Arduino IDE** from [here](https://www.arduino.cc/en/software).

### TinyGPSPlus library
You have to download the **TinyGPSPlus library** from [here](https://github.com/mikalhart/TinyGPSPlus/archive/refs/tags/v1.0.2b.zip), rename the folder in the zip as "TinyGPSPlus" and put it in the **libraries folder** of the Arduino folder.

## STM32 Nucleo-64 F401RE setup

### The RIOT operating system
We need to install the **RIOT operating system**, [here](https://github.com/RIOT-OS/Tutorials#tutorials-for-riot) you can find a tutorial for the installation.

## Raspberry pi 4 setup
For the Raspberry pi 4 setup you can check [here](https://github.com/IlKaiser/IoT_Group-Project/blob/main/raspberry_code/Readme.md).

## Server setup

### Installing NodeJS and preparing the server
You need to install **NodeJS** in your operating system in order to have the web dashboard working correctly, [here](https://nodejs.org/en/) you can find details.
After this, you have to check if you have **npm** installed, otherwise you can install it from [here](https://www.npmjs.com/get-npm).
You also need to require the **.env** file, that has to placed in the dashboard directory. More info about the .env file will come later.

### Nodemon
After this, you can install **nodemon** (not mandatory), a very useful package to run our NodeJS server, with this command: **npm install -g nodemon** if you are on Windows, or **sudo npm install -g nodemon** if you are on Linux/Ubuntu.

## Data storage

### MongoDB
Using free tier on site it is possible to have a small db instance with 512Mb of available space and up to 100 users connected at the same time, which is perfect for our scenario. On MongoDB are saved user_names,password (generated from the SHA256 of the actual password concatenated with a random salt), and salt for every user.
To setup free tier see [here](https://docs.atlas.mongodb.com/getting-started/).
We decided, for safety and concistency reasons, that is not possible for users to register from the frontend, so every new user is added manually. We used the [command line method](https://docs.mongodb.com/guides/server/insert/) with Linux.
### AWS, Iot-Core and DynamoDB
Then, you need so sign in in [AWS](https://aws.amazon.com/education/awseducate/), in order to get access to **Iot-Core** and **DynamoDB** services. Once you're on Iot-Core, you need to create a **thing** in order to get your **certificate**, **root certificate** and **private key**. Here you can find a [tutorial](https://docs.aws.amazon.com/iot/latest/developerguide/iot-moisture-create-thing.html). Then you need to add into the **.env** file **the access key id, the secret access key and the session token** provided by AWS, to be able to receive messages from the devices and to query the DynamoDB database. So at this point the .env should look like this:

```
AWS_ACCESS_KEY_ID=YOUR_AWS_ACCESS_KEY_ID
AWS_SECRET_ACCESS_KEY=YOUR_AWS_SECRET_ACCESS_KEY
AWS_SESSION_TOKEN=YOUR_AWS_SESSION_TOKEN
```

**NOTE!** The access key id, the secret access key and the session token provided by AWS expire after a while, so you have to manually re-set them in the .env file when this happens.

Then you need to **subscribe** to the **data/seastation/+** MQTT topic, where '+' stands for 'at least one character' from the syntax of regular expressions: in our case it will represent the **seastation id**. 

![img](https://github.com/IlKaiser/IoT_Group-Project/blob/main/imgs/subscribe_aws.png)

Then by going in the Rules section you have to create a **rule** for violations, specifying as topic data/seastation/<seastation_id>, and as operation you need to choose "Insert a message into a DynamoDB table". Here you can find a [tutorial](https://docs.aws.amazon.com/iot/latest/developerguide/iot-ddb-rule.html).

**Violation rule**:

![img](https://github.com/IlKaiser/IoT_Group-Project/blob/main/imgs/violation_rule.jpeg)

So it is clear that we will create a **DynamoDb table**: particularly, each entry of the table is made up of the **Unix timestamp** associated to the creation of that entry, the **seastation id** of the Sea Station that send the violation to AWS by MQTT, the **device_data**, which consists of the **seastation id**, the **violation id** of the violation detected, the **latitude** and **longitude** of the floater that send the indication of the violation to the Sea Station and the **url of the picture** taken by the Raspberry pi Camera. 

## Launching the application

- Download the repository from [here](https://github.com/IlKaiser/IoT_Group-Project/archive/refs/heads/main.zip), or clone it with **git clone https://github.com/IlKaiser/IoT_Group-Project.git**
- Open a terminal in the dashboard folder
- Launch **npm install**
- Launch the nodeJS server with **node index.js** or **nodemon** (if you installed it)
- Open the sensor.ino sketch in the arduino_code folder
- **Upload** the sketch
- Open another terminal in the nucleo_code folder of the project
- Launch the application by flashing the code into the STM-32 board with **make BOARD=nucleo-f401re flash term**
- For **Raspberry Pi 4** you can check all details [here](https://github.com/IlKaiser/IoT_Group-Project/blob/main/raspberry_code/Readme.md).

The server will be available at **http://localhost:8080/**.
