/* Memory Allocator Driver */

#include "mad.h"
#include "mad_ioctl.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Txar.li");
MODULE_DESCRIPTION("Memory Allocator Driver");
MODULE_VERSION("0.01");

/* Global variables */
static unsigned int bufsize = 0;

/* Module parameters */
module_param(bufsize, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(bufsize, "Buffer size");

/*****************************************************************************
**  Global Variables
******************************************************************************/

/* File operations */
static struct file_operations Fops = {
    .read  = mad_read,
    .unlocked_ioctl = mad_ioctl /* .ioctl for kernel < 2.6.35 */
};

/* Platform driver */
static struct platform_driver mad_drv = {
	.driver = {
		.name = "mad",
	},
};

/*****************************************************************************
** FUNCTION: mad_read
** INPUTS: 
**  struct file * filp: file structure according to include/linux/fs.h
**  char        * buf : buffer including data
**  size_t        len : length of the buffer
**  loff_t      * off :
** OUTPUTS: 
** 
** COMMENTS: Return the number of bytes put into the buffer. 0 for EOF.
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
** Called when a process which already opened the dev file attempts to read
** from it.
*******************************************************************************/
static ssize_t mad_read(struct file * filp, char * buf, size_t len, loff_t * off)
{
    return 0;
}

/*****************************************************************************
** FUNCTION: mad_ioctl
** INPUTS: 
**  struct file  * file       : See include/linux/fs.h
**  unsigned int   ioctl_num  : Number for ioctl
**  unsigned long  ioctl_param: Parameter for ioctl
** OUTPUTS: 
** 
** COMMENTS:
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
** Called when a process does an ioctl on the device file.
*******************************************************************************/
static long mad_ioctl(struct file * f, unsigned int ioctl_num, unsigned long ioctl_param)
{  
    int ret = 0;
    struct mad_mo mo;

    switch ( ioctl_num )
    {
        case MAD_IOCTL_MALLOC:
            /* Import data from user space to kernel space */
            ret =  copy_from_user( &mo, (void *)ioctl_param, sizeof(struct mad_mo) );
            if ( 0 < ret)
            {
                return -EINVAL;
            }
            printk( KERN_NOTICE "mad: MAD_IOCTL_MALLOC: %d\n", mo.size );

            mo.phyaddr = 0x80;      

            ret = copy_to_user( (void *)ioctl_param, &mo, sizeof(struct mad_mo) );
            if ( 0 < ret)
            {
                return -EINVAL;
            }
            break;
        
        case MAD_IOCTL_FREE:
            printk( KERN_NOTICE "mad: MAD_IOCTL_FREE\n" );
            break;
    }

    return 1;
}

/* Driver functions */
static int mad_init(void)
{
    return platform_driver_register(&mad_drv);
}
    
static void mad_exit(void)
{
    platform_driver_unregister(&mad_drv);
}
    
module_init(mad_init);
module_exit(mad_exit);
