#include <net/if.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>

#include <unistd.h>
#include <fcntl.h>
#include <linux/if_tun.h>

#include "extremite.h"

int tun_alloc(char *dev) {
    struct ifreq ifr;
    int fd, err;

    if ((fd = open("/dev/net/tun", O_RDWR)) < 0) {
        perror("création tun");
        exit(EXIT_FAILURE);
    }

    memset(&ifr, 0, sizeof(ifr));

    ifr.ifr_flags = IFF_TUN;
    if (*dev)
        strncpy(ifr.ifr_name, dev, IFNAMSIZ);

    if ((err = ioctl(fd, TUNSETIFF, (void *) &ifr)) < 0) {
        close(fd);
        return err;
    }
    strcpy(dev, ifr.ifr_name);
    return fd;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage : %s address\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    char *dev = malloc(256);
    int tun0 = tun_alloc(dev);
    system("./configure-tun.sh");
    printf("Interface installed !\n");

    if (fork() == 0)
        ext_out(123, tun0);
    else
        ext_in(123, argv[1], tun0);

    close(tun0);
    return 0;
}