#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "xtimer.h"
#include "thread.h"
#include <inttypes.h>
#include "fmt.h"
#include "shell.h"
#include "msg.h"

#define MAIN_QUEUE_SIZE     (8)

//define alarm threshold
#define ALARM_THRESHOLD 2

//base time interval for each thread
#define DELAY_INFO_BASE 30
#define DELAY_ALARM_BASE 1
#define DELAY_CONTROL_BASE 500

//medium time interval for each thread
#define DELAY_INFO_MEDIUM 50
#define DELAY_ALARM_MEDIUM 10
#define DELAY_CONTROL_MEDIUM 10000000

//high time interval for each thread
#define DELAY_INFO_HIGH 70
#define DELAY_ALARM_HIGH 20
#define DELAY_CONTROL_HIGH 60000000

//constants for correcting the proximity orientation with the compass
#define CORRECTION_THRESHOLD 1

//power management mode constants
#define POWER_MANAGEMENT_LOW           "1"
#define POWER_MANAGEMENT_MEDIUM        "2"
#define POWER_MANAGEMENT_HIGH          "3"
#define POWER_MANAGEMENT_GENERAL       "general"

//gps info constants
#define GPS_INFO_REAL                  "41.3456;19.3185;0.25"
#define GPS_INFO_NOT_FOUND             "/"

//constant (in sec) to determine wheter we have to change from one mode to another or not
#define CONTROL_TIME_THRESHOLD         60

//fixed number for float conversion
#define FIXED_NUMBER 4.3

static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

static char stackThreadAlarm[THREAD_STACKSIZE_DEFAULT];
static char stackThreadInfo[THREAD_STACKSIZE_DEFAULT];
static char stackThreadControl[THREAD_STACKSIZE_DEFAULT];

int first_read_compass_done = 0;

char* pm_mode = "";

//define delay variables for each thread
int delay_info = DELAY_INFO_BASE, delay_control = DELAY_CONTROL_BASE, delay_alarm = DELAY_ALARM_BASE;

char* fromFloatToString(float f, unsigned precision){
    
    char* buf = (char*)malloc(10*sizeof(char));
	size_t n = fmt_float(buf, f, precision);
	buf[n] = '\0';
    
    return buf;
}

void runtimePowerManagement(char** modeIdentifier){
    printf("Switching from mode %s to mode ",*modeIdentifier);
    
    if(strcmp(*modeIdentifier, POWER_MANAGEMENT_LOW) == 0){
		printf("%s\n",POWER_MANAGEMENT_MEDIUM);
		
		*modeIdentifier = POWER_MANAGEMENT_MEDIUM;
		
		delay_alarm = DELAY_ALARM_MEDIUM;
		delay_control = DELAY_CONTROL_MEDIUM;
		delay_info = DELAY_INFO_MEDIUM;	
	}
	
	else if(strcmp(*modeIdentifier, POWER_MANAGEMENT_MEDIUM) == 0){
		printf("%s\n",POWER_MANAGEMENT_HIGH);
		
		*modeIdentifier = POWER_MANAGEMENT_HIGH;
		
		delay_alarm = DELAY_ALARM_HIGH;
		delay_control = DELAY_CONTROL_HIGH;
		delay_info = DELAY_INFO_HIGH;	
	}
	
	else if(strcmp(*modeIdentifier, POWER_MANAGEMENT_HIGH) == 0){
	    printf("%s\n",POWER_MANAGEMENT_LOW);
	    
	    *modeIdentifier = POWER_MANAGEMENT_LOW;
		
		delay_alarm = DELAY_ALARM_BASE;
		delay_control = DELAY_CONTROL_BASE;
		delay_info = DELAY_INFO_BASE;	
	}
}

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

static void* threadControl(void* arg)
{
	(void)arg;
	
	// Start measuring time
	time_t begin, end;
	time(&begin);
    
	char* modeIdentifier = POWER_MANAGEMENT_LOW;
	
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
		
		if(strcmp(pm_mode, POWER_MANAGEMENT_GENERAL) == 0){
		    // Stop measuring time and calculate the elapsed time
			time(&end);
			time_t elapsed = end - begin;
		
			//printf("Elapsed time: %ld seconds.\n", elapsed);
		
			if(elapsed > CONTROL_TIME_THRESHOLD){
				printf("Elapsed time: %ld seconds.\n", elapsed);
				runtimePowerManagement(&modeIdentifier);
		    
				// Restart measuring time
				time(&begin);
			}   	
		}
		
		xtimer_usleep(delay_control);
	}
	
	return NULL;
}


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

char* getGpsInfoFromArduino(int decision){
    if(decision == 1)
		return GPS_INFO_REAL;
	else
	    return GPS_INFO_NOT_FOUND;
}

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

void fixedPowerManagement(char* mode){
    if(strcmp(mode, POWER_MANAGEMENT_LOW) == 0){
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
		
	else if(strcmp(mode, POWER_MANAGEMENT_HIGH) == 0){
		printf("ACCEL HIGH!\n");
		    
		delay_alarm = DELAY_ALARM_HIGH;
		delay_control = DELAY_CONTROL_HIGH;
		delay_info = DELAY_INFO_HIGH;	
	}
}

int cmd_handler(int argc, char **argv){
    (void)argc;
    (void)argv;
    
    char* mode = argv[1];
    
    // Saving pm mode into a global variable
    pm_mode = mode;
    
    printf("pm_mode = %s\n",pm_mode);
    
    if(strcmp(mode, POWER_MANAGEMENT_GENERAL) != 0){
	    fixedPowerManagement(mode);   	
	}
	
	else{
	    printf("Power management: general mode inserted!\n");
	}
	
	thread_create(stackThreadAlarm,sizeof(stackThreadAlarm),THREAD_PRIORITY_MAIN-1,0,threadAlarm,NULL,"Thread Alarm detection");
	printf("Thread alarm created! \n");
	
	thread_create(stackThreadControl,sizeof(stackThreadControl),THREAD_PRIORITY_MAIN-1,0,threadControl,NULL,"Thread Control");
	printf("Thread control created! \n");
	
	thread_create(stackThreadInfo,sizeof(stackThreadInfo),THREAD_PRIORITY_MAIN-1,0,threadInfo,NULL,"Thread Info ");
	printf("Thread info created! \n");
		
    return 0;
}

static const shell_command_t shell_commands[] = {
    { "pmtest", "starts the application with a certain power management level", cmd_handler},
    { NULL, NULL, NULL }
};


int main(void){	
		
	/* we need a message queue for the thread running the shell in order to
     * receive potentially fast incoming networking packets */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

    /* start shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should be never reached */
    return 0;
    
}
