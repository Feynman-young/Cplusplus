#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>

#define BUFFER_LEN 1024
#define M_NUMS 4

void pipe_example() {
    int pipe_fd[2];
    pid_t pid;
    char buffer[BUFFER_LEN];
    memset(buffer, 0, sizeof(buffer));

    if (pipe(pipe_fd) == -1) {
        perror("pipe() error");
    }

    pid = fork();

    if (pid == 0) {
        close(pipe_fd[1]);
        while (read(pipe_fd[0], buffer, sizeof(buffer)) > 0) {
            printf("child => : %s\n", buffer);
            memset(buffer, 0, sizeof(buffer));
        }
        close(pipe_fd[0]);
        exit(0);
    } else {
        close(pipe_fd[0]);
        const char* msgs[] = {"hello ", "from ", "parent ", "process"};
        for (int i = 0; i < M_NUMS; i++) {
            write(pipe_fd[1], msgs[i], strlen(msgs[i]));
        }
        close(pipe_fd[1]);
        wait(NULL);
    }
}

void socketpair_example() {
    int pipe_fd[2];
    pid_t pid;
    char buffer[BUFFER_LEN];

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, pipe_fd) == -1) {
        perror("socketpair() error");
    }

    pid = fork();

    if (pid == 0) {
        close(pipe_fd[0]);
        while (1) {
            memset(buffer, 0 , sizeof(buffer));
            int n = recv(pipe_fd[1], buffer, sizeof(buffer), 0);
            if (n < 0) {
                perror("child process: recv() error");
                break;
            }
            if (n == 0) {
                break;
            }
            printf("c => : %s\n", buffer);
            char* respone = "ack";
            send(pipe_fd[1], respone, strlen(respone), 0);
        }
        close(pipe_fd[1]);
        exit(0);
    } else {
        close(pipe_fd[1]);
        const char* msgs[] = {"hello ", "from ", "parent ", "process"};
        for (int i = 0; i < M_NUMS; i++) {
            send(pipe_fd[0], msgs[i], strlen(msgs[i]), 0);
            memset(buffer, 0, sizeof(buffer));
            if (recv(pipe_fd[0], buffer, sizeof(buffer), 0) < 0) {
                perror("parent process: recv() error");
                break;
            }
            printf("p => : %s\n", buffer);
        }
        close(pipe_fd[0]);
        wait(NULL);
    }
}

int main() {
    pipe_example();
    socketpair_example();
    return 0;
}
