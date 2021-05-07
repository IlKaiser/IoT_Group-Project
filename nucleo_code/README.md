# Nucleo code

## Starting the application
The application starts by initializing the I2C interface. In order to do this, we need to add the *periph_i2c* feature in the [Makefile](https://github.com/IlKaiser/IoT_Group-Project/blob/main/nucleo_code/Makefile):

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

The periodical requests are done by using the **xtimer** module, so we need to specify it in the [Makefile](https://github.com/IlKaiser/IoT_Group-Project/blob/main/nucleo_code/Makefile):

```c
USEMODULE += xtimer
```

Also, we need to include this header in the [main.c](https://github.com/IlKaiser/IoT_Group-Project/blob/main/nucleo_code/main.c):

```c
#include "xtimer.h"
```

We define the **sampling period** for the Alarm thread as follows:

```c
#define DELAY_ALARM_BASE 1

int delay_alarm = DELAY_ALARM_BASE;
```

The **alarm threshold** is defined as follows:

```c
#define ALARM_THRESHOLD 2
```

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

As we can see we need to define the **Arduino address**:

```c
#define ARDUINO_ADDRESS (0X04)
```

Given that the function takes as input the command related to the specific request for the Arduino, the buffer for receiving data from the I2C bus, the length of the buffer and the specific service, we need to specify the **command** and **buffer length** for both cases of use of the I2C communication by the Alarm thread:

```c
#define ALARM_BUFFER_SIZE 1
#define ALARM_CMD 1
#define BOAT_FOUND_CMD 5
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

So, the periodical request of the alarm thread is resumed in the following code:

```c
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


