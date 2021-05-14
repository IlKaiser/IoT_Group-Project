# Evaluation

## Introduction
As we have seen by the explanation of the [Nucleo code](https://github.com/IlKaiser/IoT_Group-Project/blob/main/nucleo_code/README.md), between our main purposes we can find a **power management system**: based on the value of **acceleration** along the **z-axis** that is constantly sampled from the **MPU9250** accelerometer, we have defined three **modalities** of our system:
- **mode 1**: the sea conditios are **good**, so our system needs to monitor the environment with an **high** frequence
- **mode 2**: the sea conditios are **stable**, so our system needs to monitor the environment with a **normal** frequence
- **mode 3**: the sea conditios are **not good**, so our system needs to monitor the environment with a **low** frequence

In order to have a better understanding of the power consumption of our system, we decided to use [Iot-lab](https://www.iot-lab.info/).
First, we need to create a **monitoring profile**: 

![img](https://github.com/IlKaiser/IoT_Group-Project/blob/main/evaluation/images/eval-monitoring-profile.png)

The monitoring profile will allow us to measure consumption in terms of **power**. In particular, the metric used for the power is the **Watt (W)**. 

Then, in order to test our modalities separately, we need to create three different **experiments** of 30 minutes each. Below I show a picture of the experiment I used to test the mode 1:

![img](https://github.com/IlKaiser/IoT_Group-Project/blob/main/evaluation/images/eval-experiment.png)

As we can see, for the experiment I used the **Saclay** frontend, where I reserved **one M3-node** (**M3-10**) as a node for our **application**.

Finally, we tested all three modalities together by simulating the switch from one to another, so we created one last experiment, for a total **4** experiments.

## Code
More info on the **Iotlab-m3 code** can be founf [here](https://github.com/IlKaiser/IoT_Group-Project/blob/main/evaluation/iotlab-m3-code.md).
