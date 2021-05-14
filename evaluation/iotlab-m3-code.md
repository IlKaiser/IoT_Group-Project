# Iotlab-m3 code

## Shell initialization
The application starts by initializing a **message queue** for the thread running the shell, and then runs the shell, that will be useful for launching the application on the M3-node in the diffent modes:

```c
msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

char line_buf[SHELL_DEFAULT_BUFSIZE];
shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
```

In order to do this, we need to include these **modules** in the [Makefile](https://github.com/IlKaiser/IoT_Group-Project/blob/main/evaluation/Makefile):

```c
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
```

Also, we need to include these **headers** in the [main.c](https://github.com/IlKaiser/IoT_Group-Project/blob/main/evaluation/main.c):

```c
#include "shell.h"
#include "msg.h"
```

Then we need to define the **message queue** together with its **size** in the [main.c](https://github.com/IlKaiser/IoT_Group-Project/blob/main/evaluation/main.c):

```c
#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];
```

Then, we need to define the **shell_commands** in the [main.c](https://github.com/IlKaiser/IoT_Group-Project/blob/main/evaluation/main.c):

```c
static const shell_command_t shell_commands[] = {
    { "pmtest", "starts the application with a certain power management level", cmd_handler},
    { NULL, NULL, NULL }
};
```

## cmd_handler
As we can see, we only have one customized command, that is **pmtest**, which is responsible of starting the application by invoking the *cmd_handler* function, that we report below:

```c
int cmd_handler(int argc, char **argv){
    (void)argc;
    (void)argv;
    
    char* mode = argv[1];
    
    // Saving pm mode into a global variable
    pm_mode = mode;
    
    printf("pm_mode = %s\n",pm_mode);
    
    if(strcmp(mode, POWER_MANAGEMENT_HIGH) != 0 && strcmp(mode, POWER_MANAGEMENT_MEDIUM) != 0 &&
       strcmp(mode, POWER_MANAGEMENT_LOW) != 0 && strcmp(mode, POWER_MANAGEMENT_GENERAL) != 0){
       
       printf("Please insert %s or %s or %s or %s\n", POWER_MANAGEMENT_HIGH, POWER_MANAGEMENT_MEDIUM, POWER_MANAGEMENT_LOW, POWER_MANAGEMENT_GENERAL);
       
       exit(EXIT_FAILURE);  
    }
    
    if(strcmp(mode, POWER_MANAGEMENT_GENERAL) != 0){
	    fixedPowerManagement(mode);   	
	}
	
	else{
	    printf("Power management: general mode inserted!\n");
	    
	    thread_create(stackThreadTimer,sizeof(stackThreadTimer),THREAD_PRIORITY_MAIN-1,0,threadTimer,NULL,"Thread Timer");
		printf("Thread timer created! \n");
	}
	
	thread_create(stackThreadAlarm,sizeof(stackThreadAlarm),THREAD_PRIORITY_MAIN-1,0,threadAlarm,NULL,"Thread Alarm detection");
	printf("Thread alarm created! \n");
	
	thread_create(stackThreadControl,sizeof(stackThreadControl),THREAD_PRIORITY_MAIN-1,0,threadControl,NULL,"Thread Control");
	printf("Thread control created! \n");
	
	thread_create(stackThreadInfo,sizeof(stackThreadInfo),THREAD_PRIORITY_MAIN-1,0,threadInfo,NULL,"Thread Info ");
	printf("Thread info created! \n");
		
    return 0;
}
```

### Power modes

The cmd_handler takes as input the **number of arguments** of the program and the **effective arguments**. We only have one argument, that is the **mode** that will be assigned to the instance of the application. As discussed [here](https://github.com/IlKaiser/IoT_Group-Project/blob/main/evaluation/README.md), there will be **4** different modes:
- **high mode**
- **medium mode**
- **low mode**
- **general mode**

We need to define the **power modes macros** in the [main.c](https://github.com/IlKaiser/IoT_Group-Project/blob/main/evaluation/main.c):

```c
#define POWER_MANAGEMENT_HIGH           "high"
#define POWER_MANAGEMENT_MEDIUM         "medium"
#define POWER_MANAGEMENT_LOW            "low"
#define POWER_MANAGEMENT_GENERAL        "general"
```

After validating the mode, the application checks whether we are in general mode or not:
- if we are in general mode, the application creates a **timer thread**
- otherwise, the application simply fixes the sampling delays based on the inserted mode, with the help of the *fixedPowerManagement* function

In order to create the thread we need to include this **header** in the [main.c](https://github.com/IlKaiser/IoT_Group-Project/blob/main/evaluation/main.c):

```c
#include "thread.h"
```

We also need to define a **stack** for the thread in the [main.c](https://github.com/IlKaiser/IoT_Group-Project/blob/main/evaluation/main.c):

```c
static char stackThreadTimer[THREAD_STACKSIZE_DEFAULT];
```

## Timer thread
Below I report the *threadTimer* function:

```c
static void* threadTimer(void* arg){
	(void)arg;
	
	// Start measuring time
	time_t begin, end;
	time(&begin);
    
	char* modeIdentifier = POWER_MANAGEMENT_HIGH;
	
	printf("[TIMER] Now in mode %s\n",modeIdentifier);
	
	while(1){
	  	// Stop measuring time and calculate the elapsed time
		time(&end);
		time_t elapsed = end - begin;
		
		//printf("Elapsed time: %ld seconds.\n", elapsed);
		
		if(elapsed >= CONTROL_TIME_THRESHOLD){
				
			long int el = elapsed;
				
			printf("Elapsed time: %ld seconds.\n", el);
			runtimePowerManagement(&modeIdentifier);
			
			printf("[TIMER] Now in mode %s\n",modeIdentifier);
		    
			// Restart measuring time
			time(&begin);
		}
	    
	    xtimer_usleep(DELAY_TIMER);
	}
	
	return NULL;
}
```

This thread continuosly evaluates the **elapsed time**, in particular every **DELAY_TIMER** seconds and, if the elapsed time exceeds the **CONTROL_TIME_THRESHOLD**, then the **mode** is automatically changed with a predefined logic that we can find in the *runtimePowerManagement* function, and the timer is restarted.

In order to use the timer, we need to include this **header** in the [main.c](https://github.com/IlKaiser/IoT_Group-Project/blob/main/evaluation/main.c):

```c
#include <time.h>
```

We need to define the **CONTROL_TIME_THRESHOLD** in the [main.c](https://github.com/IlKaiser/IoT_Group-Project/blob/main/evaluation/main.c) as follows:

```c
#define CONTROL_TIME_THRESHOLD         600
```

We need to define the **delay period constant** for the timer thread as follows in the [main.c](https://github.com/IlKaiser/IoT_Group-Project/blob/main/evaluation/main.c):

```c
#define DELAY_TIMER      500
```

In order to do a periodical check of the elapsed time, we need to include the **xtimer module** in the [Makefile](https://github.com/IlKaiser/IoT_Group-Project/blob/main/evaluation/Makefile):

```c
USEMODULE += xtimer
```

Also, we need to include this **header** in the [main.c](https://github.com/IlKaiser/IoT_Group-Project/blob/main/evaluation/main.c):

```c
#include "xtimer.h"
```

### runtimePowerManagement function

```c
void runtimePowerManagement(char** modeIdentifier){
    printf("Switching from mode %s to mode ",*modeIdentifier);
    
    if(strcmp(*modeIdentifier, POWER_MANAGEMENT_HIGH) == 0){
		printf("%s\n",POWER_MANAGEMENT_MEDIUM);
		
		*modeIdentifier = POWER_MANAGEMENT_MEDIUM;
		
		delay_alarm = DELAY_ALARM_MEDIUM;
		delay_control = DELAY_CONTROL_MEDIUM;
		delay_info = DELAY_INFO_MEDIUM;	
	}
	
	else if(strcmp(*modeIdentifier, POWER_MANAGEMENT_MEDIUM) == 0){
		printf("%s\n",POWER_MANAGEMENT_LOW);
		
		*modeIdentifier = POWER_MANAGEMENT_LOW;
		
		delay_alarm = DELAY_ALARM_HIGH;
		delay_control = DELAY_CONTROL_HIGH;
		delay_info = DELAY_INFO_HIGH;	
	}
	
	else if(strcmp(*modeIdentifier, POWER_MANAGEMENT_LOW) == 0){
	    printf("%s\n",POWER_MANAGEMENT_HIGH);
	    
	    *modeIdentifier = POWER_MANAGEMENT_HIGH;
		
		delay_alarm = DELAY_ALARM_BASE;
		delay_control = DELAY_CONTROL_BASE;
		delay_info = DELAY_INFO_BASE;	
	}
}
```

The *runtimePowerManagement* function takes as input the **address of the current mode** that our application is running, and based on that, change the mode in **circular way** by setting the corresponding **sampling delays**.

We need to define the **sampling delays** for each mode in the [main.c](https://github.com/IlKaiser/IoT_Group-Project/blob/main/evaluation/main.c):

**High mode delays**
```c
#define DELAY_INFO_BASE 30
#define DELAY_ALARM_BASE 1
#define DELAY_CONTROL_BASE 100
```

**Medium mode delays**
```c
#define DELAY_INFO_MEDIUM 50
#define DELAY_ALARM_MEDIUM 10
#define DELAY_CONTROL_MEDIUM 10000000
```

**Low mode delays**
```c
#define DELAY_INFO_HIGH 70
#define DELAY_ALARM_HIGH 20
#define DELAY_CONTROL_HIGH 60000000
```

## fixedPowerManagement function

```c
void fixedPowerManagement(char* mode){
    if(strcmp(mode, POWER_MANAGEMENT_HIGH) == 0){
		printf("ACCEL LOW!\n");
		    
		delay_alarm = DELAY_ALARM_BASE;
		delay_control = DELAY_CONTROL_BASE;
		delay_info = DELAY_INFO_BASE;	
	}
    
    else if(strcmp(mode, POWER_MANAGEMENT_MEDIUM) == 0){
		printf("ACCEL MEDIUM!\n");
		    
		delay_alarm = DELAY_ALARM_MEDIUM;
		delay_control = DELAY_CONTROL_MEDIUM;
		delay_info = DELAY_INFO_MEDIUM;	
	}
		
	else if(strcmp(mode, POWER_MANAGEMENT_LOW) == 0){
		printf("ACCEL HIGH!\n");
		    
		delay_alarm = DELAY_ALARM_HIGH;
		delay_control = DELAY_CONTROL_HIGH;
		delay_info = DELAY_INFO_HIGH;	
	}
}
```

The *fixedPowerManagement* function takes as input the mode inserted by the user, and sets as **sampling delays** the delays corresponding to that mode.

## Sampling threads

Finally, the handler initializes the sampling threads, the same as those seen in the [real application](https://github.com/IlKaiser/IoT_Group-Project/blob/main/nucleo_code/README.md), that will simulate the sampling of data from sensors. In order to create these threads, we need to define their stacks in the [main.c](https://github.com/IlKaiser/IoT_Group-Project/blob/main/evaluation/main.c):

```c
static char stackThreadAlarm[THREAD_STACKSIZE_DEFAULT];
static char stackThreadInfo[THREAD_STACKSIZE_DEFAULT];
static char stackThreadControl[THREAD_STACKSIZE_DEFAULT];
```

### Alarm thread

```c
static void* threadAlarm(void* arg)
{
    (void)arg;
    
    srand(time(NULL));
	
	int cont=0;
	while(1)
	{	
		int alarm_response = rand() % 2;
		
		printf("[ALARM]MEX: %d \n",alarm_response);
		
		if(alarm_response == 1)
			cont++;
		else
			cont=0;
			
			
		
		if(cont>=ALARM_THRESHOLD){
			printf("[ALARM] Boat found! Sending indication to Arduino for Raspberry...\n");
		}
		if(cont==0){
			printf("No boats found \n");
		}
		
		
		xtimer_sleep(delay_alarm);
	
	}
	
	return NULL;
}
```

The **alarm thread** is responsible for the simulation of **boat detections**. The detection is completely random, based on a **boolean random variable** continuosly generated every **delay_alarm** seconds, and if '1' is extracted **ALARM_THRESHOLD** or more time in a row, we assume that there was a violation, otherwise the counter is restarted.

We need to define the **ALARM_THRESHOLD** and the **delay_alarm** in the [main.c](https://github.com/IlKaiser/IoT_Group-Project/blob/main/evaluation/main.c):

```c
#define ALARM_THRESHOLD 2

int delay_alarm = DELAY_ALARM_BASE;
```

Notice that the delay_info is setted to th delay corresponding to the **high mode**, and the same holds for the other sampling delays that we will see later.

### Control thread

```c
static void* threadControl(void* arg)
{
	(void)arg;
	
	srand(time(NULL));

	//sample values for magnetic field detected by the compass and accelerometer
    short x_compass, x_start_compass;
	
	getControlData(&x_compass, &x_start_compass);
	
	//get control data correction degree will be saved into buffer with sintax -360/360 as char*
	while(1)
	{
		
		//get control data
	    getControlData(&x_compass, &x_start_compass);
	    
	    //printf("[CONTROL] x_start_compass = %hd\n",x_start_compass);
	    //printf("[CONTROL] x_compass = %hd\n",x_compass);
				
		// Proximity sensor correction
		proximitySensorCorrection(x_start_compass, x_compass);
		
		xtimer_usleep(delay_control);
	}
	
	return NULL;
}
```

The **control thread** is responsible for the simulation of **proximity sensor correction**. Sampling from the the **MPU-9250** are simulated through the *getControlData* function:

```c
void getControlData(short* x_compass, short* x_start_compass){
    
    if(!first_read_compass_done){
	    *x_start_compass = rand() % 50;
		*x_compass = *x_start_compass;
		
		first_read_compass_done = 1;
	}
	
	else{
		
		int lower = *x_start_compass - 2;
		int upper = *x_start_compass + 2;
		
	    *x_compass = (rand() % (upper - lower + 1)) + lower;	
	}
	
}
```

The correction of the orientation of the proximity sensor is simulated with the *proximitySensorCorrection* function:

```c
void proximitySensorCorrection(short x_start_compass, short x_compass){
    
	short x_diff_compass = x_start_compass - x_compass;	
	int x_diff_compass_abs = abs((int)x_diff_compass);
	
	// If the proximity sensor is NOT correctly orientated
	if(x_diff_compass_abs > CORRECTION_THRESHOLD){
		
		//printf("[CONTROL] Sending to Arduino the indication of correcting the proximity sensor orientation\n");
		
	}
	
	// The proximity sensor is correctly orientated
	else{
	    //printf("The proximity sensor is correctly orientated\n"); 
	}
		
}
```

The base logic of the function is the same of the [real application](https://github.com/IlKaiser/IoT_Group-Project/blob/main/nucleo_code/README.md).

In the [main.c](https://github.com/IlKaiser/IoT_Group-Project/blob/main/evaluation/main.c), we need to define the **delay_control** sampling period, as well as the **CORRECTION_THRESHOLD**:

```c
#define CORRECTION_THRESHOLD 1

int delay_control = DELAY_CONTROL_BASE;
```

## Info thread

```c
static void* threadInfo(void* arg)
{
    (void)arg;
    
    srand(time(NULL));
	
	while(1)
	{
		int decision = rand() % 2;
		
		char* gpsInfo = getGpsInfoFromArduino(decision);
		
		if(strcmp(gpsInfo, "/") == 0){
			printf("[INFO] No info received!\n");
		}
		
		else{
		    printf("[INFO] %s\n",gpsInfo);
		}
			
		xtimer_sleep(delay_info);
	
	}
	
	return NULL;
}
```

The **info thread** is responsible for simulating the retrieval of **GPS** data. In particular, we simulate with a random **boolean decision**, whether the position is available or not, by using the getGpsInfoFromArduino, that we report below:

```c
char* getGpsInfoFromArduino(int decision){
    if(decision == 1)
		return GPS_INFO_REAL;
	else
	    return GPS_INFO_NOT_FOUND;
}
```

The function simply return some **fixed** GPS info if the decision is '1', nothing otherwise.

In the [main.c](https://github.com/IlKaiser/IoT_Group-Project/blob/main/evaluation/main.c), we need to define the **delay_info** sampling delay:

```c
int delay_info = DELAY_INFO_BASE;
```

Also, in the [main.c](https://github.com/IlKaiser/IoT_Group-Project/blob/main/evaluation/main.c), we need to define the **GPS_INFO_REAL** and the **GPS_INFO_NOT_FOUND** constants:

```c
#define GPS_INFO_REAL                  "41.3456;19.3185;0.25"
#define GPS_INFO_NOT_FOUND             "/"
```
