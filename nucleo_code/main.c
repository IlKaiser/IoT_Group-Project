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
#include "fmt.h"

//define I2C parameters
#define I2C_INTERFACE I2C_DEV(0)
#define BAUDRATE (115200U)
#define ARDUINO_ADDRESS (0X04)

//define command's number and buffer size
#define INFO_BUFFER_SIZE 30
#define INFO_CMD 4

#define CONTROL_BUFFER_SIZE 10
#define CONTROL_CMD_RIGHT 2
#define CONTROL_CMD_LEFT 3

#define ALARM_BUFFER_SIZE 1
#define ALARM_CMD 1
#define ALARM_THRESHOLD 2

//base time interval for each thread
#define DELAY_INFO_BASE 30
#define DELAY_ALARM_BASE 1
#define DELAY_CONTROL_BASE 1000000

//medium time interval for each thread
#define DELAY_INFO_MEDIUM 50
#define DELAY_ALARM_MEDIUM 10
#define DELAY_CONTROL_MEDIUM 10000000

//high time interval for each thread
#define DELAY_INFO_HIGH 70
#define DELAY_ALARM_HIGH 20
#define DELAY_CONTROL_HIGH 6000000

//constants for correcting the proximity orientation with the compass
#define CORRECTION_THRESHOLD 1
#define HIGH_CORRECTION_THRESHOLD 10

//constants for correcting the proximity orientation with the gyroscope
#define FROM_RIGHT_TO_LEFT_THRESHOLD -0.002
#define FROM_LEFT_TO_RIGHT_THRESHOLD 0.002

//acceleration constants for power management ( m/s^2 )
#define LOW_PM_THRESHOLD 9.5
#define HIGH_PM_THRESHOLD 10.0

//gravity acceleration constant
#define GRAVITY_ACCELERATION 9.8066

//fixed number for float conversion
#define FIXED_NUMBER 4.3

static char stackThreadControl[THREAD_STACKSIZE_DEFAULT];
static char stackThreadAlarm[THREAD_STACKSIZE_DEFAULT];
static char stackThreadInfo[THREAD_STACKSIZE_DEFAULT];

//define mutex to protect i2c access
mutex_t mutex;

//define delay variables for each thread
int delay_info = DELAY_INFO_BASE, delay_control = DELAY_CONTROL_BASE, delay_alarm = DELAY_ALARM_BASE;


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

void controlBuzzer(gpio_t buzzer,int status)
{
	if(status==0)
		gpio_clear(buzzer);
	else if(status==1)
		gpio_set(buzzer);
}

char* fromFloatToString(float f, unsigned precision){
    
    char* buf = (char*)malloc(10*sizeof(char));
	size_t n = fmt_float(buf, f, precision);
	buf[n] = '\0';
    
    return buf;
}

float calculateDegreeFromDps(short dps, float fixedNumber){
    float degree = (float)dps;
    
    degree = degree * fixedNumber;
    degree = degree / 3600;
    degree = degree / fixedNumber; 
    
    return degree;
}

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
        mpu9x50_read_compass(&dev, &measurement);
        //printf("[MPU9250] Compass data [micro T] - X: %"PRId16"   Y: %"PRId16"   Z: %"PRId16"\n",
        //        measurement.x_axis, measurement.y_axis, measurement.z_axis);
        
        *x_compass = measurement.x_axis;

		mutex_unlock(&mutex);
		
	
}

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

void proximitySensorCorrection(char* buffer, short x_start_compass, short previous_x_compass, short x_compass, short z_gyro){
    
    float degree = calculateDegreeFromDps(z_gyro, FIXED_NUMBER);
	
	short x_diff_compass = x_start_compass - x_compass;	
	int x_diff_compass_abs = abs((int)x_diff_compass);
	
	// Non-return point
	if(x_diff_compass_abs > HIGH_CORRECTION_THRESHOLD){
	    //printf("Non-return point!\n");
	    I2CCommunication(CONTROL_CMD_LEFT, buffer, CONTROL_BUFFER_SIZE, "CONTROL");
	    return;
	}
	
	// If the proximity sensor is too far from the starting point
	if(x_diff_compass_abs > CORRECTION_THRESHOLD){
		
		/* Evaluate if the floater is moving away or approaching the x_start_compass
		   by exploiting the previous measured value */
	    
	    short previous_x_diff_compass = x_start_compass - previous_x_compass;	
	    int previous_x_diff_compass_abs = abs((int)previous_x_diff_compass);
	    
	    // If the proximity sensor is moving to the left
	    if(degree > FROM_LEFT_TO_RIGHT_THRESHOLD){
			
			// If it is approaching
			if(x_diff_compass_abs < previous_x_diff_compass_abs){
				
				//printf("Continue moving the proximity sensor to the left!\n");
			    
			    // Send to Arduino the indication to continue move it to the left
			    I2CCommunication(CONTROL_CMD_LEFT, buffer, CONTROL_BUFFER_SIZE, "CONTROL");
		           	
			}
			
			// If it is moving away
			else if(x_diff_compass_abs > previous_x_diff_compass_abs){
				//printf("Rotate the proximity sensor to the right!\n");
			    
			    // Send to Arduino the indication to rotate it to the right
			    I2CCommunication(CONTROL_CMD_RIGHT, buffer, CONTROL_BUFFER_SIZE, "CONTROL");
			          	
			}
			
			// If it is remained in the same position as before
			/*else if(x_diff_compass_abs == previous_x_diff_compass_abs){
			    
			    printf("The floater is not moving!\n");
		           	
			}*/
		}
		
		// If the proximity sensor is moving to the right
		else if(degree < FROM_RIGHT_TO_LEFT_THRESHOLD){
			
			// If it is approaching
			if(x_diff_compass_abs < previous_x_diff_compass_abs){
			    //printf("Continue moving the proximity sensor to the right!\n");
			    
			    // Send to Arduino the indication to continue move it to the right
			    I2CCommunication(CONTROL_CMD_RIGHT, buffer, CONTROL_BUFFER_SIZE, "CONTROL");
			       	
			}
			
			// If it is moving away
			else if(x_diff_compass_abs > previous_x_diff_compass_abs){
				
				//printf("Rotate the proximity sensor to the left!\n");
			    
			    // Send to Arduino the indication to rotate it to the left
			    I2CCommunication(CONTROL_CMD_LEFT, buffer, CONTROL_BUFFER_SIZE, "CONTROL");
			       	
			}
			
			// If it is remained in the same position as before
			/*else if(x_diff_compass_abs == previous_x_diff_compass_abs){
			    
			    printf("The floater is not moving!\n");
		           	
			}*/
			
		}
		
		// If no movement is detected
		else if(degree == 0){
		    //printf("No movement detected!\n");
		}
	}
	
	// The proximity sensor is correctly orientated
	else{
	    //printf("The proximity sensor is correctly orientated\n"); 
	}
		
}

static void* threadControl(void* arg)
{
	
	void* args=arg;
	args=NULL;
		
	if(args==NULL)
		printf(" ");
	
	char buffer[CONTROL_BUFFER_SIZE]={0};
		
	// definition of MPU-device
	mpu9x50_t dev;
    int result;

    // MPU device initialization
    result = mpu9x50_init(&dev, &mpu9x50_params[0]);
    
    // Check errors
    if (result == -1) {
        puts("[MPU9250] The given i2c is not enabled");
       // return NULL;
    }
    else if (result == -2) {
        puts("[MPU9250] The compass did not answer correctly on the given address");
        //return NULL;
    }
    
    // Definition of sample rates
    mpu9x50_set_sample_rate(&dev, 200);
    if (dev.conf.sample_rate != 200) {
        puts("[MPU9250] The sample rate was not set correctly");
        //return NULL;
    }
    
    mpu9x50_set_compass_sample_rate(&dev, 100);
    
    if (dev.conf.compass_sample_rate != 100) {
        puts("[MPU9250] The compass sample rate was not set correctly");
        //return NULL;
    }

	int i=0;
	short x_start_compass = 0, previous_x_compass = 0;
	
	//sample values for magnetic field detected by the compass and accelerometer
    short z_gyro, x_compass, z_accel;
	
	//get control data correction degree will be saved into buffer with sintax -360/360 as char*
	while(1)
	{
		// Take the previous measure for the compass in order to improve the floater correction
		if (i > 1){
		    previous_x_compass = x_compass;	
		}
		
		//get control data correction degree will be saved into buffer with sintax -360/360 as char*
	    getControlData(dev, &z_gyro, &x_compass, &z_accel);	
		
		//trick to correctly take the initial reference for the floater
		if(i==1){
		    x_start_compass = x_compass;
		    previous_x_compass = x_compass;
		}
		
		// Proximity sensor correction
		proximitySensorCorrection(buffer, x_start_compass, previous_x_compass, x_compass, z_gyro);
		
		/* Power management
		float z_accell = ( (float) z_accel ) * GRAVITY_ACCELERATION / 1000;
		powerManagement(z_accell);*/
		
		//trick to correctly take the initial reference for the floater
		if(i == 0 || i == 1){
		    i++;
		}
		
		
		xtimer_usleep(delay_control);
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
		
		
		xtimer_sleep(delay_alarm);
	
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
	
		xtimer_sleep(delay_info);
	
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
