#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <errno.h>

#define SHM_KEY 1234
#define SEM_MUTEX_KEY 5678
#define SEM_COUNT_KEY 6789
#define SEM_SPACES_KEY 7890
#define BUFFER_SIZE 5
#define MSG_LEN 10

struct shared_memory {
    char buffer[BUFFER_SIZE][MSG_LEN + 1];
    int in;
    int out;
    int count;
};

void gen_text(char text[MSG_LEN + 1]) {
    for (int i = 0; i < MSG_LEN; i++) {
        text[i] = 'a' + (rand() % 26);
    }
    text[MSG_LEN] = '\0';
}

void sem_op(int semid, int op) {
    struct sembuf sb = {0, op, 0};
    if (semop(semid, &sb, 1) == -1) {
        perror("semop");
        exit(1);
    }
}

void write_to_shared_memory() {
    int shmid, mutex, count, spaces;
    struct shared_memory *shared_mem;

    shmid = shmget(SHM_KEY, sizeof(struct shared_memory), 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }

    shared_mem = shmat(shmid, NULL, 0);
    if (shared_mem == (void *) -1) {
        perror("shmat");
        exit(1);
    }

    mutex = semget(SEM_MUTEX_KEY, 1, 0666);
    count = semget(SEM_COUNT_KEY, 1, 0666);
    spaces = semget(SEM_SPACES_KEY, 1, 0666);
    if (mutex == -1 || count == -1 || spaces == -1) {
        perror("semget");
        exit(1);
    }

    char message[MSG_LEN + 1];

    while (1) {
        gen_text(message);

        sem_op(spaces, -1);

        sem_op(mutex, -1);

        printf("User (PID %d): message to print: %s\n", getpid(), message);
        strncpy(shared_mem->buffer[shared_mem->in], message, MSG_LEN + 1);
        shared_mem->in = (shared_mem->in + 1) % BUFFER_SIZE;
        shared_mem->count++;

        sem_op(mutex, 1);

        sem_op(count, 1);

        srand(time(NULL) ^ (getpid() << 16));
        sleep(rand() % 4 + 1);
    }

    shmdt(shared_mem);
}

int main() {

    srand(time(NULL) ^ (getpid() << 16));
    write_to_shared_memory();
    return 0;
}


