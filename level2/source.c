#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void p() {
    char buffer[76];
    int *a;

    fflush(stdout);

    gets(buffer);

    *a = __builtin_return_address(0);

    // perform bitwise AND operation on the value stored in var_10h
    // and check if the result is equal to 0xb0000000
    if ((*a & 0xb0000000) == 0xb0000000) {
        // print the value stored in var_10h using the format string "%p"
        printf("(%p)\n", *a);

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
