#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h> /* mmap */
#include <errno.h>

#include "mad_ioctl.h"

#define PAGE_SIZE 4096

int main (int argc, char ** argv)
{
    uint64_t * p_virtadd = 0;
    int ret = 0;
    int fd = 0;

    struct mad_mo mo;
    
    fd = open("/dev/"MAD_DEV_FILENAME, O_RDWR | O_SYNC, 0);
    if ( 0 > fd )
    {
        printf("Cannot open device file: %s\n", MAD_DEV_FILENAME);
        return -1;
    }

    /* Test ioctl */
    mo.size = 0x999;
    /* Align for page size */
    if ( mo.size % PAGE_SIZE )
    {
        mo.size = (mo.size/PAGE_SIZE) * PAGE_SIZE + PAGE_SIZE;
    }

    ret = ioctl(fd, MAD_IOCTL_MALLOC, &mo);

    if ( 0 > ret )
    {
        printf("ioctl MAD_IOCTL_MALLOC failed: %d\n", ret);
        return -1;
    }
    p_virtadd = (uint64_t)mo.virtaddr;
    printf("KERNEL MODE: Reserved at phyaddr 0x%lx, virtadd 0x%lx, size 0x%lx\n", mo.phyaddr, p_virtadd, mo.size);

    /* At this point, we have reserved physical memory accessible by the virtual memory
       pointer p_virtadd but only in kernel mode.
       Memory remapping is required to have this memory accesible in user land.
    */
    //p_virtadd[0] = 0xcafebabe; /* Guaranteed segfault */

    p_virtadd = mmap(NULL, mo.size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, mo.phyaddr);

    if ( 0 == p_virtadd ) 
    {
	printf("mmap failed errno: %d %s\n", errno, strerror(errno));
        ioctl(fd, MAD_IOCTL_FREE, &mo);
        close(fd);
        return -1;        
    }
    printf("USER LAND: Reserved at phyaddr 0x%lx, virtadd 0x%lx, size 0x%lx\n", mo.phyaddr, p_virtadd, mo.size);

    printf("Writing 0x%x to the origin of the reserved space\n", 0xcafebabe);
    p_virtadd[0] = 0xcafebabe;
    printf("Checking written value: 0x%x\n", p_virtadd[0]);


    printf("Freeing memory...\n");
    ret = ioctl(fd, MAD_IOCTL_FREE, &mo);

    if ( 0 > ret )
    {
        printf("ioctl MAD_IOCTL_FREE failed: %d\n", ret);
	close(fd);
        return -1;
    }

    close(fd);
    printf("Memory freed\n");

    return 1;
}
