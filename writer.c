#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>

const char *fifo = "/tmp/my_fifo";
int fifo_fd;

void signal_handler(int sig) {
    if (sig == SIGUSR1) {
        printf("Sending signal: SIGN:1\n");
        if (write(fifo_fd, "SIGN:1\n", 7) == -1) {
            perror("write");
        }
    } else if (sig == SIGUSR2) {
        printf("Sending signal: SIGN:2\n");
        if (write(fifo_fd, "SIGN:2\n", 7) == -1) {
            perror("write");
        }
    }
}

int main() {
    char buffer[1024];
    
    printf("Writer PID: %d\n", getpid());

    fifo_fd = open(fifo, O_WRONLY);
    if (fifo_fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    printf("opening fifo...\n");

    signal(SIGUSR1, signal_handler);
    signal(SIGUSR2, signal_handler);

    while (1) {
        printf("Enter text: ");
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            size_t len = strlen(buffer);
            if (buffer[len - 1] == '\n') {
                buffer[len - 1] = '\0';
            }
            char message[1050];
            snprintf(message, sizeof(message), "%s\n", buffer);
            printf("Sending data: %s\n", message);
            if (write(fifo_fd, message, strlen(message)) == -1) {
                perror("write");
            }
        }
    }

    close(fifo_fd);
    return 0;
}
