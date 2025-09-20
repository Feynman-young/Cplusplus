// Basic linux network programming api

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// Big endian and little endian
void byteorder()
{
    uint32_t temp = 1;
    printf("Byte Order: ");
    *(char *)&temp ? printf("Little endian\n") : printf("Big endian\n");
}

union packet {
    unsigned int value;
    uint8_t union_bytes[sizeof(unsigned int)];
};

void test_hton_ntohl()
{
    union packet data;
    data.value = 0x12345678;

    printf("Little endian: ");
    for (int i = 0; i < sizeof(unsigned int); ++i)
    {
        printf("0x%02x ", data.union_bytes[i]);
    }
    printf("\n");

    data.value = htonl(data.value);

    printf("Big endian:    ");
    for (int i = 0; i < sizeof(unsigned int); ++i)
    {
        printf("0x%02x ", data.union_bytes[i]);
    }
    printf("\n");

    data.value = ntohl(data.value);

    printf("Little endian: ");
    for (int i = 0; i < sizeof(unsigned int); ++i)
    {
        printf("0x%02x ", data.union_bytes[i]);
    }
    printf("\n");
}

#define PORT_ 54321
#define M_CONN_ 5
#define S_ADDR_ "127.0.0.1"

void server()
{

    struct sockaddr_in s_addr;
    s_addr.sin_family = AF_INET;
    s_addr.sin_addr.s_addr = INADDR_ANY;
    s_addr.sin_port = htons(PORT_);

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

    if (listen(server_fd, M_CONN_) < 0)
    {
        perror("listen() error");
        exit(1);
    }

    struct sockaddr_in c_addr;
    socklen_t c_addrlen = sizeof(c_addr);

    int client_fd = accept(server_fd, (struct sockaddr *)&c_addr, &c_addrlen);
    if (client_fd < 0)
    {
        perror("accept() error");
        exit(1);
    }

    char remote[INET_ADDRSTRLEN];
    printf("connect with %s:%d\n", inet_ntop(AF_INET, &c_addr.sin_addr, remote, INET_ADDRSTRLEN),
           ntohs(c_addr.sin_port));
    close(client_fd);
    close(server_fd);
}

void client()
{

    struct sockaddr_in s_addr;
    s_addr.sin_family = AF_INET;
    inet_pton(AF_INET, S_ADDR_, &s_addr.sin_addr);
    s_addr.sin_port = htons(PORT_);

    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0)
    {
        perror("socket() error");
        exit(1);
    }

    if (connect(client_fd, (struct sockaddr *)&s_addr, sizeof(s_addr)) < 0)
    {
        perror("connect() error");
        exit(1);
    }

    close(client_fd);
}

void conn()
{
    pid_t pid;
    pid = fork();

    if (pid == 0)
    {
        sleep(1);
        client();
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
    byteorder();
    test_hton_ntohl();
    conn();
    return 0;
}