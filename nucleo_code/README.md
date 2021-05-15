# Nucleo code

## Starting the application
The application starts by initializing the I2C interface. In order to do this, we need to add the *periph_i2c* feature in the [Makefile](https://github.com/IlKaiser/IoT_Group-Project/blob/main/nucleo_code/Makefile):

### I2C initialization

```c
FEATURES_REQUIRED += periph_i2c
```

Also, we need to include the following headers in the [main.c](https://github.com/IlKaiser/IoT_Group-Project/blob/main/nucleo_code/main.c):

```c
#include "periph_conf.h"
#include "periph/i2c.h"
```

Also we have to define the I2C interface:

```c
#define I2C_INTERFACE I2C_DEV(0)
```

Below we report the code related to the initialization of the I2C interface:

```c
//initialize the i2c interface
i2c_init(I2C_INTERFACE);
```
### I2C communication
Finally, the I2C communication is managed by the *I2CCommunication* function:

```c
void I2CCommunication(int cmd,void*buffer,int len,char* service){
    mutex_lock(&mutex);
    while(i2c_acquire(I2C_INTERFACE)<0)
	printf("[%s]ERROR ACQUIRING \n",service);
    if(i2c_write_byte(I2C_INTERFACE,ARDUINO_ADDRESS,cmd,0)<0)
	printf("[%s] ERROR WRTITING \n",service);
    if(i2c_read_bytes(I2C_INTERFACE,ARDUINO_ADDRESS,buffer,len,0)<0)
	printf("[%s] READING \n",service);
    i2c_release(I2C_INTERFACE);
    mutex_unlock(&mutex);	
}
```

As we can see we need to define the **Arduino address** in the [main.c](https://github.com/IlKaiser/IoT_Group-Project/blob/main/nucleo_code/main.c):

```c
#define ARDUINO_ADDRESS (0X04)
```

Notice how the access to the I2C bus is treated as a **critical section**, because we have to avoid that multiple threads access it at the same time. In order to do this, we used a **mutex**, so we have to include the following header in the [main.c](https://github.com/IlKaiser/IoT_Group-Project/blob/main/nucleo_code/main.c):

```c
#include "mutex.h"
```

We define the mutex as follows:

```c
//define mutex to protect i2c access
mutex_t mutex;
```

### Threads
Then we create three different threads, one for each sensor we used:

```c
thread_create(stackThreadAlarm,sizeof(stackThreadAlarm),THREAD_PRIORITY_MAIN,0,threadAlarm,NULL,"Thread Alarm detection");
	printf("Thread proximity alarm created! \n");
	
thread_create(stackThreadControl,sizeof(stackThreadControl),THREAD_PRIORITY_MAIN,0,threadControl,NULL,"Thread Control");
	printf("Thread control created! \n");
	
thread_create(stackThreadInfo,sizeof(stackThreadInfo),THREAD_PRIORITY_MAIN,0,threadInfo,NULL,"Thread Info ");
	printf("Thread info created! \n");
```

In order to do this we need to include this header in the [main.c](https://github.com/IlKaiser/IoT_Group-Project/blob/main/nucleo_code/main.c):

```c
#include "thread.h"
```

## Pins initialization
All pins used are initialized by using the **GPIO peripheral driver**, that maps each pin to a **port** with a certain **port number**. Particularly, all mappings are shown in the picture below:

![img](https://github.com/IlKaiser/IoT_Group-Project/blob/main/imgs/conversions.png)

## Alarm thread

This thread initializes the buzzer, and then starts periodically requesting the Arduino info about the current situation nearby the floater by the I2C communication. If the Nucleo receives a number of indications of a possible violation greater or equal than a certain **ALARM_THRESHOLD**, then the buzzer starts to sound and a notification that a boat was found is sent to the Arduino by the I2C communication.
The buzzer is initialized in this way:

### Alarm thread: buzzer

```c
//definition and initialitation of buzzer using pin D9
gpio_t buzzer;

buzzer=GPIO_PIN(PORT_C,7);
if(gpio_init(buzzer,GPIO_OUT)<0){
    printf("[ALARM]Error during buzzer initialitation!\n");
    return NULL;
}
```

It is managed by the *controlBuzzer* function:

```c
void controlBuzzer(gpio_t buzzer,int status){
    if(status==0)
	gpio_clear(buzzer);
    else if(status==1)
	gpio_set(buzzer);
}
```

### Alarm thread: periodical sampling

The periodical requests are done by using the **xtimer** module, so we need to specify it in the [Makefile](https://github.com/IlKaiser/IoT_Group-Project/blob/main/nucleo_code/Makefile):

```c
USEMODULE += xtimer
```

Also, we need to include this header in the [main.c](https://github.com/IlKaiser/IoT_Group-Project/blob/main/nucleo_code/main.c):

```c
#include "xtimer.h"
```

We define the **base sampling period** for the Alarm thread as follows:

```c
#define DELAY_ALARM_BASE 1

int delay_alarm = DELAY_ALARM_BASE;
```

### Alarm threshold
The **alarm threshold** is defined as follows:

```c
#define ALARM_THRESHOLD 2
```

### Alarm thread: I2C communication
Given that the *I2CCommunication* function takes as input the command related to the specific request for the Arduino, the buffer for receiving data from the I2C bus, the length of the buffer and the specific service, we need to specify the **command** and **buffer length** for both cases of use of the I2C communication by the Alarm thread:

```c
#define ALARM_BUFFER_SIZE 1
#define ALARM_CMD 1
#define BOAT_FOUND_CMD 5
```

### Alarm thread: periodical requests
So, the periodical request of the alarm thread is resumed in the following code:

```c
int cont=0;
char buffer[ALARM_BUFFER_SIZE]={0};
	
while(1){
	I2CCommunication(ALARM_CMD,buffer,ALARM_BUFFER_SIZE,"ALARM");
	printf("[ALARM]MEX: %s \n",buffer);
		
	if(atoi(buffer)==1)
		cont++;
	else
		cont=0;
			
			
		
	if(cont==ALARM_THRESHOLD){
		controlBuzzer(buzzer,1);
		I2CCommunication(BOAT_FOUND_CMD,buffer,ALARM_BUFFER_SIZE,"BOAT FOUNDs");
	}
	if(cont==0)
		controlBuzzer(buzzer,0);
		
	xtimer_sleep(delay_alarm);
	
}
```

## Control thread

### Control thread: MPU-9250 initialization
The thread first initialize the **MPU9250 sensor**, then periodically samples it in order to check, together with the Arduino, whether the proximity sensor needs to be better oriented or not, and also to manage the power consumption in a smart way.
In order to use the MPU9250 sensor we have to specify the **mpu9250 module** in the [Makefile](https://github.com/IlKaiser/IoT_Group-Project/blob/main/nucleo_code/Makefile):

```c
# MPU-9250 driver
DRIVER ?= mpu9250
USEMODULE += $(DRIVER)
```

Also, we need to include some **utilities** in the [Makefile](https://github.com/IlKaiser/IoT_Group-Project/blob/main/nucleo_code/Makefile):

```c
# utilities for MPU 
ifneq (,$(wildcard $(CURDIR)../../RIOT/tests*/.))
  DEFAULT_MODULE += test_utils_interactive_sync
  # add interactive test configuration when testing Kconfig
  ifeq (1,$(TEST_KCONFIG))
    KCONFIG_ADD_CONFIG += $(RIOTBASE)/tests/test_interactive.config
  endif
endif

ifneq (,$(filter tests_driver_%,$(APPLICATION)))
  BOARD ?= nucleo-f401re
endif
```

Also, we need to include the following headers in the [main.c](https://github.com/IlKaiser/IoT_Group-Project/blob/main/nucleo_code/main.c):

```c
#include "mpu9x50.h"
#include "mpu9x50_params.h"
```

Then we have to specify the buffer length for the control thread for the I2C communication with Arduino:

```c
#define CONTROL_BUFFER_SIZE 10
```

The MPU9250 is initialized as follows:

```c
char buffer[CONTROL_BUFFER_SIZE]={0};
		
// definition of MPU-device
mpu9x50_t dev;
int result;

// MPU device initialization
result = mpu9x50_init(&dev, &mpu9x50_params[0]);
    
// Check errors
if (result == -1) {
    puts("[MPU9250] The given i2c is not enabled");
    return NULL;
}
else if (result == -2) {
    puts("[MPU9250] The compass did not answer correctly on the given address");
    return NULL;
}
   
// Definition of sample rates
mpu9x50_set_sample_rate(&dev, 200);
if (dev.conf.sample_rate != 200) {
    puts("[MPU9250] The sample rate was not set correctly");
    return NULL;
}
    
mpu9x50_set_compass_sample_rate(&dev, 100);
    
if (dev.conf.compass_sample_rate != 100) {
    puts("[MPU9250] The compass sample rate was not set correctly");
    //return NULL;
}
```

### Control thread: periodical sampling
We define the **base sampling period** for the Control thread as follows:

```c
#define DELAY_CONTROL_BASE 100

int delay_control = DELAY_CONTROL_BASE;
```

So, the periodical sampling of the control thread is resumed in the following code:

```c
short x_start_compass = 0;
	
//sample values for magnetic field detected by the compass and accelerometer
short z_gyro, x_compass, z_accel;
	
getControlData(dev, &z_gyro, &x_start_compass, &z_accel);
		
int last_state = 0;  // It stands for 'the proximity sensor is correctly orientated'

while(1){
	//get control data correction degree will be saved into buffer with sintax -360/360 as char*
	getControlData(dev, &z_gyro, &x_compass, &z_accel);
				
	// Proximity sensor correction
	proximitySensorCorrection(buffer, x_start_compass, x_compass, z_gyro, &last_state);
		
	// Power management
	float z_accell = ( (float) z_accel ) * GRAVITY_ACCELERATION / 1000;
	powerManagement(z_accell);
		
	xtimer_usleep(delay_control);
}
```

### Control thread: getControlData function
As we can see, data from MPU9250 are retrieved by using the *getControlData* function. Below we show the code related to this function:

```c
void getControlData(mpu9x50_t dev, short* z_gyro, short* x_compass, short* z_accel)
{
	//Data structure to save info
	mpu9x50_results_t measurement;
    
	mutex_lock(&mutex);
	
	/* Get accel data in milli g */
        mpu9x50_read_accel(&dev, &measurement);
        *z_accel = measurement.z_axis;
        //printf("[MPU9250] Accel data [milli g] - X: %"PRId16"   Y: %"PRId16"   Z: %"PRId16"\n",
        //        measurement.x_axis, measurement.y_axis, measurement.z_axis);
        /* Get gyro data in dps */
        mpu9x50_read_gyro(&dev, &measurement);
        *z_gyro = measurement.z_axis;
        //printf("[MPU9250] Gyro data [dps] - X: %"PRId16"   Y: %"PRId16"   Z: %"PRId16"\n",
        //        measurement.x_axis, measurement.y_axis, measurement.z_axis);
        /* Get compass data in mikro Tesla */
        
        while(!first_read_compass_done){
			
		mpu9x50_read_compass(&dev, &measurement);
		*x_compass = measurement.x_axis;
		    
		if(*x_compass != 0){
			first_read_compass_done = 1;
			break;	
		}	
	}
		
	if(first_read_compass_done){
		mpu9x50_read_compass(&dev, &measurement);
		*x_compass = measurement.x_axis;	
	}

	mutex_unlock(&mutex);
	
}
```

Notice that each sampling from the MPU9250 is treated as a **critical section**, because the sensor communicates with the Nucleo board by I2C, so we have to use the **mutex** in order to avoid **starvations** of other threads.

### Control thread: proximitySensorCorrection function

```c
void proximitySensorCorrection(char* buffer, short x_start_compass, short x_compass, short z_gyro, int* last_state){
    float degree = calculateDegreeFromDps(z_gyro, FIXED_NUMBER);
    
	short x_diff_compass = x_start_compass - x_compass;	
	int x_diff_compass_abs = abs((int)x_diff_compass);
	
	// If the proximity sensor is NOT correctly orientated
	if(x_diff_compass_abs > CORRECTION_THRESHOLD){
		
		// If it was correctly orientated before this point
		if(*last_state == 0){
		    
		    // If it is moving to the left 
		    if(degree > 0){
			    
			    // Setting the last state to 1, which means that the "next" last state will be that
			    // the proximity sensor was moved to the right
			    *last_state = 1;
			    // Send to Arduino the indication to move it to the right
			    I2CCommunication(CONTROL_CMD_RIGHT, buffer, CONTROL_BUFFER_SIZE, "CONTROL");
			        	
			}
			
			// If it is moving to the right
			else if(degree < 0){
			    
			    // Setting the last state to 2, which means that the "next" last state will be that
			    // the proximity sensor was moved to the left
			    *last_state = 2;
			    // Send to Arduino the indication to move it to the left
			    I2CCommunication(CONTROL_CMD_LEFT, buffer, CONTROL_BUFFER_SIZE, "CONTROL");
			        	
			} 
		    	
		}
		
		// If it was moved to the rigth before this point
		else if(*last_state == 1){
			
		    // Send to Arduino the indication to move it to the right
			I2CCommunication(CONTROL_CMD_RIGHT, buffer, CONTROL_BUFFER_SIZE, "CONTROL");
		}
		
		// If it was moved to the left before this point
		else if(*last_state == 2){
			
		    // Send to Arduino the indication to move it to the left
			I2CCommunication(CONTROL_CMD_LEFT, buffer, CONTROL_BUFFER_SIZE, "CONTROL");
		}
		
	}
	
	// The proximity sensor is correctly orientated
	else{
		*last_state = 0;
	    //printf("The proximity sensor is correctly orientated\n"); 
	}
		
}
```

The main idea of the proximity sensor orientation correction is measuring the first value of the **magnetic field (in µT)** along the **x-axis** of the **compass**, and continuosly check whether the current value measured is too far from the starting value, for example by exploiting a **correction threshold**, that we define in the [main.c](https://github.com/IlKaiser/IoT_Group-Project/blob/main/nucleo_code/main.c) as follows:

```c
#define CORRECTION_THRESHOLD 1
```

If the current orientation of the proximity sensor is too far from the starting one, then we send to Arduino by I2C the indication to rotate the sensor in the opposite direction from the currently detected motion, that is detected by the the measurement of the **dps (degree per seconds)** along the **z-axis** of the **gyroscope**. This procedure is repeated until the proximity sensor is not correctly orientated again. We have also to define, in the [main.c](https://github.com/IlKaiser/IoT_Group-Project/blob/main/nucleo_code/main.c), the two commands for the I2C communication, indicating a correction towards the right or the left:

```c
#define CONTROL_CMD_RIGHT 2
#define CONTROL_CMD_LEFT 3
```

The power management is handled by the sampling of the **acceleration** measured along the **z-axis** of the **accelerometer**:

- if the acceleration is below a **low threshold**, then sampling delays are setted to the base delays
- if the acceleration is above a **low threshold** but below an **high threshold**, then sampling delays are slightly increased
- if the acceleration is above a **high threshold**, then sampling delays are further increased

So we have to define the thresholds in the [main.c](https://github.com/IlKaiser/IoT_Group-Project/blob/main/nucleo_code/main.c):

```c
#define LOW_PM_THRESHOLD 9.5
#define HIGH_PM_THRESHOLD 10.0
```

Also, we have to define all delays in the [main.c](https://github.com/IlKaiser/IoT_Group-Project/blob/main/nucleo_code/main.c):

```c
//medium time interval for each thread
#define DELAY_INFO_MEDIUM 50
#define DELAY_ALARM_MEDIUM 10
#define DELAY_CONTROL_MEDIUM 10000000

//high time interval for each thread
#define DELAY_INFO_HIGH 70
#define DELAY_ALARM_HIGH 20
#define DELAY_CONTROL_HIGH 60000000
```

All this is done by the *powerManagement* function:

```c
void powerManagement(float z_accel){
    if(z_accel <= LOW_PM_THRESHOLD){
		printf("ACCEL LOW!\n");
		    
		delay_alarm = DELAY_ALARM_BASE;
		delay_control = DELAY_CONTROL_BASE;
		delay_info = DELAY_INFO_BASE;	
    }
    
    else if(z_accel > LOW_PM_THRESHOLD && z_accel <= HIGH_PM_THRESHOLD){
		printf("ACCEL MEDIUM!\n");
		    
		delay_alarm = DELAY_ALARM_MEDIUM;
		delay_control = DELAY_CONTROL_MEDIUM;
		delay_info = DELAY_INFO_MEDIUM;	
    }
		
    else if(z_accel > HIGH_PM_THRESHOLD){
		printf("ACCEL HIGH!\n");
		    
		delay_alarm = DELAY_ALARM_HIGH;
		delay_control = DELAY_CONTROL_HIGH;
		delay_info = DELAY_INFO_HIGH;	
     }
}
```

## Info thread
This thread periodically requests the Arduino for the **last known position** of the floater. In the [main.c](https://github.com/IlKaiser/IoT_Group-Project/blob/main/nucleo_code/main.c), we need to define the **base sampling period** for the Info thread as follows:

```c
#define DELAY_INFO_BASE 30

int delay_info = DELAY_INFO_BASE;
```

Also, in the [main.c](https://github.com/IlKaiser/IoT_Group-Project/blob/main/nucleo_code/main.c), we need to define the **buffer length** for the I2C communication with Arduino, as well as the **info command**:

```c
#define INFO_BUFFER_SIZE 20
#define INFO_CMD 4
```

So, the periodical requests of the Gps informations done by the Info thread are resumed in the following code:
```c
char buffer[INFO_BUFFER_SIZE]={0};
while(1){	
	
	I2CCommunication(INFO_CMD,buffer,INFO_BUFFER_SIZE,"INFO");
		
	char* first = getSubstr(buffer, 0, 2);
		
	if(strcmp(first, "/") == 0){
		printf("[INFO] No info received!\n");
	}
		
	else{
		printf("[INFO] %s\n",buffer);
	}
			
	xtimer_sleep(delay_info);
	
}
```
