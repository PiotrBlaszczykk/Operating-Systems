#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <string.h>

#define max_name_length 20
#define BUFF_SIZE 1024

int main(int argc, char *argv[]) {

  if (argc != 3) {
    printf("Podaj imie oraz adres servera");
    return -1;
  }
  
  char name[max_name_length];
  for(int i = 0; i < max_name_length; i ++) {
    name[i] = argv[1][i];
  }

  int port = atoi(argv[2]);
  int server_fd;
  struct sockaddr_in server_address;
  socklen_t server_addr_size;
  char buffer[BUFF_SIZE];

  server_fd = socket(AF_INET, SOCK_STREAM, 0);

  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = INADDR_ANY;
  server_address.sin_port = htons(port);

  if (connect(server_fd,(struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
    printf("connect failed\n");
    return -1;
  } else {
    printf("polaczono z serwerem\n");
  }

  
  char innit_msg[BUFF_SIZE];
  write(server_fd, &name, sizeof(char) * 20);


  pid_t pid = fork();

  if (pid == 0) {
    // dziecko, odczytuje wiadomosci
    char buffer[BUFF_SIZE];
    int bytes_read;

    while (1) {
      memset(buffer, 0, BUFF_SIZE);

      bytes_read = read(server_fd, &buffer, BUFF_SIZE);
      if (bytes_read <= 0) {
        printf("utracono poleczenie z serwerem\n");
        return 0;
      }
      printf("%s\n", buffer);

    }
  } else {

    // rodzic - pisze

    char innit_msg[BUFF_SIZE];
    int scenario;

    while (1) {

      char buffer[BUFF_SIZE];
      char info[BUFF_SIZE];
      char msg_from_server[BUFF_SIZE];
      
      scanf("%s", buffer);
      scenario = 0;
      
      if (strcmp(buffer, "STOP") == 0) {
        scenario = 1;
      } else if (strcmp(buffer, "LIST") == 0) {
        scenario = 2;
      } else if (strcmp(buffer, "2ALL") == 0) {
        scenario = 3;
      } else if (strcmp(buffer, "2ONE") == 0) {
        scenario = 4;
      } else if (strcmp(buffer, "ALIVE") == 0) {
        scenario = 5;
      } else if (strcmp(buffer, "TEST") == 0) {
        scenario = 6;
      }

      switch(scenario) {
        case 0:
          printf("nieprawdidlowa opcja\n");
          break;

        case 1:
          printf("wylogowano sie\n");
          strcpy(info, "1");
          write(server_fd, &info, BUFF_SIZE);
          shutdown(server_fd, SHUT_RDWR);
          close(server_fd);
          return 0;

        case 2: // LIST
          
          strcpy(info, "2");
          write(server_fd, &info, BUFF_SIZE);
          break;

        case 3: // 2ALL
          strcpy(info, "3");

          write(server_fd, &info, BUFF_SIZE);
          scanf("%s", buffer);
          write(server_fd, &buffer, BUFF_SIZE);
          break;

        case 4: // 2ONE
          printf("podaj id a potem napisz wiadomosc\n");
          strcpy(info, "4");

          write(server_fd, &info, BUFF_SIZE);
          memset(info, 0, BUFF_SIZE);
          scanf("%s", info);
          write(server_fd, &info, BUFF_SIZE);
          scanf("%s", buffer);
          write(server_fd, &buffer, BUFF_SIZE);
          break;

        case 5: // ALIVE
          strcpy(info, "5");
     
          write(server_fd, &info, BUFF_SIZE);
          break;  

        case 6: // TEST
          strcpy(info, "6");
          write(server_fd, &info, BUFF_SIZE);
          scanf("%s", buffer);
          write(server_fd, &buffer, BUFF_SIZE);

        default:
          printf("nic\n");
          break;
      }

    }
  }

  shutdown(server_fd, SHUT_RDWR);
  close(server_fd);
  return 0;

}
