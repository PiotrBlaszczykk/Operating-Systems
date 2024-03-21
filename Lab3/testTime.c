#include <stdio.h>
#include <time.h>

int main() {

  time_t start, koniec;
  double elapsed;
  time(&start);

  long long x = 0;

  for(long long i = 0; i < 10000000000; i++) {
    x ++;
  }

  time(&koniec);
  elapsed = difftime(koniec, start); 
  printf("Czas wykonania: %.2f sekund\n", elapsed);

}
