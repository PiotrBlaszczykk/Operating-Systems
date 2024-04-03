#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("podaj 1 argument\n");
    return EXIT_FAILURE;
  }

  int n = atoi(argv[1]);

  printf("Liczba procesow do wykonania: %d\n", n);

  pid_t pid;

  printf("proces macierzysty, PID: %d\n", getpid());

  for(int i = 0; i < n; i++) {
    pid = fork();

    if (pid == 0) {
      
      printf("Proces potomny, PID rodzica = %d, Moj PID = %d\n", getppid(), getpid());
      exit(0); 
    }
  }

  while (wait(NULL) > 0);

  printf("wykonano %d procesow\n", n);
  return 0;
}
