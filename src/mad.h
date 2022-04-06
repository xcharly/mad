#ifndef _MAD_H
#define _MAD_H

/*****************************************************************************
**  Header files
******************************************************************************/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h> /* module_param() */
#include <linux/fs.h> /* Char device: file operation */
#include <linux/platform_device.h> /* platform_driver_register() */
#include <linux/uaccess.h> /* copy_from_user(), copy_to_user() */
#include <linux/miscdevice.h> /* Misc device */

#include <linux/dma-mapping.h> /* dma_alloc_coherent */

#include "mad_ioctl.h"

/*****************************************************************************
**  Macro definition
******************************************************************************/
#ifdef MAD_DEBUG
  #define MAD_DEBUG_LOG(fmt, ...) printk( KERN_INFO pr_fmt(fmt), ##__VA_ARGS__ )
#else
  #define MAD_DEBUG_LOG(fmt, ...)
#endif




/*****************************************************************************
**  Function Prototype Declaration
******************************************************************************/

static int mad_phy_malloc(struct mad_mo * mo);
static int mad_phy_free(struct mad_mo * mo);

/*! Called when a process which already opened the dev file attempts to read from it.
 \param[in] : filp : file structure according to include/linux/fs.h
 \param[in] : buf  : buffer including data 
 \param[in] : len  : length of the buffer 
 \param[in] : off
 \return ssize_t: Return the number of bytes put into the buffer. 0 for EOF.
*/
static ssize_t mad_read(struct file * filp, char * buf, size_t len, loff_t * off);

/*! Called when a process does an ioctl on the device file.
 \param[in] : file       : See include/linux/fs.h
 \param[in] : ioctl_num  : Number for ioctl
 \param[in] : ioctl_param: Parameter for ioctl
 \return 
*/
static long mad_ioctl(struct file * f, unsigned int ioctl_num, unsigned long ioctl_param);


/**
 * @brief mad memory map implementation
 * @param[in]  file driver file descriptor
 * @param[in|out]  vma  linear memory region descriptor
 * @return 0 on success. -ECODE otherwise.
 */
static int mad_mmap(struct file *file, struct vm_area_struct *vma);

/*! Probe call
 \param[in] : pdev : Pointer to the platform device structure
 \return : Return 0 on success, < 0 on error
*/
static int mad_probe(struct platform_device * pdev);

/*! Unregister the device
 \param[in] : pdev : Pointer to the platform device structure
 \return : Return 0 on success, < 0 on error
*/
static int mad_remove(struct platform_device * pdev);

static int mad_init(void);
static void mad_exit(void);

#endif /* _MAD_H */