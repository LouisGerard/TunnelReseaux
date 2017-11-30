#include <net/if.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>

#include <unistd.h>
#include <fcntl.h>
#include <linux/if_tun.h>

#include "extremite.h"

#define BUFFER_SIZE_MAX 256

int tun_alloc(char *dev) {
    struct ifreq ifr;
    int fd, err;

    if ((fd = open("/dev/net/tun", O_RDWR)) < 0) {
        perror("création tun");
        exit(1);
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

void tun_copy (int src, int dst)
{
    size_t size;
    char* buffer = malloc (sizeof (char) * BUFFER_SIZE_MAX);

    while (1)
    {
        size = read (src, buffer, BUFFER_SIZE_MAX);
        write (dst, buffer, size);
        // todo if ... break
    }

    free (buffer);
}

int main() {
    char *dev = malloc(256);
    int tun0 = tun_alloc(dev);
    printf("Interface created !\n");
    ext_out();
//    while (1);
    return 0;
}