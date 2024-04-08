# Level 9

## First clue

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