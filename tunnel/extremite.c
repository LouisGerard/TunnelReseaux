#include <sys/socket.h>
#include <cerrno>
#include <netinet/in.h>

#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>

void ext_out() {
    int sock = socket(AF_INET6, SOCK_STREAM, 0);
    if(sock < 0) {
        perror("socket()");
        exit(errno);
    }

    struct sockaddr_in sin = { 0 };

    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_family = AF_INET6;
    sin.sin_port = htons(123);

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
    int csock = accept(sock, (struct sockaddr *)&csin, &sinsize);

    if(csock < 0) {
        perror("accept()");
        exit(errno);
    }

    char buff[256];
    while (recv(sock, &buff, 256, 0) > 0) {
        printf("%s", buff);
    }

    close(sock);
}
