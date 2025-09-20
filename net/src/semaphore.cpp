#include <cstdlib>
#include <iostream>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/wait.h>

/*
struct semid_ds {
    struct ipc_perm      sem_perm;         // 0    permissions
    __kernel_old_time_t  sem_otime;        // 32   last semop() time
    __kernel_old_time_t  sem_ctime;        // 40   last change time
    struct sem          *sem_base;         // 48   pointer to array of semaphores
    struct sem_queue    *sem_pending;      // 56   head of pending operations
    struct sem_queue   **sem_pending_last; // 64   tail pointer of pending list
    struct sem_undo     *undo;             // 72   undo structures (for SEM_UNDO)
    unsigned short       sem_nsems;        // 80   number of semaphores in set
}; */

union semun {
    int val;               /* Value for SETVAL */
    struct semid_ds *buf;  /* Buffer for IPC_STAT, IPC_SET */
    unsigned short *array; /* Array for GETALL, SETALL */
    struct seminfo *__buf; /* Buffer for IPC_INFO (Linux-specific) */
};

void P(int sem_id)
{
    struct sembuf op;
    op.sem_num = 0;
    op.sem_op = -1;
    op.sem_flg = 0;
    if (semop(sem_id, &op, 1) == -1)
    {
        std::cerr << "semop P failed\n";
        exit(1);
    }
}

void V(int sem_id)
{
    struct sembuf op;
    op.sem_num = 0;
    op.sem_op = 1;
    op.sem_flg = 0;
    if (semop(sem_id, &op, 1) == -1)
    {
        std::cerr << "semop V failed\n";
        exit(1);
    }
}

int main()
{
    /* System V semaphore system calls:
     *
     *   int semget(key_t key, int num_sems, int sem_flags);
     *
     * - Use ftok() to generate a key that uniquely identifies an IPC object.
     * - num_sems : Number of semaphores in the set.
     * - sem_flags: Flags controlling creation and access (similar to file open modes).
     */
    key_t key = ftok("/tmp", 65);
    if (key == -1)
    {
        std::cerr << "ftok failed\n";
        exit(1);
    }

    int sem_id = semget(key, 1, IPC_CREAT | 666);
    if (sem_id == -1)
    {
        std::cerr << "semget failed\n";
        exit(1);
    }

    union semun arg;
    arg.val = 1;
    if (semctl(sem_id, 0, SETVAL, arg) == -1)
    {
        std::cerr << "semctl SETVAL failed\n";
        exit(1);
    }

    pid_t pid = fork();
    if (pid == 0)
    {
        for (int i = 0; i < 3; ++i)
        {
            P(sem_id);
            std::cout << "Child: " << i + 1 << "\n";
            V(sem_id);
            sleep(1);
        }
        exit(0);
    }
    else
    {
        for (int i = 0; i < 3; ++i)
        {
            P(sem_id);
            std::cout << "Parent: " << i + 1 << "\n";
            V(sem_id);
            sleep(1);
        }
        wait(nullptr);
        if (semctl(sem_id, 0, IPC_RMID) == -1)
        {
            std::cerr << "semctl IPC_RMID failed\n";
            exit(1);
        }
    }
}