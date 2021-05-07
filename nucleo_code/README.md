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

## Alarm thread

This thread initializes








