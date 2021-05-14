# Evaluation

## Introduction
As we have seen by the explanation of the [Nucleo code](https://github.com/IlKaiser/IoT_Group-Project/blob/main/nucleo_code/README.md), between our main purposes we can find a **power management system**: based on the value of **acceleration** along the **z-axis** that is constantly sampled from the **MPU9250** accelerometer, we have defined three **modalities** of our system:
- **mode 1**: the sea conditios are **good**, so our system needs to monitor the environment with an **high** frequence
- **mode 2**: the sea conditios are **stable**, so our system needs to monitor the environment with a **normal** frequence
- **mode 3**: the sea conditios are **not good**, so our system needs to monitor the environment with a **low** frequence

In order to have a better understanding of the power consumption of our system, we decided to use [Iot-lab](https://www.iot-lab.info/): in particular, you can check this [tutorial](https://iot-lab.github.io/docs/tools/consumption-monitoring/) in order to see more details.
First, we need to create a **monitoring profile**: 

![img](https://github.com/IlKaiser/IoT_Group-Project/blob/main/evaluation/images/eval-monitoring-profile.png)

The monitoring profile will allow us to measure consumption in terms of **power**. In particular, the metric used for the power is the **Watt (W)**. 

Then, in order to test our modalities separately, we need to create three different **experiments**. Below we show a picture of the experiment I used to test the mode 1:

![img](https://github.com/IlKaiser/IoT_Group-Project/blob/main/evaluation/images/eval-experiment.png)

As we can see, for the experiment we used the **Saclay** frontend, where we reserved **one M3-node** (**M3-10**) as a node for our **application**.

Finally, we tested all three modalities together by simulating the switch from one to another, so we created one last experiment, for a total **4** experiments. All experiments lasted **30 minutes**.

## Mode 1
Below we show the results for the **mode 1**, also with a zoomed view:

![img](https://github.com/IlKaiser/IoT_Group-Project/blob/main/evaluation/images/power-consumption-pm-1.png)

![img](https://github.com/IlKaiser/IoT_Group-Project/blob/main/evaluation/images/power-consumption-pm-1-zoom.png)

As we can see, in a lot of points we have **peaks**, corresponding to the **sampling** of values. The main factor that affects this high power consumption is given by the simulated periodical sampling from the **MPU-9250**, that is in the order of **microseconds**. Obviously, as the number of wireless node increases, the performance of one of this nodes remain almost **similar**, because they are executing the same application code. At the beginning of the plot it's possible to see a little **noise** but the trend across the time it's stable: this noise can be explained by time occurred to setup and run the command from the application **shell**.

Below we also show a picture of the application running in the console in mode 1:

![img](https://github.com/IlKaiser/IoT_Group-Project/blob/main/evaluation/images/console-pm-not-general.png)

## Mode 2
Below we show the results for the **mode 2**, also with a zoomed view:

![img](https://github.com/IlKaiser/IoT_Group-Project/blob/main/evaluation/images/power-consumption-pm-2.png)

![img](https://github.com/IlKaiser/IoT_Group-Project/blob/main/evaluation/images/power-consumption-pm-2-zoom.png)

As we can see, in some points we have **peaks**, corresponding to the **sampling** of values, but if we compare these plots with those of mode 1 above, for sure we have less peaks than there, beacuse the sampling periods for all sensors are slightly **increased**: this leads to a **reduction** of the power consumption. All other observations done for mode 1 holds also for mode 2.

## Mode 3
Below we show the results for the **mode 3**, also with a zoomed view:

![img](https://github.com/IlKaiser/IoT_Group-Project/blob/main/evaluation/images/power-consumption-pm-3.png)

![img](https://github.com/IlKaiser/IoT_Group-Project/blob/main/evaluation/images/power-consumption-pm-3-zoom.png)

As we can see, in some points we have **peaks**, corresponding to the **sampling** of values, but if we compare these plots with those of mode 1 and 2 above, for sure we have less peaks than there, beacuse the sampling periods for all sensors are, again, slightly **increased**: this leads to a further **reduction** of the power consumption. All other observations done for mode 1 and 2 holds also for mode 3.

## Genaral mode
Below we show the results for the **general mode**, where the switching occures after about **10 minutes**:

![img](https://github.com/IlKaiser/IoT_Group-Project/blob/main/evaluation/images/pm-general.png)

In the first 10 minutes we can observe the trend of **mode 1**, in the next 10 minutes the trend for **mode 2** and in the final 10 minutes the trend for **mode 3**.

Below we also show a picture of the application running in the console in mode 3 at the moment of the switching from mode 1 to mode 2:

![img](https://github.com/IlKaiser/IoT_Group-Project/blob/main/evaluation/images/switch_from_1_to_2.png)

## Code
More info on the **Iotlab-m3 code** can be found [here](https://github.com/IlKaiser/IoT_Group-Project/blob/main/evaluation/iotlab-m3-code.md).
