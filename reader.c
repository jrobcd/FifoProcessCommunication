#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>

const char *fifo = "/tmp/my_fifo";

void handle_sigterm(int sig) {
    FILE *log_file = fopen("log.txt", "a");
    if (log_file == NULL) {
        perror("fopen log_file");
        exit(EXIT_FAILURE);
    }
    fprintf(log_file, "Writer killed, I'm leaving\n");
    fclose(log_file);
    printf("Writer killed, I'm leaving\n");
    exit(0);
}

int main() {
    char buffer[1024];
    int fifo_fd;

    // Guardar el PID del reader en un archivo
    FILE *pid_file = fopen("reader_pid.txt", "w");
    if (pid_file != NULL) {
        fprintf(pid_file, "%d\n", getpid());
        fclose(pid_file);
    } else {
        perror("fopen pid_file");
    }

    signal(SIGTERM, handle_sigterm);

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

    FILE *log_file = fopen("log.txt", "a"); 
    FILE *signals_file = fopen("signals.txt", "a");
    if (log_file == NULL) {
        perror("fopen log_file");
        exit(EXIT_FAILURE);
    }
    if (signals_file == NULL) {
        perror("fopen signals_file");
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
            } else if (strncmp(buffer, "SIGN:", 5) == 0) {
                fprintf(signals_file, "%s\n", buffer + 5);
                fflush(signals_file);
                printf("%s -> signals.txt\n", buffer + 5);
            } else if (strncmp(buffer, "PID:", 4) == 0) {
                // Procesar mensaje de PID
            } else {
                printf("Unknown message: %s\n", buffer);
            }
        } else if (bytes_read == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                continue;
            } else {
                perror("read");
                break;
            }
        }
    }

    close(fifo_fd);
    fclose(log_file);
    fclose(signals_file);
    return 0;
}
