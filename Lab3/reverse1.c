#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// odwracanie znak po znaku (bajcie)

void reverse_file(const char *input_file, const char *output_file) {

    printf("odwracanie zawartosci pliku znak po znaku\n");

    FILE *input_fp, *output_fp;
    unsigned char temp;

    // "rb" - read binary
    input_fp = fopen(input_file, "rb");

    // "rb" - write binary
    output_fp = fopen(output_file, "wb");

    fseek(input_fp, 0, SEEK_END); 
    long file_size = ftell(input_fp); 
    rewind(input_fp); 

    for (long i = file_size - 1; i >= 0; i--) {
        fseek(input_fp, i, SEEK_SET);
        fread(&temp, 1, 1, input_fp);
        fwrite(&temp, 1, 1, output_fp);
    }

    fclose(input_fp);
    fclose(output_fp);
}

int main(int argc, char *argv[]) {

    time_t start, koniec;
    double elapsed;
    time(&start);

    if (argc != 3) {
        printf("zly input, podaj plik wejsciowy i wyjsciowy\n");
        return 1;
    }

    reverse_file(argv[1], argv[2]);

    time(&koniec);
    elapsed = difftime(koniec, start);
    printf("czas wykonania: %.2f sekund\n", elapsed);

    FILE *file = fopen("pomiar_zad2.txt", "a");
    fprintf(file, "Czas wykonania czytajac po jednym znaku: %.2f sekund\n", elapsed);
    fclose(file);

    return 0;
}
