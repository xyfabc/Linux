/*
 * 	drivers/char/c210com_i2c.c
 *
 *	Simple com_i2c interface for Hanwang C210 board
 *
 *	Copyright (C) 2006 sunzheng
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 *	as published by the Free Software Foundation; either version
 *	2 of the License, or (at your option) any later version.
 *
 */

//#include <linux/config.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/miscdevice.h>
#include <linux/ioport.h>
#include <linux/init.h>
#include <linux/spinlock.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/slab.h>

#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/system.h>
#include <asm/irq.h>

#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <asm/io.h>
#include <asm/system.h>
#include <asm/uaccess.h>

#include <linux/device.h>

#include <asm/jzsoc.h>
#include "AT88.h"

#define com_I2C_DELAY_SLOTS		8

unsigned char pre5CLK(void);



#define DEBUG_MESG_GOLF
#undef DEBUG_MESG_GOLF

#define GOLF_TRY_AT
#undef GOLF_TRY_AT


/////////////////////////////////////////////////////
//#define DEBUG
#ifdef DEBUG
#define DPRINTK(x...) printk(##x)
#else
#define DPRINTK(x...)
#endif

#define COM_I2C_MINOR	230

struct com_i2c_dev                                     
{                                                        
  struct cdev cdev; /*cdev结构体*/                       
};

static int com_i2cdev_major = COM_I2C_MINOR;
struct com_i2c_dev *com_i2c_devp;

static int com_i2c_ref_count = 0;


unsigned char GDATAn;
unsigned char GCLKn;

/*****************************************************************************
 *                          Capturer Operations                              *
 *****************************************************************************/

static void com_I2C_Delay(unsigned int count)
{
#if 0
	int i,j;
	for (i=0; i<10 ; i++)
	{
		for (j=0; j < count; j++)
		{
			udelay(1);	
		}	
	}
#else
	ndelay(200);
#endif
}

static void com_SetGPxDir(unsigned char dir, unsigned char GPx_BP)
{
	spinlock_t ep_lock = SPIN_LOCK_UNLOCKED;

	unsigned long flags;

	spin_lock_irqsave(&ep_lock, flags);
	
	if(dir !=0 )
	{
		__gpio_as_output(GPx_BP);
		__gpio_clear_pin(GPx_BP);
	}
	else
	{
		__gpio_as_input(GPx_BP);
		__gpio_disable_pull(GPx_BP);
	}
	
	spin_unlock_irqrestore(&ep_lock, flags);
}

static void com_i2c_init(unsigned long GPnX)
{
	spinlock_t ep_lock = SPIN_LOCK_UNLOCKED;
	unsigned long flags;

	GDATAn = AT_SDA;
	GCLKn = AT_SCK;

	spin_lock_irqsave(&ep_lock, flags);

	__gpio_as_output1(GDATAn);
	__gpio_as_output1(GCLKn);

	spin_unlock_irqrestore(&ep_lock, flags);
}

//----------------------------------------------
void com_I2CStart()
{   
	spinlock_t ep_lock = SPIN_LOCK_UNLOCKED;

	unsigned long flags;

	spin_lock_irqsave(&ep_lock, flags);

	__gpio_set_pin(GDATAn);
	__gpio_set_pin(GCLKn);
	com_I2C_Delay(com_I2C_DELAY_SLOTS);       /* add I2C_Delay */	
	__gpio_clear_pin(GDATAn);
	com_I2C_Delay(com_I2C_DELAY_SLOTS);       /* then followed by SCL line with time I2C_Delay */
	__gpio_clear_pin(GCLKn);

	spin_unlock_irqrestore(&ep_lock, flags);

}

//----------------------------------------------
unsigned char com_I2CStop()
{
	spinlock_t ep_lock = SPIN_LOCK_UNLOCKED;

	unsigned long flags;

	spin_lock_irqsave(&ep_lock, flags);

	__gpio_clear_pin(GCLKn);
	com_I2C_Delay(com_I2C_DELAY_SLOTS);
	__gpio_clear_pin(GDATAn);
	com_I2C_Delay(com_I2C_DELAY_SLOTS);  
	__gpio_set_pin(GCLKn);
	com_I2C_Delay(com_I2C_DELAY_SLOTS);       /* then followed by SCL line with time I2C_Delay */		
	__gpio_set_pin(GDATAn);

	spin_unlock_irqrestore(&ep_lock, flags);

	return (0);        /* Return:  	"0" - the bus line is OK*/
}

//----------------------------------------------
unsigned char com_I2CMasterWrite(unsigned long input_byte)
{
	volatile unsigned char i;
	unsigned int mask;

	__gpio_clear_pin(GCLKn);
	__gpio_clear_pin(GDATAn);

	mask = 0x80;                      
	for (i=0; i<8; i++)		    /* send one byte of data */
	{
		if (mask & input_byte) 	/* send bit according to data */
			__gpio_set_pin(GDATAn);
		else 
			__gpio_clear_pin(GDATAn);

		mask = mask >> 1;		/* shift right for the next bit */
		com_I2C_Delay(com_I2C_DELAY_SLOTS);
		__gpio_set_pin(GCLKn);
		com_I2C_Delay (com_I2C_DELAY_SLOTS);
		__gpio_clear_pin(GCLKn);
	}

	com_SetGPxDir(GPIO_INPUT,GDATAn);
	com_I2C_Delay(com_I2C_DELAY_SLOTS);

	__gpio_set_pin(GCLKn);
	com_I2C_Delay(com_I2C_DELAY_SLOTS);	//lhs
	mask = __gpio_get_pin(GDATAn);	    /* read acknowledge */			    
	com_I2C_Delay(com_I2C_DELAY_SLOTS);
	
	__gpio_clear_pin(GDATAn);
	com_I2C_Delay(com_I2C_DELAY_SLOTS);			    /* to avoid short pulse transition on SDA line */
	com_SetGPxDir(GPIO_OUTPUT, GDATAn);

	//mdelay(2);
	ndelay(200);

	return (mask);          	/* return acknowledge bit */
}	

//----------------------------------------------
unsigned char com_I2CMasterRead(unsigned long ack)
{
	volatile unsigned char i;
	unsigned int mask, rec_data;    

	com_SetGPxDir(GPIO_INPUT, GDATAn);
	
	__gpio_clear_pin(GCLKn);
	com_I2C_Delay(com_I2C_DELAY_SLOTS);

	rec_data = 0;	
	mask = 0x80;	
	for (i=0; i<8; i++)
	{
		if (__gpio_get_pin(GDATAn))                    
			rec_data |= mask;
		mask = mask >> 1;   

		__gpio_set_pin(GCLKn);
		com_I2C_Delay(com_I2C_DELAY_SLOTS);      

		__gpio_clear_pin(GCLKn);
		com_I2C_Delay(com_I2C_DELAY_SLOTS);                                                                       
	} 

	com_SetGPxDir(GPIO_OUTPUT, GDATAn);
	if (ack)               		/* set SDA data first before port direction */	
		__gpio_set_pin(GDATAn);
	else 
		__gpio_clear_pin(GDATAn);

	com_I2C_Delay(com_I2C_DELAY_SLOTS);
	__gpio_set_pin(GCLKn);
	com_I2C_Delay(com_I2C_DELAY_SLOTS);
	__gpio_clear_pin(GCLKn);
	com_I2C_Delay(com_I2C_DELAY_SLOTS);			    /* to avoid short pulse transition on SDA line */
	
	//mdelay(2);
	ndelay(200);
	
	return (rec_data);
}

unsigned char pre5CLK(void)
{
	int i;
	com_SetGPxDir(GPIO_OUTPUT, GDATAn);
	com_SetGPxDir(GPIO_OUTPUT, GCLKn);

	__gpio_set_pin(GCLKn);
	__gpio_set_pin(GDATAn);
	

	for(i = 0 ; i < 50; i++)
	{
		
		com_I2C_Delay(com_I2C_DELAY_SLOTS); 
	}
	for(i = 0 ; i<50; i++)
	{
		__gpio_clear_pin(GCLKn);
		com_I2C_Delay(com_I2C_DELAY_SLOTS);
		__gpio_set_pin(GCLKn);
		com_I2C_Delay(com_I2C_DELAY_SLOTS);
	}
	return 1;
}

//----------------------------------------------

/*****************************************************************************
 *                        /dev/com_i2c file Operations                           *
 *****************************************************************************/

static int com_i2c_ioctl(struct inode *inode, struct file *file, unsigned int cmd,
		unsigned long arg)
{
	unsigned int ret = 0;

//	printk("com_i2c_ioctl enter cmd = %d arg =%ld \n",cmd,arg);
	switch (cmd) {
		case COM_I2C_INIT:
#ifdef GOLF_TRY_AT
			printk("0");
#endif
			com_i2c_init(arg);
			break;
		case COM_I2C_START:	/* obsoleted */
#ifdef GOLF_TRY_AT
			printk("1");
#endif
			com_I2CStart();
			break;
		case COM_I2C_STOP:	/* obsoleted */
#ifdef GOLF_TRY_AT
			printk("2");
#endif
			com_I2CStop();
			break;
		case COM_I2C_WRITE:	/* obsoleted */
#ifdef GOLF_TRY_AT
			printk("3");
#endif
			ret = com_I2CMasterWrite(arg);
			break;
		case COM_I2C_READ:	/* obsoleted */
#ifdef GOLF_TRY_AT
			printk("4");
#endif
			ret = com_I2CMasterRead(arg);
			break;
		default:
			ret = -EINVAL;
			break;
	}
//	printk("com_i2c_ioctl exit ret = %d \n",ret);
#ifdef GOLF_TRY_AT
	printk("_'%x'_", ret);
#endif
	return ret;
}

/*
 *	We enforce only one user at a time here with the open/close.
 */
static int com_i2c_open(struct inode *inode, struct file *file)
{
	/* can only be opened once simultaneously */
	printk("AT88SC open!\n");
	if (com_i2c_ref_count)
		return -EBUSY;
	
	com_i2c_ref_count++;

	return 0;
}

static int com_i2c_release(struct inode *inode, struct file *file)
{
	com_i2c_ref_count--;

	return 0;
}


static struct file_operations com_i2c_fops = {
	.owner =	THIS_MODULE,
	.ioctl =	com_i2c_ioctl,
	.open =	com_i2c_open,
	.release = com_i2c_release,
};


struct class *at88_class;

static int Relay_create_device(int major_num, char *dev_name, char *class_name)
{
	at88_class = class_create(THIS_MODULE, class_name);
	device_create(at88_class, NULL, MKDEV(major_num, 0), dev_name, "AT88SC");

	return 0;
}

int __init com_i2c_init_module(void)
{
	int result;

	result = register_chrdev(com_i2cdev_major, "AT88SC", &com_i2c_fops);
	if(result<0)
	{
		printk("\nFALLED: Cannot register relay_driver! \n");
		
		return result;
	}
	else
	{
		printk("Register com_i2c_driver OK!\n");
	}
	
	Relay_create_device(com_i2cdev_major, "AT88SC", "class_AT88SC");

	return 0;
}

void __exit com_i2c_exit_module(void)
{
	unregister_chrdev(com_i2cdev_major, "AT88SC");
	device_destroy(at88_class, MKDEV(com_i2cdev_major, 0));
	class_destroy(at88_class);
}

module_init(com_i2c_init_module);
module_exit(com_i2c_exit_module);

MODULE_DESCRIPTION("GPIO Driver for FK168 Board");
MODULE_LICENSE("GPL");
EXPORT_NO_SYMBOLS;
