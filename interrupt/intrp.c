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
#include <../arch/arm/include/asm/io.h>

#define GICD_CTRL		0x000 // Distributer control reg
#define GICD_TYPER  	0x004 // Interrupt Controller type reg (RO)
#define GIC_IIDR		0x008 // Distributer Implement ID (RO)
#define GICD_IGROUPR	0x080 // Interrupt group reg
#define GICD_ISENABLER	0x100 // Set enable reg
#define GICD_ICENABLER  0x180 // clear enable reg
#define GICD_ISPENDR	0x200 // set pending
#define GICD_ICPENDR	0x280 // clear pending
#define GICD_ISACTIVER  0x300 // set active reg
#define GICD_ICACTIVER  0x380 // clear active reg
#define GICD_IPRIORITYR 0x400 // priority reg
#define GICD_ITARGETSR  0x800 // processor target
#define GICD_ICFGR      0xC00 // interrupt config reg
#define GICD_PPISR		0xD00 // private peripheral interrupt status
#define GICD_SPISR		0xD04 // shared peripheral interrupt status
#define GICD_SGIR		0xF00 // software generated interrupt reg
#define GICD_CPENDSGIR	0xF10 // SGI clear pending reg
#define GICD_SPENDSGIR	0xF20 // SGI set pending reg
#define GICD_PIDR4		0xFD0 // Peripheral ID 4 reg


#define DISTRIBUTER		0x02561000
#define INTERFACE  0x02562000
#define DIST_NAME	 "DISTSEC"
#define INTR_NAME	 "INTRSEC"
#define MSMC				0x0C000000
#define MSMCSRAM   "MSMCSRAM"

int init_module(void)  { 

	void __iomem *gic_virt;
	void __iomem *addr;


	struct resource* res;
	res = request_mem_region((unsigned long) DISTRIBUTER,4096,DIST_NAME);
	if(res == NULL)
	{
		printk("fialure to aquire memory region\n");
		return(-1);
	}

	gic_virt =  ioremap((phys_addr_t) DISTRIBUTER,4096);


	if(gic_virt == NULL)
	{
			printk("error in virtual address %x\n",gic_virt);
			return(-1);
	}

	unsigned int val;
	//Disable interrupt #36 for setup
	val = ioread32(gic_virt + GICD_ISENABLER + 4);
	printk("current value of enable reg is 0x%x\n",val);
	val = val & 0xFFFFFFEF;
	iowrite32(val,gic_virt + GICD_ISENABLER + 4);
	wmb();
	val = ioread32(gic_virt + GICD_ISENABLER + 4);
	printk("new value is 0x%x\n",val);

	//Configure interrupt #36 sensitivity
	val = ioread32(gic_virt + GICD_ICFGR + 8);
	printk("current value of GICD_ICFGR reg is 0x%x\n",val);
	val = val | 0x00000400;
	iowrite32(val,gic_virt + GICD_ICFGR + 8);
	wmb();
	val = ioread32(gic_virt + GICD_ICFGR + 8);
	printk("new value is 0x%x\n",val);

	//set target core
	val = ioread32(gic_virt + GICD_ITARGETSR + 36);
	printk("current value of GICD_ITARGETSR reg is 0x%x\n",val);
	val = val & 0xFFFFFF00;
	val = val | 0x0000000F;
	iowrite32(val,gic_virt + GICD_ITARGETSR + 36);
	wmb();
	val = ioread32(gic_virt + GICD_ITARGETSR + 36);
	printk("new value is 0x%x\n",val);

	//Enable interrupt #36 for setup
	val = ioread32(gic_virt + GICD_ISENABLER + 4);
	printk("current value of enable reg is 0x%x\n",val);
	val = val | 0x00000010;
	iowrite32(val,gic_virt + GICD_ISENABLER + 4);
	wmb();
	val = ioread32(gic_virt + GICD_ISENABLER + 4);
	printk("new value is 0x%x\n",val);

	
	printk("Interrupt is set enjoy :)\n");

	return 0;
	
}

void cleanup_module(void) { 

release_mem_region((unsigned long) DISTRIBUTER,4096);
printk("memory is released\n"); }


