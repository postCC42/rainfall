#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void p() {
    char buffer[76];
    int *var_10h;

    fflush(stdout);

    gets(buffer);

    // store the value pointed to by var_10h + 0x10 in var_10h
    *var_10h = *((int*)var_10h + 0x10);

    // perform bitwise AND operation on the value stored in var_10h
    // and check if the result is equal to 0xb0000000
    if ((*var_10h & 0xb0000000) == 0xb0000000) {
        // print the value stored in var_10h using the format string "%p"
        printf("(%p)\n", *var_10h);

        // exit the program with a status of 1
        exit(1);
    }

    puts(buffer);
    strdup(buffer);
}

int main() {
    p();  // Call the function p()
    return 0;
}
