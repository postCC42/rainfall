#include <stdio.h>
#include <stdlib.h>

int m;

void v(void) {
    char buffer[520];

    fgets(buffer, 512, 0);
    printf(buffer);

    if (m == 64) {
        fwrite("Wait what?!\n", 1, 12, 1);
        system("/bin/sh");
    }
    return 0;
}

void main(void) {
    v();
    return 0;
}