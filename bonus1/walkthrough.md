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
0x08048448 <+36>:	mov    $0x1,%eax            
0x0804844d <+41>:	jmp    0x80484a3 <main+127> // otherwise go the end of main function
0x0804844f <+43>:	mov    0x3c(%esp),%eax      // move atoi(argv[1]) into eax 
0x08048453 <+47>:	lea    0x0(,%eax,4),%ecx    // 0 + eax * 4 -> ecx
0x0804845a <+54>:	mov    0xc(%ebp),%eax       // move the addr 0xc above ebp into eax
0x0804845d <+57>:	add    $0x8,%eax            // add 0x8 to eax
0x08048460 <+60>:	mov    (%eax),%eax          // dereference eax
0x08048462 <+62>:	mov    %eax,%edx            // eax -> edx
0x08048464 <+64>:	lea    0x14(%esp),%eax      // 
0x08048468 <+68>:	mov    %ecx,0x8(%esp)
0x0804846c <+72>:	mov    %edx,0x4(%esp)
0x08048470 <+76>:	mov    %eax,(%esp)
0x08048473 <+79>:	call   0x8048320 <memcpy@plt>   // memcpy(dest = 0x14(esp) , source = edx, num = ecx)
0x08048478 <+84>:	cmpl   $0x574f4c46,0x3c(%esp)   // 
0x08048480 <+92>:	jne    0x804849e <main+122>
0x08048482 <+94>:	movl   $0x0,0x8(%esp)           // value 0 in 0x8(%esp)
0x0804848a <+102>:	movl   $0x8048580,0x4(%esp)     // "/bin/sh"
0x08048492 <+110>:	movl   $0x8048583,(%esp)        // "sh"
0x08048499 <+117>:	call   0x8048350 <execl@plt>    // execl("sh", "/bin/sh", null)
0x0804849e <+122>:	mov    $0x0,%eax
0x080484a3 <+127>:	leave  
0x080484a4 <+128>:	ret  
```


## Exploit