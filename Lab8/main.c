#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <errno.h>
#include <string.h> 

#define SHM_KEY 1234
#define SEM_MUTEX_KEY 5678
#define SEM_COUNT_KEY 6789
#define SEM_SPACES_KEY 7890
#define BUFFER_SIZE 5

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

struct shared_memory {
    char buffer[BUFFER_SIZE][11];
    int in;
    int out;
    int count;
};

void initialize_ipc() {
    int shmid, mutex, count, spaces;
    union semun arg;

    shmid = shmget(SHM_KEY, sizeof(struct shared_memory), 0666 | IPC_CREAT | IPC_EXCL);
    if (shmid == -1) {
        if (errno == EEXIST) {
            shmid = shmget(SHM_KEY, sizeof(struct shared_memory), 0666);
            if (shmid == -1) {
                perror("shmget");
                exit(1);
            }
        } else {
            perror("shmget");
            exit(1);
        }
    } else {
        struct shared_memory *shared_mem = shmat(shmid, NULL, 0);
        if (shared_mem == (void *) -1) {
            perror("shmat");
            exit(1);
        }
        memset(shared_mem, 0, sizeof(struct shared_memory));
        shmdt(shared_mem);
    }

    mutex = semget(SEM_MUTEX_KEY, 1, 0666 | IPC_CREAT | IPC_EXCL);
    count = semget(SEM_COUNT_KEY, 1, 0666 | IPC_CREAT | IPC_EXCL);
    spaces = semget(SEM_SPACES_KEY, 1, 0666 | IPC_CREAT | IPC_EXCL);

    if (mutex == -1 || count == -1 || spaces == -1) {
        if (errno == EEXIST) {
            mutex = semget(SEM_MUTEX_KEY, 1, 0666);
            count = semget(SEM_COUNT_KEY, 1, 0666);
            spaces = semget(SEM_SPACES_KEY, 1, 0666);
            if (mutex == -1 || count == -1 || spaces == -1) {
                perror("semget");
                exit(1);
            }
        } else {
            perror("semget");
            exit(1);
        }
    } else {
        arg.val = 1;
        if (semctl(mutex, 0, SETVAL, arg) == -1) {
            perror("semctl mutex");
            exit(1);
        }
        arg.val = 0;
        if (semctl(count, 0, SETVAL, arg) == -1) {
            perror("semctl count");
            exit(1);
        }
        arg.val = BUFFER_SIZE;
        if (semctl(spaces, 0, SETVAL, arg) == -1) {
            perror("semctl spaces");
            exit(1);
        }
    }
}

void start_process(const char *program_name) {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        execl(program_name, program_name, NULL);
        perror("execl");
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <number_of_printers> <number_of_users>\n", argv[0]);
        exit(1);
    }

    int num_printers = atoi(argv[1]);
    int num_users = atoi(argv[2]);

    if (num_printers <= 0 || num_users <= 0) {
        fprintf(stderr, "Both the number of printers and users must be greater than zero.\n");
        exit(1);
    }

    initialize_ipc();

    for (int i = 0; i < num_printers; i++) {
        start_process("./printer");
    }

    for (int i = 0; i < num_users; i++) {
        start_process("./user");
    }

    for (int i = 0; i < num_printers + num_users; i++) {
        wait(NULL);
    }

    return 0;
}




