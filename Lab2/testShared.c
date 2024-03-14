#include <stdio.h>
#include "shared.h"

int main() {
  printf("start\n");

  int n;
  printf("podaj n: ");
  scanf("%d", &n);
  printf("\n");

  int max_iter;
  printf("podaj max iteracji: ");
  scanf("%d", &max_iter);
  printf("\n");

  int wynik = test_collatz_convergence(n, max_iter);
  printf("liczba iteracji: %d", wynik);
  printf("\n");

  return 0;

}
