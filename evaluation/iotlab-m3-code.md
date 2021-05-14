# Iotlab-m3 code

## Shell initialization
The application starts by initializing a **message queue** for the thread running the shell, and then runs the shell, that will be useful for launching the application on the M3-node in the diffent modes:

```c
msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

char line_buf[SHELL_DEFAULT_BUFSIZE];
shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
```

In order to do this, we need to include these **headers** in the [main.c](https://github.com/IlKaiser/IoT_Group-Project/blob/main/evaluation/main.c):

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
As we can see, we only have one customized command, that is **pmtest**, which is responsible of starting the application by invoking the *cmd_handler* function, that I report below:

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

The cmd_handler takes as input the **number of arguments** of the program and the **effective arguments**. We only have one argument, that is the **mode** that will be assigned to the instance of the application. As discussed [here](https://github.com/IlKaiser/IoT_Group-Project/blob/main/evaluation/README.md), there will be **4** different modes:
- **high mode**
- **medium mode**
- **low mode**
- **general mode**

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

This thread continuosly evaluates the **elapsed time**, in particular every DELAY_TIMER seconds and, if the elapsed time exceeds the CONTROL_TIME_THRESHOLD, then the **mode** is automatically changed with a predefined logic that we can find in the *runtimePowerManagement* function, and the timer is restarted.

In order to use the timer, we need to include this **header** in the [main.c](https://github.com/IlKaiser/IoT_Group-Project/blob/main/evaluation/main.c):

```c
#include <time.h>
```

We need to define the CONTROL_TIME_THRESHOLD in the [main.c](https://github.com/IlKaiser/IoT_Group-Project/blob/main/evaluation/main.c) as follows:

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

