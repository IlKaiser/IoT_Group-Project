# Setup [WIP]

## The RIOT operating system
We need to install the **RIOT operating system**, [here](https://github.com/RIOT-OS/Tutorials#tutorials-for-riot) you can find a tutorial for the installation.

## STM32 Nucleo-64 F401RE board
You need to have available an **STM-32 Nucleo-64 F401RE board**, [here](https://www.st.com/en/evaluation-tools/nucleo-f401re.html) you can find info about it.

## Arduino Uno
You need to have available an **Arduino Uno board**, [here](https://store.arduino.cc/arduino-uno-rev3) you can find info about it.

## Raspberry pi 4
You need to have available an **Raspberry pi 4 board**, [here](https://www.raspberrypi.org/products/raspberry-pi-4-model-b/) you can find info about it.

## Arduino IDE + TinyGPSPlus library
You need to download the Arduino IDE from [here](https://www.arduino.cc/en/software), and then you have to download the **TinyGPSPlus library** from [here](https://github.com/mikalhart/TinyGPSPlus/archive/refs/tags/v1.0.2b.zip), rename the folder in the zip as "TinyGPSPlus" and put it in the **libraries folder** of the Arduino folder. 

## Installing NodeJS and preparing the server
You need to install **NodeJS** in your operating system in order to have the web dashboard working correctly, [here](https://nodejs.org/en/) you can find details.
After this, you have to check if you have **npm** installed, otherwise you can install it from [here](https://www.npmjs.com/get-npm).
You also need to require the **.env** file, that has to placed in the dashboard directory. More info about the .env file will come later.

## Nodemon
After this, you can install **nodemon** (not mandatory), a very useful package to run our NodeJS server, with this command: **npm install -g nodemon** if you are on Windows, or **sudo npm install -g nodemon** if you are on Linux/Ubuntu.

## Installing MQTT-cli
You need also to install this [MQTT-cli](https://www.hivemq.com/blog/mqtt-cli/), in order to do publish/subscribe requests from the command line.

## MongoDB [WIP]


## AWS, Iot-Core and DynamoDB
Then, you need so sign in in [AWS](https://aws.amazon.com/education/awseducate/), in order to get access to **Iot-Core** and **DynamoDB** services. Once you're on Iot-Core, you need to create a **thing** in order to get your **certificate**, **root certificate** and **private key**. Here you can find a [tutorial](https://docs.aws.amazon.com/iot/latest/developerguide/iot-moisture-create-thing.html). Then you need to add into the **.env** file **the access key id, the secret access key and the session token** provided by AWS, to be able to receive messages from the devices and to query the DynamoDB database. So at this point the .env should look like this:

```
AWS_ACCESS_KEY_ID=YOUR_AWS_ACCESS_KEY_ID
AWS_SECRET_ACCESS_KEY=YOUR_AWS_SECRET_ACCESS_KEY
AWS_SESSION_TOKEN=YOUR_AWS_SESSION_TOKEN
```

Then you need to **subscribe** to the **data/seastation/+** MQTT topic, where '+' stands for 'at least one character' from the syntax of regular expressions: in our case it will represent the **seastation id**. 

![img](https://github.com/IlKaiser/IoT_Group-Project/blob/main/imgs/subscribe_aws.png)

Then by going in the Rules section you have to create a **rule** for violations, specifying as topic data/seastation/<seastation_id>, and as operation you need to choose "Insert a message into a DynamoDB table". Here you can find a [tutorial](https://docs.aws.amazon.com/iot/latest/developerguide/iot-ddb-rule.html).

**Violation rule**:

![img](https://github.com/IlKaiser/IoT_Group-Project/blob/main/imgs/violation_rule.png)

So it is clear that we will create a **DynamoDb table**: particularly, each entry of the table is made up of the **Unix timestamp** associated to the creation of that entry, the **seastation id** of the Sea Station that send the violation to AWS by MQTT, the **device_data**, which consists of the **seastation id**, the **violation id** of the violation detected, the **latitude** and **longitude** of the floater that send the indication of the violation to the Sea Station and the **url of the picture** taken by the Raspberry pi Camera. 
