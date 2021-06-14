/* Memory Allocator Driver */

#include "mad.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Txar.li");
MODULE_DESCRIPTION("Memory Allocator Driver");
MODULE_VERSION("0.01");

/* Global variables */
static unsigned int bufsize = 0;

/* Module parameters */
module_param(bufsize, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(bufsize, "Buffer size");

static int mad_init(void)
{
    printk( KERN_NOTICE "mad: driver initialized\n" );
    printk( KERN_INFO "Bufsize: %d\n", bufsize);
    return 0;
}
    
static void mad_exit(void)
{
    return;
}
    
module_init(mad_init);
module_exit(mad_exit);
