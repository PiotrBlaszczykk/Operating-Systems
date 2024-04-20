#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

int main() {

  remove("wyniki.txt");

  char *dx_values[] = {"0.1", "0.01", "0.0001", "0.000001", "0.00000001", "0.000000001"};
  char *n_values[] = {"1", "10", "500", "1000", "5000", "10000"};
  int iterations = sizeof(dx_values) / sizeof(dx_values[0]);
  int processes = sizeof(n_values) / sizeof(n_values[0]);
  
  for(int i = 0; i < iterations; i++) {
    for(int j = 0; j < processes; j++) {

      pid_t pid = fork();

      if (pid == 0) {
        
        execl("./main", "main", dx_values[i], n_values[j], NULL);
        perror("execl failed");
        exit(1);
      }

      else {

        wait(NULL);
      }

    }

  }

  return 0;
}
