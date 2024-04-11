#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void handler(int signum) {
  printf("ootrzymano sygnal od chatcher, koncze prace\n");
  exit(0);
}

int main(int argc, char *argv[]) {

  if(argc != 3) {
    printf("podaj PID i tryb pracy\n");
    return EXIT_FAILURE;
  }
  
  pid_t pid = atoi(argv[1]);
  int tryb = atoi(argv[2]);

  union sigval value;
  value.sival_int = tryb;
  
  sigqueue(pid, SIGUSR1, value);

  signal(SIGUSR1, handler);

  pause();

  return 0;
}
