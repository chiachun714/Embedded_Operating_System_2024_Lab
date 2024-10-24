#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <name>\n", argv[0]);
        return -1;
    }

    char *message = argv[1];  // Use the command line argument as the message
    size_t len = strlen(message);
    int i = 0;
    int fd = open("/dev/mydev", O_WRONLY);
    
    if (fd < 0) {
        perror("Failed to open the device...");
        return -1;
    }

    while (1) {
        write(fd, message, len);   // Write buffer to device
        sleep(1);  // Wait for 1 second
    }

    close(fd);
    return 0;
}
