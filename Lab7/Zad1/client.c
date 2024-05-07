#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <sys/stat.h>

struct msgbuf {
  long mtype;
  char mtext[100];
};

//SYTEM V

int main() {

  key_t key;
  char selfPath[PATH_MAX];
  ssize_t len = readlink("/proc/self/exe", selfPath, sizeof(selfPath) - 1);
  if (len != -1) {
    selfPath[len] = '\0';

    key = ftok(selfPath, getpid());
    if (key == -1) {
      perror("ftok failed\n");
      return EXIT_FAILURE;
    } else {
      printf("my key: %d\n", key);
    }

  } else {
    printf("cos poszlo nie tak\n");
    return EXIT_FAILURE;
  }

  key_t server_key = ftok("/bin/ls", 's');
  if (server_key == -1) {
    printf("ftok failed\n");
    return EXIT_FAILURE;
  }

  int server_queue = msgget(server_key, 0666);
  if (server_queue == -1) {
    printf("msgget failed\n");
    return EXIT_FAILURE;
  }

  int client_queue = msgget(key, 0666 | IPC_CREAT);
  if (client_queue == -1) {
    printf("failed to open client's queue");
    return EXIT_FAILURE;
  }

  pid_t pid = fork();
  if (pid == -1) {
    perror("Fork failed");
    return EXIT_FAILURE;
  }

  if (pid == 0) {
    // potomny, odbiera wiadomości
    struct msgbuf msg;
    while (1) {

      msgrcv(client_queue, &msg, sizeof(msg.mtext), 0, 0);
      printf("received: %s\n", msg.mtext);
    }

  } else {
    // macierzysty, wysyła wiadomości
    struct msgbuf init_msg;
    init_msg.mtype = 2; // type 2 = INIT
    char buffor[100];
    sprintf(buffor, "%d", key);
    strcpy(init_msg.mtext, buffor);

    if (msgsnd(server_queue, &init_msg, sizeof(init_msg.mtext), 0) == -1) {
      printf("failed to send INIT message\n");
    } else {
      printf("INIT message sent to server\n");
    }

    int my_id;
    if (msgrcv(client_queue, &init_msg, sizeof(init_msg.mtext), 2, 0) == -1) {
      perror("Failed to receive ID");
      return EXIT_FAILURE;
    }
    my_id = atoi(init_msg.mtext);
    printf("Your ID is: %d\n", my_id);

    struct msgbuf msg;
    msg.mtype = 1;

    printf("Send messages: \n");
    while (1) {

      scanf("%99s", buffor);
      if (strcmp(buffor, "exit") == 0) {
        kill(pid, SIGKILL);
        wait(NULL); 
        return 0;
      }
      strcpy(msg.mtext, buffor);
      if (msgsnd(server_queue, &msg, sizeof(msg.mtext), 0) == -1) {
        perror("failed to send a message");
      }
    }
  }

  return 0;
}

