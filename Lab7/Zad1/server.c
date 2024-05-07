#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
//#include <signal.h>
#include <unistd.h>
#include <string.h>

struct msgbuf {
  long mtype;
  char mtext[100];
};

struct client {
  int client_id;
  key_t client_key;
  int client_queue;
};

// max 10 klientow
//SYSTEM V

void print_clients(struct client clients[10]);

int main() {

  struct client clients[10];
  int client_no = 0;
  char buffor[100];
  char chr_client_id;

  for(int i = 0; i < 10; i ++) {
    clients[i].client_id = -1;
    clients[i].client_key = -1;
    clients[i].client_queue = -1;
  }

  key_t server_key = ftok("/bin/ls", 's');
  if (server_key == -1) {
    printf("ftok failed\n");
    return EXIT_FAILURE;
  }

  int server_queue = msgget(server_key, 0666 | IPC_CREAT);
  if (server_queue == -1) {
    printf("msgget failed\n");
    return EXIT_FAILURE;
  }

  printf("server queue created successfully\n");

  struct msgbuf msg;

  while(1) {

    if (msgrcv(server_queue, &msg, sizeof(msg.mtext), 0, 0) == -1) {
      printf("msgrcv failed\n");
    } else {
      printf("message recieved: %s\n", msg.mtext);
    }
    
    printf("msg type: %ld\n", msg.mtype);

    if (msg.mtype == 2) {

      printf("INIT msg recieved\n");
      clients[client_no].client_id = client_no;
      clients[client_no].client_key = atoi(msg.mtext);
      clients[client_no].client_queue = msgget(clients[client_no].client_key, 0666);

      print_clients(clients);

      sprintf(buffor, "%d", client_no);
      strcpy(msg.mtext, buffor);
      
      msgsnd(clients[client_no].client_queue, &msg, sizeof(msg.mtext), 0);

      client_no ++;

    } else {

      printf("msg: %s\n", msg.mtext);
      
      for (int i = 0; i < client_no; i++) {

        msgsnd(clients[i].client_queue, &msg, sizeof(msg.mtext), 0);
      }

    }


  }

  return 0;
}

void print_clients(struct client clients[10]) {

  if (clients[0].client_id == -1) {
    printf("no clients registered \n");
  } else {

   for (int i = 0; i < 10; i++) {
      
      if (clients[i].client_id == -1) {
        break;
      } else {

        printf("client no %d, client's key: %d, client's queue: %d\n", clients[i].client_id, clients[i].client_key, clients[i].client_queue);
      }
     
    }

  }

}
