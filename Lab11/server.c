#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>

#define BUFF_SIZE 1024
#define max_no_clients 10

struct client {

  int client_id;
  int client_fd;
  struct sockaddr_in client_addr;
  socklen_t client_addr_size;
  char client_name[20];

  int active; // zeby list ladnie wygladalo
  pthread_t thread_id; // watek, ktory odpowiada za komuniakacje z klientem

};

struct thread_info {
  int cl_id;
};

struct client clients[max_no_clients];

pthread_mutex_t mutex; 

void prepare_clients_array();
void show_clients();
int get_free_id(); 
void clean_clients();
void clean_threads();

int biggest_client_id; // potrzebne, aby wiedziec ile watkow zjoinowac

void *thread_function(void *arg) {


    struct thread_info *info = (struct thread_info *)arg;
    int cl_id = info->cl_id;
    char buffer[BUFF_SIZE];
    int scenario; // jaki protokol realizujemy
    int bytes_read;
    char msg_from[BUFF_SIZE];

    int flag = 0; // czy klient zostal juz spingowany, 0 - nie, 1 - tak;
    
    char ping[BUFF_SIZE];

    while (1) {

    fd_set read_fds;
    struct timeval timeout;

    FD_ZERO(&read_fds);
    FD_SET(clients[cl_id].client_fd, &read_fds);

    timeout.tv_sec = 60;
    timeout.tv_usec = 0;

    int activity = select(clients[cl_id].client_fd + 1, &read_fds, NULL, NULL, &timeout);

    memset(buffer, 0, BUFF_SIZE); // czysczenie
    
    if (activity == 0) {

      if (flag == 0) {

        memset(ping, 0, BUFF_SIZE);
        strcpy(ping, "PING");
        printf("pingowanie klienta\n");
        write(clients[cl_id].client_fd, &ping, BUFF_SIZE);
        flag = 1;
      } else {

        printf("klient id: %d nick: %s byl zbyt dlugo nieaktywny i zostal rozlaczony\n", clients[cl_id].client_id, clients[cl_id].client_name);
        pthread_mutex_lock(&mutex);
        clients[cl_id].client_id = -1;
        shutdown(clients[cl_id].client_fd, SHUT_RDWR);
        close(clients[cl_id].client_fd);
        pthread_mutex_unlock(&mutex);
        pthread_exit(NULL);

      }

    } else if (activity < 0) {

      printf("select error\n");

    } else {

      scenario = 0; // defaultowy scenariusz
      bytes_read = read(clients[cl_id].client_fd, &buffer, sizeof(buffer));

      if (bytes_read <= 0) {

        printf("utracono poloczenie z %s\n", clients[cl_id].client_name);
        pthread_mutex_lock(&mutex);
        clients[cl_id].client_id = -1;
        shutdown(clients[cl_id].client_fd, SHUT_RDWR);
        close(clients[cl_id].client_fd);
        clean_clients();
        pthread_mutex_unlock(&mutex);
        pthread_exit(NULL);

      } else {

        printf("protokol od klienta %s : %s\n", clients[cl_id].client_name,  buffer);
        flag = 0;
        char c = buffer[0];
        scenario = atoi(&c);

      }

      switch(scenario) {
        case 0:
          printf("nic\n");
          break;

        case 1:
          printf("klient sie wylogowal\n");
          pthread_mutex_lock(&mutex);
          clients[cl_id].client_id = -1;
          clean_clients();
          pthread_mutex_unlock(&mutex);
          pthread_exit(NULL);
          break;

        case 2:
          printf("LIST\n");
          memset(buffer, 0, BUFF_SIZE);
          char id_to_print[2];
          int id_copy;

          pthread_mutex_lock(&mutex);
          for (int i = 0; i < max_no_clients; i ++) {

            if (clients[i].client_id != -1 && clients[i].active == 1) {
              
              id_copy = clients[i].client_id; 
              sprintf(id_to_print, "%d", id_copy);

              memset(buffer, 0, BUFF_SIZE);

              strcpy(buffer, "id: ");
              strcat(buffer, id_to_print);
              strcat(buffer, " nick: ");
              strcat(buffer, clients[i].client_name);
              
              write(clients[cl_id].client_fd, &buffer, BUFF_SIZE);

            }
          }
          pthread_mutex_unlock(&mutex);
          memset(buffer, 0, BUFF_SIZE);
          break;

        case 3:
          printf("2ALL\n");

          memset(msg_from, 0, BUFF_SIZE);
          strcpy(msg_from, "wiadomosc od ");
          memset(buffer, 0, BUFF_SIZE);
          read(clients[cl_id].client_fd, &buffer, BUFF_SIZE);
          strcat(msg_from, clients[cl_id].client_name);
          strcat(msg_from, " : ");
          strcat(msg_from, buffer);

          pthread_mutex_lock(&mutex);
          for (int i = 0; i < max_no_clients; i ++) {
            
            if( clients[i].client_id == -1) {
              break;
            }

            if (clients[i].client_id != cl_id) {
              
              write(clients[i].client_fd, &msg_from, BUFF_SIZE);
            }
          }
          pthread_mutex_unlock(&mutex);
          memset(buffer, 0, BUFF_SIZE);


          break;
        case 4:
          printf("2ONE\n");
          memset(buffer, 0, BUFF_SIZE);
          read(clients[cl_id].client_fd, &buffer, sizeof(buffer));
          int id_to_send = atoi(&buffer[0]);
          memset(buffer, 0, BUFF_SIZE);
          read(clients[cl_id].client_fd, &buffer, sizeof(buffer));

          memset(msg_from, 0, BUFF_SIZE);
          strcpy(msg_from, "wiadomosc od ");
          strcat(msg_from, clients[cl_id].client_name);
          strcat(msg_from, " : ");
          strcat(msg_from, buffer);


          pthread_mutex_lock(&mutex);
          write(clients[id_to_send].client_fd, &msg_from, BUFF_SIZE); 
          pthread_mutex_unlock(&mutex);

          break;

        case 5:
          
          printf("ALIVE\n");
          pthread_mutex_lock(&mutex);
          pthread_mutex_unlock(&mutex);
          break; 

        case 6:
          printf("TEST\n");
          memset(buffer, 0, BUFF_SIZE);
          read(clients[cl_id].client_fd, buffer, BUFF_SIZE);
          pthread_mutex_lock(&mutex);
          pthread_mutex_unlock(&mutex);
          printf("wiadomosc testowa od klienta %s : %s\n", clients[cl_id].client_name,  buffer);
          break;

        default:
          printf("nic\n");
          break;
        }
      
    }
  }

  pthread_exit(NULL);
}

int main(int argc, char *argv[]) {

  if (argc != 2) {
    printf("PODAJ PORT!!!\n");
    return -1;
  }

  pthread_mutexattr_t mutex_attr;
  pthread_mutexattr_init(&mutex_attr);
  int type = PTHREAD_MUTEX_ERRORCHECK;
  pthread_mutexattr_settype(&mutex_attr, type);
  pthread_mutex_init(&mutex, &mutex_attr);


  int port = atoi(argv[1]);
  int server_fd;
  struct sockaddr_in server_address;
  socklen_t server_addr_size = sizeof(server_address);
  char buffer[BUFF_SIZE];

  server_fd = socket(AF_INET, SOCK_STREAM, 0);

  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = INADDR_ANY;
  server_address.sin_port = htons(port);

  if ((bind(server_fd, (struct sockaddr *)&server_address, sizeof(server_address))) == -1) {
    printf("bind failed\n");
    return -1;
  }

  prepare_clients_array();
  printf("server gotowy i czeka na klientow\n");
  show_clients();
  biggest_client_id = 0; // potrzebne, aby wiedziec ile watkow "zjoinowac"
    
  while(1) {

    pthread_mutex_lock(&mutex);
    clean_threads();
    clean_clients();
    pthread_mutex_unlock(&mutex);

    
    if (listen(server_fd, max_no_clients) == -1) {
      printf("listen failed\n");
      return -1;
    }

    pthread_mutex_lock(&mutex);
    int cl_id = get_free_id();
    pthread_mutex_unlock(&mutex);
 

    if (cl_id >= 0){

      pthread_mutex_lock(&mutex);
      clients[cl_id].client_id = cl_id;
      pthread_mutex_unlock(&mutex);

      clients[cl_id].client_fd = accept(server_fd, (struct sockaddr *)&clients[cl_id].client_addr, &clients[cl_id].client_addr_size);
      if (clients[cl_id].client_fd == -1) {
        printf("accept failed\n");
        return -1;
      } else {
        printf("accept succeeded\n");
      }

      // odczytujemy nick klienta, dzieje sie to automatycznie po zarejestrowaniu klienta
      read(clients[cl_id].client_fd, &clients[cl_id].client_name, sizeof(char) * 20);
      // robie watek do oblugi klienta
      pthread_mutex_lock(&mutex);
      clients[cl_id].active = 1;
      pthread_mutex_unlock(&mutex);

      struct thread_info info;
      info.cl_id = cl_id;
      pthread_create(&clients[cl_id].thread_id, NULL, thread_function, &info);

      show_clients();
    }   
  }

  for (int i = 0; i < biggest_client_id; i ++ ){ 
    pthread_join(clients[i].thread_id, NULL);
  }

  show_clients();
  shutdown(server_fd, SHUT_RDWR);
  close(server_fd);

  return 0;

}

void prepare_clients_array() {
  
  for (int i = 0; i < max_no_clients; i ++) {
    
    clients[i].client_id = -1;
    clients[i].client_addr_size = sizeof(clients[i].client_addr);
    clients[i].active = 0;
  }
}

void show_clients() {

  printf("klienci:\n");
  int flag = 0;
  for (int i = 0; i < max_no_clients; i ++) {
    if (clients[i].client_id != -1) {
      printf("id: %d, nick: %s\n", clients[i].client_id, clients[i].client_name);
      flag = 1;
    }
  }

  if (flag == 0) {
    printf("brak klientow\n");
  }
}

void clean_clients() {
  for (int i = 0; i < max_no_clients; i ++) {
    if (clients[i].client_id == -1) {
      memset(clients[i].client_name, 0, sizeof(char) * 20);
      clients[i].active = 0;
    }
  }
}

void clean_threads() {
  for (int i = 0; i < biggest_client_id; i ++) {
    if (clients[i].client_id == -1) {
        pthread_join(clients[i].thread_id, NULL);
    }
  }
}

int get_free_id() {
  
  // zwraca pierwsze wolne ID dla klienta, jesli zwroci -1 to nie ma miejsc

  int free_id = 0;
  for (int i = 0; i < max_no_clients; i ++) {

    if (clients[i].client_id == -1) {
      
      if (free_id > biggest_client_id) {
        biggest_client_id = free_id;
      }

      printf("znaleziono id: %d\n", free_id);
      return free_id;
    } else {

      free_id += 1;

    }
  }

  printf("brak miejsc na nowych klientow\n");
  return -1;

}
