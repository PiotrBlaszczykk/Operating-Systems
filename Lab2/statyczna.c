#include "statyczna.h"
#include "stdio.h"

int collatz_conjecture(int input) {

  if (input % 2 == 0) {
    input = input / 2;
    return input;
  } else {
    input = (input * 3) + 1;
    return input;
  }

}

int test_collatz_convergence(int input, int max_iter) {

  int n = input;
  int iteracje = 0;

  while (n != 1) {

    if (iteracje == max_iter) {
      
      return -1;

    } else {

      n = collatz_conjecture(n);
      iteracje += 1;
    }

  }

  return iteracje;
}

void printMessage() {
  printf("Message from my extra static library\n");
}

