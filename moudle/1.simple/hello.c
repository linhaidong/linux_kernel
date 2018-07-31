#include <linux/init.h>
#include <linux/module.h>

static int __init hello_init(void)
{
	printk("hello world module init!\n");
	printk("hello world module init!\n");
 	return 0;
}

static void __exit hello_exit(void)
{
	printk("hello world module exit!\n");
}

module_init(hello_init);
module_exit(hello_exit);

