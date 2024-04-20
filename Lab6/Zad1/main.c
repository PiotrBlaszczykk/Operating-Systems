#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/time.h>

long double fun(long double x);

long double licz(long double x, long double zakres, long double dx);

int main(int argc, char *argv[]) {

  if(argc != 3) {
    printf("podaj dx i n\n");
    return EXIT_FAILURE;
  }

  struct timeval start, end;

  long double dx = strtold(argv[1], NULL);
  int n = atoi(argv[2]);

  long double long_n = n;
  long double one = 1.0;
  long double zakres = one / long_n;

  long double sum = 0.0;
  long double x = 0.0;

  int fd[2];
  pid_t pid;
  long double policzone;
  long double odczytane;

  gettimeofday(&start, NULL);

  for (int i = 0; i < n; i ++) {
      
    pipe(fd);
    pid = fork();

    if (pid == 0) {

      policzone = licz(x, zakres, dx);

      close(fd[0]);
      write(fd[1], &policzone, sizeof(long double));
      close(fd[1]);
      exit(0);

    } else {

      close(fd[1]);
      read(fd[0], &odczytane, sizeof(long double));
      close(fd[0]);

      sum += odczytane;
      x += zakres;
      wait(NULL);
    }

  }

  // gdy n > 1/dx, to niektore fragmety sa liczone za wiele razy
  // wiec wynik trzeba wtedy skorygowac
  long double check = one / long_n;
  if(dx >= check) {

    long double korekta = check / dx;
    sum *= korekta;
  }

  printf("wynik: %.15Lf\n", sum);

  gettimeofday(&end, NULL);

  double seconds = end.tv_sec - start.tv_sec;
  double microseconds = end.tv_usec - start.tv_usec;
  double elapsed = seconds + microseconds*1e-6;


  FILE *file = fopen("wyniki.txt", "a");

  fprintf(file, "dx = %s n = %d, czas: %.6f sekund, wynik: %.15Lf\n", argv[1], n, elapsed, sum);
  fclose(file);


  return 0;
    
}

long double fun(long double x) {
  
  long double one = 1.0;
  long double four = 4.0;

  long double result = four / ((x * x) + one);
  return result;
}

long double licz(long double x, long double zakres, long double dx) {
  
  long double sum = 0.0;
  long double koniec = x + zakres;

  while(x <= koniec) {

    sum += fun(x) * dx;
    x += dx;

  }

  return sum;

}



