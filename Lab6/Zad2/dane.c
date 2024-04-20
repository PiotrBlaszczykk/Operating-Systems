#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>

void handler(int signum) {
  if (signum == SIGUSR1) {
    printf("obliczenia skonczone\n");
  }
}

int main(int argc, char *argv[]) {

  if (argc != 3) {
    printf("podaj start i koniec calki\n");
    return EXIT_FAILURE;
  }

  signal(SIGUSR1, handler);

  char *potok1 = "/tmp/myfifo1";
  mkfifo(potok1, 0666);

  char *potok2 = "/tmp/myfifo2";
  mkfifo(potok2, 0666);

  char *start = argv[1];
  char *end = argv[2];

  pid_t pid = fork();
  if (pid == 0) {
    char pid_str[10];
    sprintf(pid_str, "%d", getppid());
    printf("uruchomiono liczenie\n");
    execl("./obliczanie", "obliczanie", pid_str, NULL);

  }

  else {
   
    //printf("pid: %d\n", pid);
    FILE *fp1 = fopen(potok1, "w");
    fprintf(fp1, "%s\n%s\n", argv[1], argv[2]);

    fclose(fp1);
    //printf("wyslano dane\n");

    pause();
    //sleep(0.1);

    char buffor[100];
    
    FILE *fp2 = fopen(potok2, "r");
    fgets(buffor, sizeof(buffor), fp2);
    fclose(fp2);

    printf("wynik: %s\n", buffor);

  }

  unlink(potok1);
  unlink(potok2);
  return 0;
}
