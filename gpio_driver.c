#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>

#include <linux/proc_fs.h>
#include <linux/slab.h>

#include <asm/io.h>

#define MAX_USER_SIZE 1024

#define BCM2711_GPIO_ADDRESS 0xFE200000

#define TAG "gpio_driver"

static struct proc_dir_entry *gpio_driver_proc = NULL;

static char data_buffer[MAX_USER_SIZE+1] = {0};

static unsigned int *gpio_registers = NULL;

static void gpio_pin_on(unsigned int pin)
{
	unsigned int fsel_index = pin/10;
	unsigned int fsel_bitpos = pin%10;
	unsigned int* gpio_fsel = gpio_registers + fsel_index;
	unsigned int* gpio_on_register = (unsigned int*)((char*)gpio_registers + 0x1c);

	*gpio_fsel &= ~(7 << (fsel_bitpos*3));
	*gpio_fsel |= (1 << (fsel_bitpos*3));
	*gpio_on_register |= (1 << pin);

	return;
}

static void gpio_pin_off(unsigned int pin)
{
	unsigned int *gpio_off_register = (unsigned int*)((char*)gpio_registers + 0x28);
	*gpio_off_register |= (1<<pin);
	return;
}

ssize_t gpio_driver_read(struct file *file, char __user *user, size_t size, loff_t *off)
{
	return 0;
}

ssize_t gpio_driver_write(struct file *file, const char __user *user, size_t size, loff_t *off)
{
	unsigned int pin = UINT_MAX;
	unsigned int value = UINT_MAX;

	memset(data_buffer, 0x0, sizeof(data_buffer));

	if (size > MAX_USER_SIZE)
	{
		size = MAX_USER_SIZE;
	}

	if (copy_from_user(data_buffer, user, size))
		return 0;

	printk("%s: Data buffer: %s\n", TAG, data_buffer);

	if (sscanf(data_buffer, "%d,%d", &pin, &value) != 2)
	{
		printk("Inproper data format submitted\n");
		return size;
	}

	if (pin > 26 || pin < 0)
	{
		printk("Invalid pin number submitted\n");
		return size;
	}

	if (value != 0 && value != 1)
	{
		printk("Invalid on/off value\n");
		return size;
	}

	printk("You said pin %d, value %d\n", pin, value);
	if (value == 1)
	{
		gpio_pin_on(pin);
	} else if (value == 0)
	{
		gpio_pin_off(pin);
	}

	return size;
}

static const struct proc_ops gpio_driver_proc_fops = 
{
	.proc_read = gpio_driver_read,
	.proc_write = gpio_driver_write,
};


static int __init gpio_driver_init(void)
{
	printk("%s: GPIO Driver initialized!\n", TAG);

	gpio_registers = (int*)ioremap(BCM2711_GPIO_ADDRESS, PAGE_SIZE);
	if (gpio_registers == NULL)
	{
		printk("Failed to map GPIO memory to driver\n");
		return -1;
	}

	printk("Successfully mapped in GPIO memory\n");

	// create an entry in the proc-fs
	gpio_driver_proc = proc_create("gpio_control", 0666, NULL, &gpio_driver_proc_fops);
	if (gpio_driver_proc == NULL)
	{
		return -1;
	}

	return 0;
}

static void __exit gpio_driver_exit(void)
{
	printk("Leaving my driver!\n");
	iounmap(gpio_registers);
	proc_remove(gpio_driver_proc);
	return;
}

module_init(gpio_driver_init);
module_exit(gpio_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TQMatvey");
MODULE_DESCRIPTION("GPIO control driver for Raspberry Pi 4B");
MODULE_VERSION("1.0");
