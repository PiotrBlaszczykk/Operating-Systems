#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Odwracanie zawartości pliku blokami po 1024 bajty
void reverse_file(const char *input_file, const char *output_file) {

    printf("Odwracanie zawartości pliku blokami po 1024 bajty\n");

    FILE *input_fp, *output_fp;
    unsigned char buffer[1024];
    long file_size;

    input_fp = fopen(input_file, "rb");
    output_fp = fopen(output_file, "wb");

    fseek(input_fp, 0, SEEK_END);
    file_size = ftell(input_fp);
    rewind(input_fp);

    for (long offset = file_size; offset > 0; offset -= sizeof(buffer)) {
        size_t bytes_to_read = sizeof(buffer);
        if (offset < sizeof(buffer)) {
            bytes_to_read = offset;
        }

        fseek(input_fp, -bytes_to_read, SEEK_CUR);
        fread(buffer, 1, bytes_to_read, input_fp);
        fseek(input_fp, -bytes_to_read, SEEK_CUR);

        for (size_t i = 0; i < bytes_to_read / 2; ++i) {
            unsigned char temp = buffer[i];
            buffer[i] = buffer[bytes_to_read - i - 1];
            buffer[bytes_to_read - i - 1] = temp;
        }

        fwrite(buffer, 1, bytes_to_read, output_fp);
    }

    fclose(input_fp);
    fclose(output_fp);
}

int main(int argc, char *argv[]) {
    time_t start, koniec;
    double elapsed;
    time(&start);

    if (argc != 3) {
        printf("Zły input, podaj plik wejściowy i wyjściowy\n");
        return 1;
    }

    reverse_file(argv[1], argv[2]);

    time(&koniec);
    elapsed = difftime(koniec, start);
    printf("Czas wykonania: %.2f sekund\n", elapsed);

    FILE *file = fopen("pomiar_zad2.txt", "a");
    fprintf(file, "Czas wykonania czytajac po 1024 znaki: %.2f sekund\n", elapsed);
    fclose(file);

    return 0;
}

