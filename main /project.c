#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define READ_END 0
#define WRITE_END 1

int main() {
    int fd[2];
    pid_t pid;
    char input;
    char buffer;

    // Create a pipe
    if (pipe(fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Fork a child process
    pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid > 0) { // Parent process
        close(fd[READ_END]); // Close unused read end

        while (1) {
            printf("$ ");
            scanf(" %c", &input);

            write(fd[WRITE_END], &input, sizeof(input));

            if (input == 'T') {
                break; // Terminate on 'T' input
            }
        }

        close(fd[WRITE_END]); // Close write end
        wait(NULL); // Wait for child to finish
    } 
        else { // Child process
        close(fd[WRITE_END]); // Close unused write end

        while (1) {
            read(fd[READ_END], &buffer, sizeof(buffer));
            printf("Received: %c\n", buffer);

            if (buffer == 'T') {
                break; // Terminate on 'T' input
            }
        }

        close(fd[READ_END]); // Close read end
    }

    return 0;
}
