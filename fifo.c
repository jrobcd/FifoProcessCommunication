#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

int main() {
    const char *fifo = "/tmp/my_fifo";
    if (mkfifo(fifo, 0666) == -1) {
        perror("mkfifo");
        return 1;
    }
    return 0;
}
