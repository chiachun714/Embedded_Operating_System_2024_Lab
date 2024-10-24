#include <stdio.h>      // fprintf(), perror()
#include <stdlib.h>     // exit()
#include <string.h>     // memset()
#include <signal.h>    // signal()
#include <fcntl.h>     // open()
#include <unistd.h>    // read(), write(), close()

#include <sys/socket.h> // socket(), connect()
#include <netinet/in.h> // struct sockaddr_in
#include <arpa/inet.h>  // htons()

int connfd, fd;

void sigint_handler(int signo) {
    close(fd);
}

int main(int argc, char *argv[]) {
    if(argc != 2) {
        fprintf(stderr, "Usage: ./reader <device_path>");
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, sigint_handler);

    if((fd = open(argv[1], O_RDWR)) < 0) {
        perror(argv[1]);
        exit(EXIT_FAILURE);
    }

    int ret;
    char buf[16] = {};

    while (1) {
        if((ret = read(fd, buf, sizeof(buf))) == -1) {
            perror("read()");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < 16; ++i) {
            printf("%c ", buf[i]);
        }
        printf("\n");

        sleep(1);
    }

    close(fd);
    return 0;
}
