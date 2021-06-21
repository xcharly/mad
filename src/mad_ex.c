#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "mad_ioctl.h"

int main (int argc, char ** argv)
{
    uint64_t * p_virtadd = 0;
    int ret = 0;
    int fd = 0;

    struct mad_mo mo;
    
    fd = open("/dev/"MAD_DEV_FILENAME, 0);
    if ( 0 > fd )
    {
        printf("Cannot open device file: %s\n", MAD_DEV_FILENAME);
        return -1;
    }

    /* Test ioctl */
    mo.size = 0x999;
    ret = ioctl(fd, MAD_IOCTL_MALLOC, &mo);

    if ( 0 > ret )
    {
        printf("ioctl MAD_IOCTL_MALLOC failed: %d\n", ret);
        return -1;
    }
    p_virtadd = (uint64_t)mo.virtaddr;
    printf("Reserved at phyaddr 0x%lx, virtadd 0x%lx, size 0x%lx\n", mo.phyaddr, p_virtadd, mo.size);

    //p_virtadd[0] = 0xdeadbeefcafebabe;
    //p_virtadd[1] = 0x0123456789abcdef;

    p_virtadd[0] = 0xcafebabe;
    //p_virtadd[1] = 0x12345678;

    ret = ioctl(fd, MAD_IOCTL_FREE, &mo);

    if ( 0 > ret )
    {
        printf("ioctl MAD_IOCTL_FREE failed: %d\n", ret);
        return -1;
    }

    printf("Memory freed\n");


    return 1;
}