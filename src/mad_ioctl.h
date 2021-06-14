#ifndef _MAD_IOCTL_H
#define _MAD_IOCTL_H

/*****************************************************************************
**  Header files
******************************************************************************/
#include <linux/ioctl.h>

/*****************************************************************************
**  Macro definition
******************************************************************************/

/*! Device file name */
#define MAD_DEV_FILENAME "mad"

/*! Major device number */
#define MAJOR_NUM 100

/*! Allocate physical memory */
#define MAD_IOCTL_MALLOC _IOWR(MAJOR_NUM, 0, struct mad_mo)

/*! Free physical memory */
#define MAD_IOCTL_FREE _IOWR(MAJOR_NUM, 1, struct mad_mo)


/*****************************************************************************
**  Object or Struct Declaration
******************************************************************************/

/*! Memory object structure 
 * @size: size requested in bytes
 * @phyaddr: physical address of the memory object
 */
struct mad_mo {
    uint64_t size;
    uint32_t phyaddr;
};

#endif /* _MAD_IOCTL_H */