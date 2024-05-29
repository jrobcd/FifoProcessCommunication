#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

const char *fifo = "/tmp/my_fifo";
int fifo_fd;
volatile int sigusr1_count = 0;
volatile int sigusr2_count = 0;

void signal_handler(int sig) {
    if (sig == SIGUSR1) {
        sigusr1_count++;
    } else if (sig == SIGUSR2) {
        sigusr2_count++;
    }
}

int main() {
    char buffer[1024];
    
    printf("Writer PID: %d\n", getpid());

    while (access(fifo, F_OK) == -1) {
        if (errno == ENOENT) {
            printf("Error: FIFO does not exist. Make sure the reader is running.\n");
            exit(EXIT_FAILURE);
        }
    }

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
        snprintf(message, sizeof(message), "DATA:%s\n", buffer);
        printf("Sending data: %s\n", message);
        if (write(fifo_fd, message, strlen(message)) == -1) {
            perror("write");
        }
        else if (strncmp(buffer, "kill SIGUSR1", 12) == 0) {
            printf("Sending signal: SIGUSR1\n");
            if (write(fifo_fd, "SIGNAL:SIGUSR1\n", 15) == -1) {
                perror("write");
            }
        }
    } else {
        if (feof(stdin)) {
            printf("EOF detected. Exiting...\n");
            break;
        } else {
            perror("fgets");
            exit(EXIT_FAILURE);
        }
    }
}

    close(fifo_fd);
    return 0;
}
