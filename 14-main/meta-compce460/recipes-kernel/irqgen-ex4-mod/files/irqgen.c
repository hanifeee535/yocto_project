/**
 * @file   irqgen.c
 * @author Nicola Tuveri
 * @date   11 October 2018
 * @version 0.2
 * @target_device Xilinx PYNQ-Z1
 * @brief   A stub module to support the IRQ Generator IP block for the
 *          Real-Time System course.
 */

#include <linux/init.h>             // Macros used to mark up functions e.g., __init __ex
#include <linux/module.h>           // Core header for loading LKMs into the kern
#include <linux/kernel.h>           // Contains types, macros, functions for the kernel

#include <linux/interrupt.h>        // Interrupt handling functions
#include <asm/io.h>                 // IO operations
#include <linux/slab.h>             // Kernel slab allocator


#include "irqgen.h"                 // Shared module specific declarations

/* Linux IRQ number for the first hwirq line */
//here, port is mapped to IRQ IDs 61-68 and 84-91
//and  grep -s 61 */hwirq gives output : 45/hwirq:61.
#define IRQGEN_FIRST_IRQ 45 // find the right Linux IRQ number for the first hwirq of the device
// Kernel token address to access the IRQ Generator core register
void __iomem *irqgen_reg_base = NULL;

// Module data instance
struct irqgen_data *irqgen_data = NULL;

// Dummy identifier for request_irq()
static int dummy;

/* vvvv ---- LKM Parameters vvvv ---- */
static unsigned int generate_irqs = 0;
module_param(generate_irqs, uint, 0444);
MODULE_PARM_DESC(generate_irqs, "Amount of IRQs to generate at load time.");

static unsigned int loadtime_irq_delay = 0;
module_param(loadtime_irq_delay, uint, 0444);
MODULE_PARM_DESC(loadtime_irq_delay, "Set the delay for IRQs generated at load time.");

/* Makes sure that the input values for parameters are sane */
static int parse_parameters(void)
{
    if (generate_irqs > IRQGEN_MAX_AMOUNT) {
        printk(KERN_WARNING KMSG_PFX "generate_irqs parameter exceeded maximum value: capped at %ld.\n",
                IRQGEN_MAX_AMOUNT);
        generate_irqs = IRQGEN_MAX_AMOUNT;
    }

    if (loadtime_irq_delay > IRQGEN_MAX_DELAY) {
        printk(KERN_WARNING KMSG_PFX "loadtime_irq_delay parameter exceeded maximum value: capped at %ld.\n",
                IRQGEN_MAX_DELAY);
        loadtime_irq_delay = IRQGEN_MAX_DELAY;
    }

    return 0;
    // return -EINVAL;
}
/* ^^^^ ---- LKM Parameters ^^^^ ---- */


/*  (1) implement the interrupt handler function */
static irqreturn_t irqgen_irqhandler(int irq, void *data)
{
#ifdef DEBUG
    printk(KERN_INFO KMSG_PFX "IRQ #%d received.\n", irq);
#endif

    // increment the `count_handled` counter before ACK
    irqgen_data->count_handled+=1; //calling from irqgen_data structure
    // Prepare the value to be written to the control register
    u32 regvalue = 0
            | FIELD_PREP(IRQGEN_CTRL_REG_F_ENABLE, 1) //enables the IRQ generator
            | FIELD_PREP(IRQGEN_CTRL_REG_F_HANDLED,1) //indicate the interrupt has been processed successfully.
            | FIELD_PREP(IRQGEN_CTRL_REG_F_ACK,0); // Clear the 'ACK' bit 
    // HINT: use iowrite32 and the bitfield macroes to modify the register fields
    //iowrite32() function is used to write a 32-bit value to an I/O-mapped register 
    //(in this case, the IRQ generator's control register).
    iowrite32(regvalue, IRQGEN_CTRL_REG);
    
    return IRQ_HANDLED; // returns IRQ_HANDLED to the kernel to indicate that the interrupt has been successfully handled.
}

/* Enable the IRQ Generator */
void enable_irq_generator(void)
{
#ifdef DEBUG
    printk(KERN_INFO KMSG_PFX "Enabling IRQ Generator.\n");
#endif
    // HINT: use iowrite32 and the bitfield macroes to modify the register fields
    u32 regvalue = FIELD_PREP(IRQGEN_CTRL_REG_F_ENABLE, 1);
    iowrite32(regvalue, IRQGEN_CTRL_REG);

}

/* Disable the IRQ Generator */
void disable_irq_generator(void)
{
#ifdef DEBUG
    printk(KERN_INFO KMSG_PFX "Disabling IRQ Generator.\n");
#endif
    // set to zero the `amount` field, then disable the controller
    u32 regvalue = 0
                | FIELD_PREP(IRQGEN_GENIRQ_REG_F_AMOUNT,  0) // sets the amount field of the register to 0
                | FIELD_PREP(IRQGEN_GENIRQ_REG_F_DELAY,   loadtime_irq_delay)
                | FIELD_PREP(IRQGEN_GENIRQ_REG_F_LINE,    0); //Setting it to 0 means  the IRQ generator is disabled
    // HINT: use iowrite32 and the bitfield macroes to modify the register fields
    
    iowrite32(regvalue,IRQGEN_GENIRQ_REG);
}

/* Generate specified amount of interrupts on specified IRQ_F2P line [IRQLINES_AMNT-1:0] */
void do_generate_irqs(uint16_t amount, uint8_t line, uint16_t delay)
{
    u32 regvalue = 0
                   | FIELD_PREP(IRQGEN_GENIRQ_REG_F_AMOUNT,  amount)
                   | FIELD_PREP(IRQGEN_GENIRQ_REG_F_DELAY,    delay)
                   | FIELD_PREP(IRQGEN_GENIRQ_REG_F_LINE,      line);

    printk(KERN_INFO KMSG_PFX "Generating %u interrupts with IRQ delay %u on line %d.\n",
           amount, delay, line);

    iowrite32(regvalue, IRQGEN_GENIRQ_REG);
}

// Returns the latency of last successfully served IRQ, in ns
u64 irqgen_read_latency(void)
{
    // not supported by current IP block implementation
    return ioread32(IRQGEN_LATENCY_REG) *10;
}

// Returns the total generated IRQ count from IRQ_GEN_IRQ_COUNT_REG
u32 irqgen_read_count(void)
{
    // use ioread32 to read the proper register
    return ioread32(IRQGEN_IRQ_COUNT_REG);
    //irqgen_read_count() reads the value of the interrupt count register.
    // which is the number of interrupts the IRQ generator has triggered since it was last reset or initialized.
}

// Debugging wrapper for request_irq()
#ifdef DEBUG
static
int _request_irq(unsigned int _irq, irq_handler_t _handler, unsigned long _flags, const char *_name, void *_dev)
{
    printk(KERN_DEBUG KMSG_PFX "request_irq(%u, %p, %lu, %s, %p)\n",
            _irq, _handler, _flags, _name, _dev);
    return request_irq(_irq, _handler, _flags, _name, _dev);
}
#else
# define _request_irq request_irq
#endif


// The kernel module init function
static int32_t __init irqgen_init(void)
{
    int retval = 0;

    printk(KERN_INFO KMSG_PFX DRIVER_LNAME " initializing.\n");

    retval = parse_parameters();
    if (0 != retval) {
        printk(KERN_ERR KMSG_PFX "fatal failure parsing parameters.\n");
        goto err_parse_parameters;
    }

    irqgen_data = kzalloc(sizeof(*irqgen_data), GFP_KERNEL);
    if (NULL == irqgen_data) {
        printk(KERN_ERR KMSG_PFX "Allocation of irqgen_data failed.\n");
        retval = -ENOMEM;
        goto err_alloc_irqgen_data;
    }

    /* Map the IRQ Generator core register with ioremap */
    
    irqgen_reg_base =  ioremap(IRQGEN_REG_PHYS_BASE,IRQGEN_REG_PHYS_SIZE); //from the address header file

    if (NULL == irqgen_reg_base) {
        printk(KERN_ERR KMSG_PFX "ioremap() failed.\n");
        retval = -EFAULT;
        goto err_ioremap;
    }

    /*  Register the handle to the relevant IRQ number */
    
    retval = _request_irq(IRQGEN_FIRST_IRQ,irqgen_irqhandler,0,"pynq",&dummy);/*  fill the first arguments */
    
    if (retval != 0) {
        printk(KERN_ERR KMSG_PFX "request_irq() failed with return value %d while requesting IRQ id %u.\n",
                retval, IRQGEN_FIRST_IRQ);
        goto err_request_irq;
    }

    retval = irqgen_sysfs_setup();
    if (0 != retval) {
        printk(KERN_ERR KMSG_PFX "Sysfs setup failed.\n");
        goto err_sysfs_setup;
    }

    /* Enable the IRQ Generator */
    enable_irq_generator();

    if (generate_irqs > 0) {
        /* Generate IRQs (amount, line, delay) */
        do_generate_irqs(generate_irqs, 0, loadtime_irq_delay);
    }

    return 0;

 err_sysfs_setup:
    //  free the appropriate resource when handling this error step
    irqgen_sysfs_cleanup();
 err_request_irq:
    // free the appropriate resource when handling this error step
    free_irq(IRQGEN_FIRST_IRQ,&dummy);
 err_ioremap:
    // free the appropriate resource when handling this error step
    iounmap(IRQGEN_REG_PHYS_BASE);
 err_alloc_irqgen_data:
 err_parse_parameters:
    printk(KERN_ERR KMSG_PFX "module initialization failed\n");
    return retval;
}

// The kernel module exit function
static void __exit irqgen_exit(void)
{
    // Read interrupt latency from the IRQ Generator on exit
    printk(KERN_INFO KMSG_PFX "IRQ count: generated since reboot %u, handled since load %u.\n",
           irqgen_read_count(), irqgen_data->count_handled);
    // Read interrupt latency from the IRQ Generator on exit
    printk(KERN_INFO KMSG_PFX "latency for last handled IRQ: %lluns.\n",
           irqgen_read_latency());


    // step through `init` in reverse order and disable/free/unmap allocated resources
    
    disable_irq_generator(); //Stops IRQs to prevent race conditions during cleanup.
    irqgen_sysfs_cleanup();   // place this line in the right order
    free_irq(IRQGEN_FIRST_IRQ,&dummy); //Release the IRQ to ensure no active handlers remain
    iounmap(IRQGEN_REG_PHYS_BASE); //Final cleanup of memory-mapped hardware registers after freeing IRQ and sysfs.
    printk(KERN_INFO KMSG_PFX DRIVER_LNAME " exiting.\n");
}

module_init(irqgen_init);
module_exit(irqgen_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Module for the IRQ Generator IP block for the realtime systems course");
MODULE_AUTHOR("Jan Lipponen <jan.lipponen@wapice.com>");
MODULE_AUTHOR("Nicola Tuveri <nicola.tuveri@tut.fi>");
MODULE_AUTHOR("Soyabbir Abu Hanif <mdsoyabbirabuhanif@tuni.fi>");
MODULE_AUTHOR("Hamzah Farooq <hamza.farooq@tuni.fi>");

MODULE_VERSION("0.2");

