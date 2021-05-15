# Web Dashboard

## Introduction
The **Web Dashboard** is one of the main component of our project. It is composed by two parts:

- the **server**, realized with **Node.js**
- the **frontend**, realized with **HTML, Bootstrap, Javascript, Vue.js**

It is directed to **coast guards** with the purpose of show them all the **violations** detected by the Sea Stations. Below we show a picture of all interactions at the Web Dashboard level:

![img](https://github.com/IlKaiser/IoT_Group-Project/blob/main/imgs/dashboard-architecture.jpeg)

As we can see from the picture, the server interacts with two databases:

- a **DynamoBD** database, that acts as the **cloud on AWS** where we store data of each **violation** detected
- a **MongoDB** database, where we store info about the **coast guards**

The login credentials are provided by the system admin to each user.

Finally, the frontend interacts with the server by using **HTTP/REST** calls.
 
## Overview
First, the user needs to **login** into the system by providing the login credentials:

![img](https://github.com/IlKaiser/IoT_Group-Project/blob/main/imgs/login.png)

After the login, he will be redirected to the **violations page**, where he can see all violations detected, from the most recent to the least recent. By clicking on **Logout**, he will be redirected to the login page. Below we can see a picture:

![img](https://github.com/IlKaiser/IoT_Group-Project/blob/main/imgs/violations.png)

By clicking on the button **More info** of one violation, he will be redirected to a page where he can see the **picture of the boat** taken by the Raspberry pi Camera and a **map** with the marker pointing on the last known position of the Floater that correctly detected the violation. By clicking on **<-Back** he will be redirected to the violations page. Below we can see a couple of images:

![img](https://github.com/IlKaiser/IoT_Group-Project/blob/main/imgs/violation_info_1.png)

![img](https://github.com/IlKaiser/IoT_Group-Project/blob/main/imgs/violation_info_2.png)

## Code
More info about the code can be found [here](https://github.com/IlKaiser/IoT_Group-Project/blob/main/dashboard/dashboard_code.md).
