#include <stdio.h>

int main()
{
  unsigned int cs_register_value;
  unsigned int privilege_level;

  /* Use inline assembly to get the value of the CS register */
  asm("mov %%cs, %0" : "=r" (cs_register_value));
  /* Extract the least significant 2 bits to determine the privilege level */
  privilege_level = cs_register_value & 0x03;

  if (privilege_level == 0)
      printf("Running in Kernel Mode\n");
  else
      printf("Running in User Mode\n");

  return 0;
}
