#include "device.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>  /* ioctl */

int32_t open_device(const char* device)
{
    int32_t filedesc = open(device, O_RDWR);
    
    if(filedesc < 0)
    {
        printf("An error occurred in the open.\n");
        return RET_ERR;
    }
    
    return filedesc;
}

int32_t read_device(int32_t fd, uint8_t *data, int32_t length)
{
    if(read(fd, data, length) < 0)
    {
        printf("An error occurred in the read.\n");
        return RET_ERR;
    }
    
    return RET_OK;
}

int32_t close_device(int32_t fd)
{
    if(close(fd) < 0)
    {
        printf("An error occurred in the close.\n");
        return RET_ERR;
    }
    
    return RET_OK;
}

