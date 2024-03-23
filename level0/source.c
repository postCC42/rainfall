#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#define _GNU_SOURCE

int main(int argc, char *argv[]) {
    int nb;
    char *cmd[2];
    gid_t gid;
    uid_t uid;

    nb = atoi(argv[1]);
    if (nb != 0x1a7) {
        fwrite("No !\n", 1, 5, stdout);
        return 0;
    }

    cmd[0] = strdup("/bin/sh");
    cmd[1] = 0;
    gid = getegid();
    uid = geteuid();
    setresgid(gid, gid, gid);
    setresuid(uid, uid, uid);
    execv(cmd[0], cmd);

    return 0;
}