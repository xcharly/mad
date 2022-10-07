/* Memory Allocator Driver */

#include "mad.h"
#include "mad_ioctl.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Txar.li");
MODULE_DESCRIPTION("Memory Allocator Driver");
MODULE_VERSION("0.01");

/* Global variables */
//static unsigned int bufsize = 0;

/* Module parameters */
//module_param(bufsize, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
//MODULE_PARM_DESC(bufsize, "Buffer size");

/*****************************************************************************
**  Global Variables
******************************************************************************/

/* File operations */
static struct file_operations Fops = {
    .owner = THIS_MODULE,
    .read  = mad_read,
    .unlocked_ioctl = mad_ioctl, /* .ioctl for kernel < 2.6.35 */
    .mmap = mad_mmap,
};

/* Miscellaneaous driver */
static struct miscdevice Misc_device_register = {
    .name  = MAD_DEV_FILENAME,
    .minor = MISC_DYNAMIC_MINOR,
    .fops  = &Fops,
    .mode  = S_IWUGO | S_IRUGO,
};

static struct device *Dev;

/* Platform driver */
static struct platform_driver mad_drv = {
    .probe  = mad_probe,
    .remove = mad_remove,
	.driver = {
		.name  = "mad",
        .owner = THIS_MODULE,
	},
};

/*****************************************************************************
** FUNCTION: mad_phy_malloc
** INPUTS: 
**  struct file * filp: file structure according to include/linux/fs.h
**  char        * buf : buffer including data
**  size_t        len : length of the buffer
**  loff_t      * off :
** OUTPUTS: 
** 
** COMMENTS: Return the number of bytes put into the buffer. 0 for EOF.
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
** Allocate physical memory for the memory object
*******************************************************************************/
static int mad_phy_malloc(struct mad_mo * mo)
{
    int ret = 1;
    dma_addr_t phyaddr;

    if ( NULL == mo )
    {
        return -1;
    }

    /* The devices can only address 32 bits*/
    ret = dma_set_mask_and_coherent (Dev, DMA_BIT_MASK(32));
    if ( 0 != ret )
    {
        printk(KERN_ERR "mad: Error allocating coeherent memory - dma_set_mask_and_coherent()");
        return -1;
    }

    /* Adjust the memory size to one page */
    mo->size = (mo->size + (PAGE_SIZE - 1)) & ~(PAGE_SIZE - 1);

    mo->virtaddr = dma_alloc_coherent(Dev, mo->size, &phyaddr, GFP_KERNEL);
    if ( mo->virtaddr == NULL )
    {
        printk(KERN_ERR "mad: Error allocating coeherent memory - mad_phy_malloc()");
        return -1;
    }

    mo->phyaddr = (uint64_t)phyaddr;
    mo->virt_to_phyaddr = virt_to_phys(mo->virtaddr);

    return 0;
}

static int mad_phy_free(struct mad_mo * mo)
{
    if ( NULL == mo )
    {
        return -1;
    }

    dma_free_coherent(Dev, mo->size, mo->virtaddr, mo->phyaddr);
    return 0;
}

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
                return -1;
            }

            if (mad_phy_malloc(&mo) < 0) {

                printk("mad_phy_malloc() failed.");
                return -1;
            }

            ret = copy_to_user( (void *)ioctl_param, &mo, sizeof(struct mad_mo) );
            if ( 0 < ret)
            {
                return -1;
            }
            break;
        
        case MAD_IOCTL_FREE:
            /* Import data from user space to kernel space */
            ret =  copy_from_user( &mo, (void *)ioctl_param, sizeof(struct mad_mo) );
            if ( 0 < ret)
            {
                return -1;
            }

            mad_phy_free(&mo);
            break;
    }

    return 1;
}

/**
 * @brief mad memory map implementation
 * @param[in]  file driver file descriptor
 * @param[in|out]  vma  linear memory region descriptor
 * @return 0 on success. -ECODE otherwise.
 */
static int mad_mmap(struct file *file, struct vm_area_struct *vma) {

	size_t size = vma->vm_end - vma->vm_start;

	MAD_DEBUG_LOG("mad: vma->vm_start = 0x%x\n", vma->vm_start);
	MAD_DEBUG_LOG("mad: vma->vm_end = 0x%x\n", vma->vm_end);
	MAD_DEBUG_LOG("mad: vma->vm_pgoff = 0x%x\n", vma->vm_pgoff);
    MAD_DEBUG_LOG("mad: vma->vm_page_prot = 0x%x\n", vma->vm_page_prot);
	MAD_DEBUG_LOG("mad: size = 0x%x\n", size);

	// vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);


	if (remap_pfn_range(vma, vma->vm_start, vma->vm_pgoff, size, vma->vm_page_prot)) {
		return -EAGAIN;
	}
	return 0;
}

/*****************************************************************************
** FUNCTION: mad_probe
** INPUTS: 
**  struct platform_device  * pdev : Pointer to the platform device structure
** OUTPUTS: 
** 
** COMMENTS: Return 0 on success, < 0 on error
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
** Probe call
*******************************************************************************/
static int mad_probe(struct platform_device * pdev)
{
    return 0;
}

/*****************************************************************************
** FUNCTION: mad_remove
** INPUTS: 
**  struct platform_device  * pdev : Pointer to the platform device structure
** OUTPUTS: 
** 
** COMMENTS: Return 0 on success, < 0 on error
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
** Unregister the device
*******************************************************************************/
static int mad_remove(struct platform_device * pdev)
{
    
    return 0;
}

/* Driver functions */
static int mad_init(void)
{
    int ret = 0;
    ret = platform_driver_register(&mad_drv);
    if ( 0 < ret )
    {
        printk( KERN_ERR "mad: cannot register platform: %d\n", ret );
    }

    ret = misc_register(&Misc_device_register);
    if ( 0 < ret )
    {
        printk ( KERN_ERR "mad: cannot register misc: %d\n", ret );
    }

    Dev = Misc_device_register.this_device;
    Dev->coherent_dma_mask = ~0;
    Dev->dma_mask = &Dev->coherent_dma_mask;
    
    printk( KERN_INFO "mad: driver registered\n");
    return ret;
}
    
static void mad_exit(void)
{
    misc_deregister(&Misc_device_register);
    platform_driver_unregister(&mad_drv);
}
    
module_init(mad_init);
module_exit(mad_exit);
 