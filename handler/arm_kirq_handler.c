#include <linux/module.h>
#include <linux/time.h>
#include <linux/unistd.h>
#include <linux/signal.h>
#include <linux/errno.h>
#include <linux/fcntl.h>
#include <linux/ctype.h>
#include <linux/types.h>
#include <linux/mman.h>
#include <linux/kernel.h>
#include <linux/smp.h>
#include <linux/fs.h>
#include <linux/ioport.h>
#include <linux/io.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/irq.h>
#include <linux/bitops.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/interrupt.h>
#include <linux/irqdomain.h>
#include <linux/irqchip.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/mfd/syscon.h>
#include <linux/regmap.h>

#define DEVNAME		"keystone-irq"

#define EnableIntrp  0x02561104
#define ClearPending 0x02561284




static int cnt = 0;

static irqreturn_t handler(int irq, void *devclear)
{
	cnt++;
	//printk("handling interrupt\n");
	return IRQ_HANDLED;
}
	
static int intrp_probe(struct platform_device *pdev)  { 

int stat;
int INTRP_ID;

INTRP_ID = platform_get_irq(pdev,0);
printk("irq num is %d\n",INTRP_ID);

stat = request_irq(INTRP_ID,
									 handler,
									 0,
									 DEVNAME,
									 NULL);
if(stat < 0)
{	
	printk("Interrupt is not registered\n");
	printk("ERROR CODE IS %d\n",stat);
	return(-1);
}

printk("Driver is registered correctly\n");
return 0;
}

static int intrp_remove(struct platform_device *pdev) {
int INTRP_ID;
INTRP_ID = platform_get_irq(pdev,0);
free_irq(INTRP_ID, NULL); 
printk("Driver is removed correctly\n");
return 0; 
}

static const struct of_device_id arm_intrp_id[] = {
{ .compatible = "myti,keystone-irq", },
	{},
};			

MODULE_DEVICE_TABLE(of, arm_intrp_id);

static struct platform_driver mydriver_arm = {
	.probe		= intrp_probe,
	.remove		= intrp_remove,
	.driver		= {
		.name	= DEVNAME,
		.of_match_table	= of_match_ptr(arm_intrp_id),
	}
};

module_platform_driver(mydriver_arm);


MODULE_AUTHOR("MAHMOUD ALASMAR");
MODULE_AUTHOR("ODTU THALES");
MODULE_DESCRIPTION("ARM IPCGR DRIVER");
MODULE_LICENSE("GPL v2");
										 
