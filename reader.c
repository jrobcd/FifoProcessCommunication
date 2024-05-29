#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

const char *fifo = "/tmp/my_fifo";

int main() {
    char buffer[1024];
    int fifo_fd;

    while (access(fifo, F_OK) == -1) {
        if (errno == ENOENT) {
            printf("Error: FIFO does not exist. Make sure the writer is running.\n");
            exit(EXIT_FAILURE);
        }
    }

    fifo_fd = open(fifo, O_RDONLY);
    if (fifo_fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    printf("opening fifo...\n");

    FILE *log_file = fopen("log.txt", "w"); 
    FILE *signals_file = fopen("signals.txt", "a");
    if (log_file == NULL || signals_file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

while (1) {
    ssize_t bytes_read = read(fifo_fd, buffer, sizeof(buffer) - 1);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        if (strncmp(buffer, "DATA:", 5) == 0) {
            fprintf(log_file, "%s\n", buffer + 5);
            fflush(log_file);
            printf("%s -> log.txt\n", buffer + 5);
        } else if (strncmp(buffer, "SIGNAL:", 7) == 0) {
            fprintf(signals_file, "%s\n", buffer + 7);
            fflush(signals_file);
            printf("%s -> signals.txt\n", buffer + 7);
        } else {
            printf("Unknown message: %s\n", buffer);
        }
    } else if (bytes_read == -1) {
        perror("read");
        break;
    }
}

    close(fifo_fd);
    fclose(log_file);
    fclose(signals_file);
    return 0;
}
