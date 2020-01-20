/******************************************************************************
#####         上海嵌入式家园-开发板商城         #####
#####                    www.embedclub.com                        #####
#####             http://embedclub.taobao.com               #####
* File：		beep_ioremap.c
* Author:		Hanson
* Desc：	Beep driver with ioremap
* History:	May 16th 2011
*******************************************************************************/

#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/input.h>
#include <linux/init.h>
#include <linux/serio.h>
#include <linux/delay.h>
#include <linux/clk.h>
#include <linux/miscdevice.h>
#include <linux/gpio.h>

#include <asm/io.h>
#include <asm/irq.h>
#include <asm/uaccess.h>
#include <mach/regs-clock.h>
#include <plat/regs-timer.h>
	 
#include <mach/regs-gpio.h>
//linux-2.6.32.2/arch/arm/mach-s3c2410/include/mach/regs-gpio.h
#include <linux/cdev.h>

#define DEVICE_NAME		"beep"

//Port GPBx  Register address declaration
#define GPBCON (unsigned long)ioremap(0x56000010,4)
#define GPBDAT (unsigned long)ioremap(0x56000014,4)
#define GPBUP  (unsigned long)ioremap(0x56000018,4)


MODULE_AUTHOR("Hanson He");
MODULE_LICENSE("Dual BSD/GPL");

#define BEEP_MAGIC 'k'
#define BEEP_START_CMD _IO (BEEP_MAGIC, 1)
#define BEEP_STOP_CMD _IO (BEEP_MAGIC, 2)

static unsigned int port_status =0;

/*
 * Open the device; in fact, there's nothing to do here.
 */
int beep_open (struct inode *inode, struct file *filp)
{
	return 0;
}

ssize_t beep_read(struct file *file, char __user *buff, size_t count, loff_t *offp)
{
	return 0;
}

ssize_t beep_write(struct file *file, const char __user *buff, size_t count, loff_t *offp)
{
	return 0;
}

/*
void beep_stop( void )
{
	//add your src HERE!!!
	//set GPB0 as output
	s3c2410_gpio_cfgpin(S3C2410_GPB(0), S3C2410_GPIO_OUTPUT);
	s3c2410_gpio_setpin(S3C2410_GPB(0),0);

}

void beep_start( void )
{
	//add your src HERE!!!
	//set GPB0 as output
	s3c2410_gpio_pullup(S3C2410_GPB(0),1);
	s3c2410_gpio_cfgpin(S3C2410_GPB(0), S3C2410_GPIO_OUTPUT);
	s3c2410_gpio_setpin(S3C2410_GPB(0),1);

}
*/
void beep_start( void )
{
	/*config GPBCON, set GPB0 as output port*/ 
	port_status = readl(GPBCON);
	port_status &= ~0x03;
	port_status |= 0x01;
	writel(port_status,GPBCON);

	port_status = readl(GPBDAT);	
	port_status |= 0x01; // set 1 to GPB0
	writel(port_status,GPBDAT);
}

void beep_stop( void )
{
	/*config GPBCON, set GPB0 as output port*/ 
	port_status = readl(GPBCON);
	port_status &= ~0x03;
	port_status |= 0x01;
	writel(port_status,GPBCON);

	port_status = readl(GPBDAT);
	port_status &= ~0x01;// set 0 to GPB0
	writel(port_status,GPBDAT);
}

static int beep_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
	//add your src HERE!!!
	switch ( cmd ) {
		case BEEP_START_CMD: {
			beep_start(); 	break;
		}
		case BEEP_STOP_CMD: {
			beep_stop(); 	break;
		}
		default: {
			break;
		}
	}
	return 0;

}

static int beep_release(struct inode *node, struct file *file)
{
	return 0;
}


/*
 * Our various sub-devices.
 */
/* Device 0 uses remap_pfn_range */
static struct file_operations beep_remap_ops = {
	.owner   = THIS_MODULE,
	.open    = beep_open,
	.release = beep_release,
	.read    = beep_read,
	.write   = beep_write,
	.ioctl   = beep_ioctl,	
};

/*
 * There's no need for us to maintain any
 * special housekeeping info, so we just deal with raw cdevs.
 */

static struct miscdevice misc = {
	.minor = MISC_DYNAMIC_MINOR, //动态设备号
	.name = DEVICE_NAME,
	.fops = &beep_remap_ops,
};

/*
 * Module housekeeping.
 */
static int beep_init(void)
{
	int ret;
	
	ret = misc_register(&misc);
	
	printk("The device name is: %s\n", DEVICE_NAME);
	return 0;
}


static void beep_cleanup(void)
{
	misc_deregister(&misc);
	printk("beep device uninstalled\n");
}


module_init(beep_init);
module_exit(beep_cleanup);

