#include <sys/socket.h>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <netinet/in.h>

int ext_out() {
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
}
