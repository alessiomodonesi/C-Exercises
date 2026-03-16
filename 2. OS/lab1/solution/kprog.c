#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

/* This function is called when the module is loaded */
int simple_init(void)
{
  unsigned int cs_register_value;
  unsigned int privilege_level;

  printk(KERN_INFO "Loading Kernel Module\n");

  /* Use inline assembly to get the value of the CS register */
  asm("mov %%cs, %0" : "=r" (cs_register_value));
  /* Extract the least significant 2 bits to determine the privilege level */
  privilege_level = cs_register_value & 0x03;

  if (privilege_level == 0)
      printk(KERN_INFO "Running in Kernel Mode\n");
  else
      printk(KERN_INFO "Running in User Mode\n");

  return 0;
}

/* This function is called when the module is removed */
void simple_exit(void)
{
  printk(KERN_INFO "Removing Kernel Module\n");
}

/* Macros for registering module entry/exit points */
module_init(simple_init);
module_exit(simple_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple Module");
MODULE_AUTHOR("SGG");
