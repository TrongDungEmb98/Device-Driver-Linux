/*
 * SIUL2 GPIO support.
 *
 * Copyright (c) 2016 Freescale Semiconductor, Inc.
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 or
 * later as publishhed by the Free Software Foundation.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include "./hw_udooneo_extended.h"

#define ONE_GPIO_SIZE		(((int)PAGE_SIZE) * 4)
#define MODULE_GPIO_SIZE	(ONE_GPIO_SIZE * 7)
#define MODULE_IOMUX_SIZE	(((int)PAGE_SIZE) * 4)

static int dev_open(struct inode *, struct file *);

static int dev_close(
	struct inode *,
	struct file *);

static ssize_t dev_read(
	struct file *,
	char __user *,
	size_t, loff_t *);

static ssize_t dev_write(
	struct file *,
	const char __user *,
	size_t, loff_t *);

static long dev_ioctl(
	struct file *fd,
	unsigned int cmd,
	unsigned long arg);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TRONG DUNG");
MODULE_DESCRIPTION("THIS IS MY FIRST KERNEL MODULE");

static dev_t  dev;
static struct cdev   c_dev;
static struct class  *class_p;
static struct device *device_p;

static const struct file_operations fops = {
	.open = dev_open,
	.release = dev_close,
	.read = dev_read,
	.write = dev_write,
	.unlocked_ioctl = dev_ioctl,
};

char kernel_buf[100];


static int dev_open(
	struct inode *inodep,
	struct file *filep)
{
	pr_info("Open device file\n");
	return 0;
}
static int dev_close(struct inode *inodep, struct file *filep)
{
	pr_info("Closed device file\n");
	return 0;
}
static ssize_t dev_read(
	struct file *filep,
	char __user *buf,
	size_t len, loff_t *offset)
{
	copy_to_user(buf, kernel_buf, strlen(kernel_buf));
	pr_info("Read device file\n");
	return strlen(kernel_buf);
}
static ssize_t dev_write(
	struct file *filep,
	const char __user *buf,
	size_t len, loff_t *offset)
{
	long size = 0;

	size = copy_from_user(kernel_buf, buf, len-1);
	pr_info("Write device file value %s\n", kernel_buf);
	return len;
}

static long dev_ioctl(
	struct file *fd,
	unsigned int cmd,
	unsigned long arg)
{
	return 0;
}

static int __init init_example(void)
{
	int *gpio = NULL;
	int *iomux = NULL;
	int res = 0;

	pr_info("Insmod kernel module driver led\n");

	if (alloc_chrdev_region(&dev, 0, 1, "my_dev") < 0) {
		pr_info("Error occur, can not register major number\n");
		goto alloc_dev_failed;
	}

	pr_info("<Major, Minor>: <%d, %d>\n", MAJOR(dev), MINOR(dev));

	class_p = class_create(THIS_MODULE, "class_driver_test");
	if (class_p == NULL) {
		pr_info("Error occur, can not create class device\n");
		goto create_class_failed;
	}

	device_p = device_create(class_p, NULL, dev, NULL, "driver_test");
	if (device_p == NULL) {
		pr_info("Can not create device\n");
		goto create_device_failed;
	}

	cdev_init(&c_dev, &fops);
	c_dev.owner = THIS_MODULE;
	c_dev.dev = dev;
	res = cdev_add(&c_dev, dev, 1);
	if (res) {
		pr_err("error occur when add properties for struct cdev\n");
		goto cdev_add_fail;
	}

	iomux = ioremap(IOMUXC_BASE, MODULE_IOMUX_SIZE);
	if (iomux == NULL) {
		pr_err("Can not mapping iomux physical address with virtual address\n");
		goto iomux_map_failed;
	}

	gpio = ioremap(GPIO_BASE, MODULE_GPIO_SIZE);
	if (gpio == NULL) {
		pr_err("Can not mapping gpio physical address to virtual address\n");
		goto gpio_map_failed;
	}

	res = gpio_init(gpio, iomux, GPIO1, 4, "OUTPUT");
	if (res == -1) {
		pr_err("Error occur when init gpio\n");
		goto gpio_init_failed;
	}

	res = gpio_init(gpio, iomux, GPIO1, 5, "INPUT");
	if (res == -1) {
		pr_err("Error occur when init gpio\n");
		goto gpio_init_failed;
	}

	return 0;

gpio_init_failed:
	iounmap(gpio);
gpio_map_failed:
	iounmap(iomux);
iomux_map_failed:
	cdev_del(&c_dev);
cdev_add_fail:
	device_destroy(class_p, dev);
create_device_failed:
	class_destroy(class_p);
create_class_failed:
	unregister_chrdev_region(dev, 1);
alloc_dev_failed:
	return -1;
}

static void __exit exit_example(void)
{
	cdev_del(&c_dev);
	device_destroy(class_p, dev);
	class_destroy(class_p);
	unregister_chrdev_region(dev, 1);
	pr_info("Rmmode kernel module driver led\n");
}

module_init(init_example);
module_exit(exit_example);