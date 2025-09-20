#include <cstdint>
#include <cstring>
#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

int main()
{
    key_t key = ftok("/tmp", 65);
    if (key == -1)
    {
        std::cerr << "ftok failed\n";
        exit(1);
    }

    int shm_id = shmget(key, 1024, IPC_CREAT | 0666);
    if (shm_id == -1)
    {
        std::cerr << "shmget failed\n";
        exit(1);
    }

    pid_t pid = fork();
    if (pid == 0)
    {
        char *shm_addr = (char *)shmat(shm_id, nullptr, 0);
        if (shm_addr == (char *)-1)
        {
            std::cerr << "shmat failed\n";
            exit(1);
        }
        sleep(1);
        std::cout << "Child read: " << shm_addr << std::endl;
        shmdt(shm_addr);
        exit(0);
    }
    else
    {
        char *shm_addr = (char *)shmat(shm_id, nullptr, 0);
        if (shm_addr == (char *)-1)
        {
            std::cerr << "shmat failed\n";
            exit(1);
        }

        uint8_t msg[] = "Hello from parent.";
        memcpy(shm_addr, msg, sizeof(msg));
        shmdt(shm_addr);
        wait(nullptr);

        if (shmctl(shm_id, IPC_RMID, nullptr) == -1)
        {
            std::cerr << "shmctl IPC_RMID failed\n";
            exit(1);
        }
    }
}