#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>

long double fun(long double x);

int main(int argc, char *argv[]) {

  if(argc != 2) {
    printf("potrzebne PID\n");
  }

  int pid = atoi(argv[1]);

  char *potok1 = "/tmp/myfifo1";
  char *potok2 = "/tmp/myfifo2";
  char *buffor1[100];
  char *buffor2[100];
  char *buffor3[100];

  FILE *fp1 = fopen(potok1, "r");

  fgets(buffor1, sizeof(buffor1), fp1);
  fgets(buffor2, sizeof(buffor2), fp1);

  fclose(fp1);
  
  long double start = strtold(buffor1, NULL);
  long double end = strtold(buffor2, NULL);
  long double dx = 0.00000001;
  long double sum = 0.0;
  long double x = start;

  while(x < end) {
    sum += fun(x) * dx;
    x += dx;
  }
 // printf("sum: %.15Lf\n", sum);

 // printf("wysylam sygnal do PID: %d\n", pid);
  kill(pid, SIGUSR1);

  
  FILE *fp2 = fopen(potok2, "w");
  sprintf(buffor3, "%.15Lf", sum);
  fputs(buffor3, fp2);
  //printf("wyslano dane spowrotem\n");
  fclose(fp2);

   //sleep(1);
  //printf("zakonczono liczenie w obliczanie.c\n");

  return 0;
}

long double fun(long double x) {
  
  long double one = 1.0;
  long double four = 4.0;

  long double result = four / ((x * x) + one);
  return result;
}



