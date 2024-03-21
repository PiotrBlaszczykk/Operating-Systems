#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{

  printf("Czyszczenie pliku: %s\n", argv[1]);

  const char *pathname = "plik2.txt";
  int file_descriptor = open(argv[1], O_WRONLY | O_TRUNC);

  close(file_descriptor);

}

