/******************************************************************************
 	Author: 
	Soyabbir Abu Hanif and Hamzah Farooq
  
	 custom loadable karnel module (LKM) that will
     include metadata for version, authors (both members of the group should be separately reported), license, and description
	  greets when loading and when unloading
	  supports a load-time parameter to personalize the default greetings
 *
 *****************************************************************************/

#include <linux/module.h> //loading lkm into thekarnel
#include <linux/kernel.h> // for types, macros, functions for karnel
#include <linux/init.h> //  module initialization and cleanup macros

MODULE_LICENSE("GPL_v2");
MODULE_AUTHOR("Soyabbir Abu Hanif");
MODULE_AUTHOR("Hamzah Farooq");
MODULE_DESCRIPTION("A simple kernel module that greets on load and unload with a personalized message");
MODULE_VERSION("0.1");

static char *name = "Hello World";
module_param(name, charp, 0444); // 0444 is the permission value for read-only
MODULE_PARM_DESC(name, "The name to display in /var/log/kern.log");

static int __init hello_init(void) {
    printk(KERN_INFO "Hello: Welcome %s to the Hello LKM\n", name);
    return 0;
}

static void __exit hello_exit(void) {
    printk(KERN_INFO "Goodbye %s from the Hello LKM\n", name);
}

module_init(hello_init);
module_exit(hello_exit);
