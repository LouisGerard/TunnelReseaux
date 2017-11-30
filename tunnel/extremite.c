#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>

void ext_out(unsigned port) {
    int sock = socket(AF_INET6, SOCK_STREAM, 0);
    if(sock < 0) {
        perror("socket()");
        exit(errno);
    }

    struct sockaddr_in6 sin;

    sin.sin6_addr = in6addr_any;
    sin.sin6_family = AF_INET6;
    sin.sin6_port = htons((uint16_t) port);

    if(bind (sock, (struct sockaddr *) &sin, sizeof sin) < 0) {
        perror("bind()");
        exit(errno);
    }

    if(listen(sock, 5) < 0) {
        perror("listen()");
        exit(errno);
    }

    struct sockaddr_in csin = { 0 };
    socklen_t sinsize = sizeof(struct sockaddr_in);
    int csock = accept(sock, (struct sockaddr *) &csin, &sinsize);

    if(csock < 0) {
        perror("accept()");
        exit(errno);
    }

    char buff[256];
    while (1) {
        ssize_t lu = recv(csock, &buff, 255, 0);
        if (lu > 0) {
            buff[lu] = '\0';
            printf("read (%d) : %s\n", (int) lu, buff);
        } else if (lu < 0) {
            perror("recv()");
            exit(errno);
        }
    }

    printf("\n");

    close(csock);
    close(sock);
}

void ext_in(unsigned port, char *address, int tun0) {
    int sock = socket(AF_INET6, SOCK_STREAM, 0);
    if(sock < 0) {
        perror("socket()");
        exit(errno);
    }

    struct sockaddr_in6 sin;

    inet_pton(AF_INET6, address, &sin.sin6_addr);
    sin.sin6_family = AF_INET6;
    sin.sin6_port = htons((uint16_t) port);

    if(connect(sock,(struct sockaddr *) &sin, sizeof(struct sockaddr_in6)) < 0)
    {
        perror("connect()");
        exit(errno);
    }

    char buff[256];
    while (1) {
        ssize_t lu = read(tun0, &buff, 255);
        if (lu > 0) {
            buff[lu] = '\0';
            printf("read (%d) : %s\n", (int) lu, buff);
            send(sock, &buff, (size_t) lu, 0);
        } else if (lu < 0) {
            perror("recv()");
            exit(errno);
        }
    }

    close(sock);

}