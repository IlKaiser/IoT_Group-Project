# Wiring of components
For the project we will use:
- an [STM32 Nucleo-64 F401RE](https://www.st.com/en/evaluation-tools/nucleo-f401re.html) board
- an [Arduino Uno](https://store.arduino.cc/arduino-uno-rev3) board
- a [Raspberry Pi 4](https://www.raspberrypi.org/products/raspberry-pi-4-model-b/) board
- 1 HC-SR04 ultrasonic proximity sensor
- an MPU-9250 9 Axis Gyroscope Accelerometer Magnetic Field sensor
- a BN-880 gps module
- 1 100 Ohm resistor
- 1 Stepper Unipolar motor
- 1 ULN2008
- 1 E32868T20D

First, we need to put the **5v supply** and the **ground** on the breadboard.

In order to establish the **I2C** communication between the Nucleo board and the Arduino Uno board, we need to:
- connect one **GND pin** of the Arduino Uno board with one **GND pin** of the Nucleo board
- the **A5 pin** of the Arduino Uno board with the **SCL/D15 pin** of the Nucleo board
- the **A4 pin** of the Arduino Uno board with the **SDA/D14 pin** of the Nucleo board

However, since the SCL and SDA pins are also needed for the MPU-9250 sensor, we need to put them on the breadboard.

Now we explain the wiring of sensors and actuators:
- For the **buzzer**, we have to use a **resistor of 100 Ohm**, and we need to connect the **positive pin** of the buzzer to a **digital pin** of the Nucleo board, while the **negative pin** will be connected to one side of the resistor, and finally the other side of the resistor will be connected to **ground**: particularly, the digital pin of the Nucleo board we connect the buzzer is the **D9 pin**
- For the **HC-SR04** sensor we need to connect the **GND pin** to **ground** on the breadboard, the **VCC pin** to **5V** on the breadboard, the **trig pin** to the **D12 pin** of the Arduino Uno board and the **echo pin** to the **D13 pin** of the Arduino Uno board
- For the **MPU-9250** sensor we need to connect the **GND pin** to **ground** on the breadboard, the **VCC pin** to **3.3V** of the Nucleo board, the **SCL pin** to the **SCL/D15 pin** of the Nucleo board on the breadboard and the **SDA pin** to the **SDA/D14 pin** of the Nucleo board on the breadboard
- For the **BN-880** module we need to to connect the **GND pin** to **ground** on the breadboard, the **VCC pin** to **5V** on the breadboard and the **TX pin** to the **D7 pin** of the Arduino Uno board
- For the **E32868T20D** LoraWan module we need to connect the **GND pin** to **GND** on Arduino, the **VCC pin** to **3.3V** of the Arduino board, the **AUX pin** to the **D6 pin** of the Arduino board,the **M1 pin** to the **D5 pin** of the Arduino board,the **M0 pin** to the **D4 pin** of the Arduino board,the **RX pin** to the **D3 pin** of the Arduino board,the **TX pin** to the **D2 pin** of the Arduino board 

Below we can see the **wiring of components**:

![img](https://github.com/IlKaiser/IoT_Group-Project/blob/main/circuit/circuit.png)

**NOTE!** As you can see from the wiring, we need to point out a few things: 
- the MPU sensor is not an MPU-9250 in the picture, but is an MPU-6050 sensor, that works with the same logic
- in the picture we have a 220 Ohm resistor but, as mentioned before, it should be a 100 Ohm resistor
- the E32868T20D LoraWan module is obviously not the real one 
