#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int global = 0; 
void incrementGlobal() {
    global++; }

int main(int argc, char *argv[]) {

  int local = 10;
  int status;

  if (argc != 2) {
    printf("podaj 1 argument\n");
    return EXIT_FAILURE;
  }
  
  char* sciezka = argv[1]; 
  printf("Sciezka: %s\n", sciezka);

  pid_t pid = fork();

  if (pid == -1) {

    printf("fork nie powiodl sie\n");
  } else if (pid == 0) {

    printf("child process\n");
    pid_t parent_pid = getppid();
    printf("child PID: %d ", getpid());
    printf("parent PID: %d \n", parent_pid);

    incrementGlobal();
    local ++;

    printf("child's local: %d ", local);
    printf("child's global: %d\n", global);
    
    if (execl("/bin/ls", "ls", sciezka, NULL) == -1) {

      perror("execl");
      exit(EXIT_FAILURE);
    }

    exit(69);

  } else {
   
    printf("parent process\n");
    printf("parent PID: %d ", getpid());
    printf("child PID: %d \n", pid);

    wait(&status);

    printf("parent's local: %d ", local);
    printf("parent's global: %d\n", global);


    if (WIFEXITED(status)) {
      int exit_status = WEXITSTATUS(status);
      printf("Child exit code: %d\n", exit_status);
      return exit_status;
    } else {
      return 1;
    }

  }
 // return 0;
}

