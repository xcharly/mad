#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "mad_ioctl.h"

int main (int argc, char ** argv)
{
    int ret = 0;
    int fd = 0;

    struct mad_mo mo:
    
    fd = open(MAD_DEV_FILENAME, 0);
    if ( 0 > fd )
    {
        printf("Cannot open device file: %s\n", MAD_DEV_FILENAME);
        return -1;
    }

    /* Test ioctl */
    mo.size = 0x64;
    ret = ioctl(fd, MAD_IOCTL_MALLOC, mo);

    if ( 0 > ret )
    {
        printf("ioctl MAD_IOCTL_MALLOC failed: %d\n", ret);
        return -1;
    }

    ret = ioctl(fd, MAD_IOCTL_FREE, mo);

    if ( 0 > ret )
    {
        printf("ioctl MAD_IOCTL_FREE failed: %d\n", ret);
        return -1;
    }

    printf("mo.phyaddr: %d\n", mo.phyaddr);


    return 1;
}