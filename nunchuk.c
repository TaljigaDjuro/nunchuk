/* INCLUDES */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/delay.h>

#include "nunchuk.h"

/* DEFINES */
#define RET_SUCCESS             0
#define RET_ERR                 -1
#define DEVICE_NAME             "nunchuk"
#define NUNCHUK_I2C_ADDRESS     0x52
#define NUNCHUK_I2C_BUS         1

/* PROTOTYPES */
static int init_nunchuk_module(void);
static void deinit_nunchuk_module(void);

static ssize_t nunchuk_read(struct file *, char *, size_t, loff_t *);
static int nunchuk_open(struct inode *, struct file *);
static int nunchuk_release(struct inode *, struct file *);

/* GLOBALS */
static int major_number;
static int opened;
static struct i2c_client* nunchuk_client;

static struct file_operations fops = {
    .read = nunchuk_read,
    .open = nunchuk_open,
    .release = nunchuk_release,
};

static int nunchuk_handshake(void)
{
	//pisi
	char buffer[] = {0xF0, 0x55};	
	i2c_master_send(nunchuk_client, buffer, 2);
	udelay(1);
	buffer[0] = 0xFB;
	buffer[1] = 0x00;
	i2c_master_send(nunchuk_client, buffer, 2);
	udelay(1);	
    return RET_SUCCESS;
}

static int nunchuk_read_registers(struct i2c_client *client, u8 *buf,
								  int buf_size)
{
	char sig = 0x00;
	i2c_master_send(nunchuk_client, &sig, 1);
	mdelay(10);
	i2c_master_recv(nunchuk_client, buf, 6);
	
	//udelay(100);

	return RET_SUCCESS;
}


/* FUNCTIONS */
static int init_nunchuk_module(void)
{
    struct i2c_adapter* bus_adapter;
    
    major_number = register_chrdev(0, DEVICE_NAME, &fops);

    if (major_number < 0)
    {
      printk(KERN_ALERT "Registering char device failed with %d\n", 
            major_number);
      
      return major_number;
    }
    
    printk(KERN_INFO "'mknod /dev/%s c %d 0'.\n", DEVICE_NAME, major_number);
     
    bus_adapter = i2c_get_adapter(NUNCHUK_I2C_BUS);
    
    if (bus_adapter == NULL)
    {
        return -ENODEV;
    }
    
    nunchuk_client = i2c_new_dummy(bus_adapter, NUNCHUK_I2C_ADDRESS);
    
    if (nunchuk_client == NULL)
    {
        return -ENODEV;
    }
    
    if (nunchuk_handshake() == RET_ERR)
    {
        return RET_ERR;
    }

    return RET_SUCCESS;
}

static void deinit_nunchuk_module(void)
{
    i2c_unregister_device(nunchuk_client);
    
    unregister_chrdev(major_number, DEVICE_NAME);
}

static int nunchuk_open(struct inode *inode, struct file *file)
{
    if (opened)
    {
        return -EBUSY;
    }

    opened++;
    
    try_module_get(THIS_MODULE);

    return RET_SUCCESS;
}

static int nunchuk_release(struct inode *inode, struct file *file)
{
    opened--;

    module_put(THIS_MODULE);

    return RET_SUCCESS;
}



static ssize_t nunchuk_read(struct file *filp, char *buffer, size_t length, 
                           loff_t * offset)
{
	
	//int i = 0;
	char buf[6];
	char z,c;
	nunchuk_read_registers(nunchuk_client, buf, 6);
	z = c = buffer[5];
	z = z & 1;
	c = c & 2;
	c = c >> 1;
	//printk(KERN_INFO "[%d %d %d %d]",  buf[0],buf[1],z,c);
	put_user(buf[0], buffer++);
	put_user(buf[1], buffer++);
	put_user(z, buffer++);
	put_user(c, buffer++);
    return 4;
}

module_init(init_nunchuk_module);
module_exit(deinit_nunchuk_module);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Stefan Savic <stefan.savic@rt-rk.com>");
MODULE_DESCRIPTION("This is a simple nunchuk controller driver.");
