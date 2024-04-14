#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char **argv) {
    int nb = atoi(argv[1]);
    char str[40];

    if (nb > 9) {
        return 1;
    }

    memcpy(str, argv[2], nb * 4);

    if (0x574f4c46 == nb) {
        execl("/bin/sh", "/bin/sh", 0);
    }

    return 0;
}