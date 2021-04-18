#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "periph_conf.h"
#include "periph/i2c.h"
#include "xtimer.h"
#include "mutex.h"
#include "servo.h"
#include "thread.h"
#include <inttypes.h>
#include "mpu9x50.h"
#include "mpu9x50_params.h"

//define I2C parameters
#define I2C_INTERFACE I2C_DEV(0)
#define BAUDRATE (115200U)
#define ARDUINO_ADDRESS (0X04)

//define command's number and buffer size
#define INFO_BUFFER_SIZE 30
#define INFO_CMD 3

#define CONTROL_BUFFER_SIZE 10
#define CONTROL_CMD 2

#define ALARM_BUFFER_SIZE 1
#define ALARM_CMD 1
#define ALARM_THRESHOLD 2

//time interval for each thread
#define DELAY_INFO 30
#define DELAY_ALARM 1
#define DELAY_CONTROL 1000000


//constant for servomotor
#define SERVO_MIN 550
#define SERVO_MAX 2500
#define DEGREE_MAX 360
#define DEGREE_TO_US(x) ((x*(SERVO_MAX-SERVO_MIN)/DEGREE_MAX)+SERVO_MIN)

//constant for correcting the floater orientation with the compass
#define FROM_RIGHT_TO_LEFT_CORRECTION_THRESHOLD 1
#define FROM_LEFT_TO_RIGHT_CORRECTION_THRESHOLD -1


static char stackThreadControl[THREAD_STACKSIZE_DEFAULT];
static char stackThreadAlarm[THREAD_STACKSIZE_DEFAULT];
static char stackThreadInfo[THREAD_STACKSIZE_DEFAULT];

//define mutex to protect i2c access
mutex_t mutex;


void I2CCommunication(int cmd,void*buffer,int len,char* service)
{
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

void controlServoOrientation(servo_t* servo,int degree){
	servo_set(servo,DEGREE_TO_US(degree));
}


void controlBuzzer(gpio_t buzzer,int status)
{
	if(status==0)
		gpio_clear(buzzer);
	else if(status==1)
		gpio_set(buzzer);
}


void getControlData(mpu9x50_t dev,char* correctionData,int len, short* x_axis)
{
		//Data structure to save info
		mpu9x50_results_t measurement;
    
	    mutex_lock(&mutex);
	
		/* Get accel data in milli g */
        //mpu9x50_read_accel(&dev, &measurement);
        //printf("[MPU9250] Accel data [milli g] - X: %"PRId16"   Y: %"PRId16"   Z: %"PRId16"\n",
        //        measurement.x_axis, measurement.y_axis, measurement.z_axis);
        /* Get gyro data in dps */
        //mpu9x50_read_gyro(&dev, &measurement);
        //printf("[MPU9250] Gyro data [dps] - X: %"PRId16"   Y: %"PRId16"   Z: %"PRId16"\n",
        //        measurement.x_axis, measurement.y_axis, measurement.z_axis);
        /* Get compass data in mikro Tesla */
        mpu9x50_read_compass(&dev, &measurement);
        //printf("[MPU9250] Compass data [micro T] - X: %"PRId16"   Y: %"PRId16"   Z: %"PRId16"\n",
        //        measurement.x_axis, measurement.y_axis, measurement.z_axis);
        
        *x_axis = measurement.x_axis;
        
        
        printf("\n+-------------------------------------+\n");
		
		printf("%i \n",len);
		printf("%s \n",correctionData);
		printf("\n");
		
		mutex_unlock(&mutex);
		
	
}


static void* threadControl(void* arg)
{
	
	void* args=arg;
	args=NULL;
		
	if(args==NULL)
		printf(" ");
	
	char buffer[CONTROL_BUFFER_SIZE]={0};
	
	//definition of servo and its initialitation D3
	servo_t servo;
	
	if(servo_init(&servo,PWM_DEV(0),1,SERVO_MIN,SERVO_MAX)<0)
		{
			printf("[CONTROL] FAILED TO CONNECT TO SERVO!\n");
			return NULL;
		}
		
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
        return NULL;
    }
        
    printf("[MPU9250] Initialization successful\n\n");

	int degree=0, i=0;
	short x_start = 0;
	
	//get control data correction degree will be saved into buffer with sintax -360/360 as char*
	while(1)
	{
		
		//sample values for magnetic field detected by the compass
        short x_axis;
		
		//get control data correction degree will be saved into buffer with sintax -360/360 as char*
	    getControlData(dev,buffer,CONTROL_BUFFER_SIZE, &x_axis);	
		
		if(i==1){
		    x_start = x_axis;	
		}
		
		printf("x_start=%hd\n",x_start);
		
		printf("[MPU9250] Compass data [micro T] - X: %hd\n", x_axis);
		
		short int x_diff = x_start - x_axis;
		
		if(x_diff > FROM_RIGHT_TO_LEFT_CORRECTION_THRESHOLD){
		    printf("Rotate the floater to the left!\n");
		    
		    // controlServoOrientation(&servo,degree);
		    
		    // TO DO
		}
		
		else if(x_diff < FROM_LEFT_TO_RIGHT_CORRECTION_THRESHOLD){
		    printf("Rotate the floater to the right!\n");	
		    
		    // controlServoOrientation(&servo,degree);
		    
		    // TO DO
		}
		
		degree=atoi(buffer);
		controlServoOrientation(&servo,degree);
		printf("[CONTROL] Correction angle: %d \n",degree);
		
		if(i == 0 || i == 1){
		    i++;
		}
		
		xtimer_usleep(DELAY_CONTROL);
	}
	
	return NULL;
}




static void* threadAlarm(void* arg)
{

	void* args=arg;
	args=NULL;
	if(args==NULL)
		printf(" ");
	
	//definition and initialitation of buzzer using pin D9
	gpio_t buzzer;

	buzzer=GPIO_PIN(PORT_C,7);
	if(gpio_init(buzzer,GPIO_OUT)<0)
	{
		printf("[ALARM]Error during buzzer initialitation!\n");
		return NULL;
	}
	
	int cont=0;
	char buffer[ALARM_BUFFER_SIZE]={0};
	while(1)
	{
		I2CCommunication(ALARM_CMD,buffer,ALARM_BUFFER_SIZE,"ALARM");
		printf("[ALARM]MEX: %s \n",buffer);
		
		if(atoi(buffer)==1)
			cont++;
		else
			cont=0;
			
			
		
		if(cont==ALARM_THRESHOLD)
			controlBuzzer(buzzer,1);
		
		if(cont==0)
			controlBuzzer(buzzer,0);
		
		
		xtimer_sleep(DELAY_ALARM);
	
	}
	
	return NULL;
}


static void* threadInfo(void* arg)
{

	
	void* args=arg;
	args=NULL;
		
		
	if(args==NULL)
		printf(" ");
		
	char buffer[INFO_BUFFER_SIZE]={0};
	while(1)
	{	
	
		I2CCommunication(INFO_CMD,buffer,INFO_BUFFER_SIZE,"INFO");
		printf("[INFO]MEX: %s \n",buffer);
	
		xtimer_sleep(DELAY_INFO);
	
	}
	
	return NULL;
}

int main(void)
{
	
	//initialize the i2c interface
	i2c_init(I2C_INTERFACE);
	
		
	thread_create(stackThreadAlarm,sizeof(stackThreadAlarm),THREAD_PRIORITY_MAIN,0,threadAlarm,NULL,"Thread Alarm detection");
	printf("Thread proximity alarm created! \n");
	
	thread_create(stackThreadControl,sizeof(stackThreadControl),THREAD_PRIORITY_MAIN,0,threadControl,NULL,"Thread Control");
	printf("Thread control created! \n");
	
	thread_create(stackThreadInfo,sizeof(stackThreadInfo),THREAD_PRIORITY_MAIN,0,threadInfo,NULL,"Thread Info ");
	printf("Thread info created! \n");
	

}
