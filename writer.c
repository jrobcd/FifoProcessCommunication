#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

const char *fifo = "/tmp/my_fifo";
int fifo_fd;
pid_t reader_pid = -1;

void send_pid() {
    char pid_message[100];
    snprintf(pid_message, sizeof(pid_message), "PID:%d\n", getpid());
    write(fifo_fd, pid_message, strlen(pid_message));
}

void signal_handler(int sig) {
    if (sig == SIGUSR1 || sig == SIGUSR2) {
        char message[100];
        snprintf(message, sizeof(message), "SIGN:%d\n", sig == SIGUSR1 ? 1 : 2);
        write(fifo_fd, message, strlen(message));
    }
}

void sigint_handler(int sig) {
    // Send a termination signal to reader
    if (reader_pid > 0) {
        kill(reader_pid, SIGTERM);
    }
    exit(0);
}

int main() {
    char buffer[1024];

    printf("Writer PID: %d\n", getpid());

    signal(SIGINT, sigint_handler);

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

    send_pid();

    // Esperar la lectura del PID del reader desde un archivo compartido
    FILE *pid_file = fopen("reader_pid.txt", "r");
    if (pid_file != NULL) {
        fscanf(pid_file, "%d", &reader_pid);
        fclose(pid_file);
    } else {
        perror("fopen pid_file");
    }

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
        } else {
            if (errno == EINTR) {
                printf("Input interrupted by signal. Continuing...\n");
                clearerr(stdin);
                continue;
            } else if (feof(stdin)) {
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
