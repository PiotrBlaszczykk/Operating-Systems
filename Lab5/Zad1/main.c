#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

void handler(int signum) {
  printf("Otrzymano sygnal\n");
}

int main(int argc, char *argv[]) {

  if (argc != 2) {
    printf("Podaj 1 argument\n");
    return EXIT_FAILURE;
  }

  char *arg = argv[1];
  char *input1 = "none";
  char *input2 = "ignore";
  char *input3 = "handler";
  char *input4 = "mask";

  int wariant = 0;

  if (strcmp(arg, input1) == 0) {
    wariant = 1;
  } else if (strcmp(arg, input2) == 0) {
    wariant = 2;
  } else if (strcmp(arg, input3) == 0) {
    wariant = 3;
  } else if (strcmp(arg, input4) == 0) {
    wariant = 4;
  }

  switch (wariant) {
    case 1:
      printf("Wariant 1\n");
      break;
    
    case 2:
      printf("Wariant 2\n");
      signal(SIGUSR1, SIG_IGN);
      printf("Zignorowano sygnal\n");
      break;

    case 3:
      printf("Wariant 3\n");
      signal(SIGUSR1, handler);
      break;

    case 4:
      printf("Wariant 4\n");
      sigset_t newmask, oldmask, pending;
      sigemptyset(&newmask);
      sigaddset(&newmask, SIGUSR1);
      sigprocmask(SIG_BLOCK, &newmask, &oldmask); 
      
      raise(SIGUSR1);

      sigpending(&pending); 
      if (sigismember(&pending, SIGUSR1)) {
        printf("Sygnal SIGUSR1 oczekuje\n");
      } else {
        printf("Sygnal SIGUSR1 nie oczekuje\n");
      }
      
      sigprocmask(SIG_SETMASK, &oldmask, NULL);  
      break;

    default:
      printf("Zly argument\n");
      break;
  }

  raise(SIGUSR1);  

  return 0;
}

