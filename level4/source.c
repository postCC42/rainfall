#include <stdio.h>
#include <stdlib.h>

int m;

void p(char *input) {
    printf("%s", input);
}

void n(void) {
    char buffer[520];

    fgets(buffer, sizeof(buffer), stdin);
    printInput(buffer);

    if (m == 16930116) {
        system("/bin/cat /home/user/level5/.pass");
    }
}

int main(void) {
    n();
    return 0;
}
