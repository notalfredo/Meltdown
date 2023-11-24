# Meltdown

This project is for the class CSE System Cyber Security.

## System Requirements 
- x86 processor
- linux machine

> :warning: **original testing on a linx kerenl 5.4.x machine
**

## Setup


```
// Goto
sudo vim /etc/default/grub
```
Append `pti=off` and `nokaslr` to the end of the variable `GRUB_CMDLINE_LINUX_DEFAULT`

```
// cd back to repo
$ make
$ sudo insmod MeltdownKernel.ko
$ sudo dmesg | grep "secret"

/* 
   copy and paste address in meltdown.c 
   where meltdown_attack is first called
*/

$ gcc Meltdown.c
$ ./a.out
```
