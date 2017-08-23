#include <stdint.h>
#include <stdio.h>
#include "device.h"
#include "nunchuk.h"


#define CHAR_DEVICE     "/dev/nunchuk"


static uint8_t buffer[4];


int32_t main(void)
{
	char q = 0;
	int32_t fd = open_device(CHAR_DEVICE);
	if (fd == RET_ERR)
    	{
        	return RET_ERR;
    	}	
	while(q != 'q')
	{
	
		if (read_device(fd, buffer, 4) == RET_ERR)
		    {
			return RET_ERR;
		    }
		printf("[%d %d %d %d]\n", buffer[0],buffer[1],buffer[2],buffer[3]);
		q = getchar();
	}
	    if (close_device(fd) == RET_ERR)
	    {
		return RET_ERR;
	    }
	    
	    return RET_OK;
}
