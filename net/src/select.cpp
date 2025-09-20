#include <arpa/inet.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define PORT 54321
#define MAX_CONN 5
#define BUF_SIZE 1024
#define S_ADDR_ "127.0.0.1"
#define THREAD_N 10

void handle_client(int client_fd)
{
    char buf[BUF_SIZE];
    int len = read(client_fd, buf, BUF_SIZE);
    if (len < 0)
    {
        perror("read() error");
        exit(1);
    }
    else if (len >= 0)
    {
        printf("read() => %s\n", buf);
    }
    close(client_fd);
}

void server()
{
    struct sockaddr_in s_addr;
    s_addr.sin_family = AF_INET;
    s_addr.sin_addr.s_addr = INADDR_ANY;
    s_addr.sin_port = htons(PORT);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        perror("socket() error");
        exit(1);
    }

    if (bind(server_fd, (struct sockaddr *)&s_addr, sizeof(s_addr)) < 0)
    {
        perror("bind() error");
        exit(1);
    }

    if (listen(server_fd, MAX_CONN) < 0)
    {
        perror("listen() error");
        exit(1);
    }

    fd_set rfds;
    int client_fds[MAX_CONN] = {0};
    int client_fd;

    while (1)
    {

        FD_ZERO(&rfds);
        FD_SET(server_fd, &rfds);

        int max_fd = server_fd;
        for (int i = 0; i < MAX_CONN; ++i)
        {
            client_fd = client_fds[i];
            if (client_fd > 0)
            {
                FD_SET(client_fd, &rfds);
                if (client_fd > max_fd)
                {
                    max_fd = client_fd;
                }
            }
        }

        if (select(max_fd + 1, &rfds, NULL, NULL, NULL) < 0)
        {
            perror("select() error");
            exit(1);
        }

        if (FD_ISSET(server_fd, &rfds))
        {
            struct sockaddr_in c_addr;
            socklen_t c_addrlen = sizeof(c_addr);

            client_fd = accept(server_fd, (struct sockaddr *)&c_addr, &c_addrlen);
            if (client_fd < 0)
            {
                perror("accept() error");
                continue;
            }

            char remote[INET_ADDRSTRLEN];
            printf("connect with fd %s:%d\n", inet_ntop(AF_INET, &c_addr.sin_addr, remote, INET_ADDRSTRLEN),
                   ntohs(c_addr.sin_port));

            for (int i = 0; i < MAX_CONN; i++)
            {
                if (client_fds[i] == 0)
                {
                    client_fds[i] = client_fd;
                    break;
                }
            }
        }

        for (int i = 0; i < MAX_CONN; i++)
        {
            client_fd = client_fds[i];
            if (FD_ISSET(client_fd, &rfds))
            {
                handle_client(client_fd);
                client_fds[i] = 0;
                FD_CLR(client_fd, &rfds);
            }
        }
    }
    close(server_fd);
}

void *client(void *arg)
{

    struct sockaddr_in s_addr;
    s_addr.sin_family = AF_INET;
    inet_pton(AF_INET, S_ADDR_, &s_addr.sin_addr);
    s_addr.sin_port = htons(PORT);

    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0)
    {
        perror("socket() error");
        exit(1);
    }

    if ((connect(client_fd, (struct sockaddr *)&s_addr, sizeof(s_addr))) < 0)
    {
        close(client_fd);
        perror("connect() error");
        exit(1);
    }

    char buf[BUF_SIZE] = "hello";

    write(client_fd, buf, strlen(buf));
    close(client_fd);
    return NULL;
}

void conn()
{
    pid_t pid;
    pid = fork();

    if (pid == 0)
    {
        pthread_t threads[THREAD_N];
        for (int i = 0; i < THREAD_N; i++)
        {
            if (pthread_create(&threads[i], NULL, client, NULL) < 0)
            {
                perror("pthread_create() error");
                exit(1);
            }
        }
        for (int i = 0; i < THREAD_N; i++)
        {
            pthread_join(threads[i], NULL);
        }
        exit(0);
    }
    else
    {
        server();
        wait(NULL);
    }
}

int main()
{
    conn();
    return 0;
}