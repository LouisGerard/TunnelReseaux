#include <net/if.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>

#include <unistd.h>
#include <fcntl.h>
#include <linux/if_tun.h>
#include <errno.h>
#include <ctype.h>

#include "extremite.h"

char *rtrim(char *s)
{
    char* back = s + strlen(s);
    while(isspace(*--back));
    *(back+1) = '\0';
    return s;
}

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
        fprintf(stderr, "Usage : %s file.conf\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // PARSE CONF
    FILE *conf = fopen(argv[1], "r");
    if (conf == NULL) {
        perror("fopen()");
        exit(errno);
    }

    char *line;
    char *tun_name = "tun0";
    char *in_ip = "172.16.2.131";
    unsigned in_port = 123;
    unsigned out_port = 123;
    size_t len = 0;

    while (getline(&line, &len, conf) != -1) {
        char* loc;
        if ((loc = strstr(line, "tun=")) != NULL) {
            loc += 4;
            rtrim(loc);
            tun_name =  malloc(len-4);
            strcpy(tun_name, loc);
        } else if ((loc = strstr(line, "inip=")) != NULL) {
            loc += 5;
            rtrim(loc);
            in_ip =  malloc(len-4);
            strcpy(in_ip, loc);
        } else if ((loc = strstr(line, "inport=")) != NULL) {
            loc += 7;
            rtrim(loc);
            in_port = (unsigned int) atoi(loc);
        } else if ((loc = strstr(line, "outport=")) != NULL) {
            loc += 8;
            rtrim(loc);
            out_port = (unsigned int) atoi(loc);
        }
    }

    fclose(conf);
    free(line);

    int tun0 = tun_alloc(tun_name);
    char *cmd = malloc(19 + strlen(tun_name));
    strcpy(cmd, "./configure-tun.sh ");
    strcat(cmd, tun_name);
    system(cmd);
    free(cmd);
    free(tun_name);
    printf("Interface installed !\n");

    if (fork() == 0)
        ext_out(out_port, tun0);
    else
        ext_in(in_port, in_ip, tun0);

    close(tun0);
    return 0;
}