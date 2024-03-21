#include <stdio.h>
#include <stdlib.h>
#include <dirent.h> 
#include <sys/stat.h>
#include <unistd.h>

int main(void) {
    DIR *d;
    struct dirent *dir;
    struct stat buf;
    long long total_size = 0;

    d = opendir(".");

    while ((dir = readdir(d)) != NULL) {
        if (dir->d_name[0] == '.' && (dir->d_name[1] == '\0' || (dir->d_name[1] == '.' && dir->d_name[2] == '\0')))
            continue;

        if (stat(dir->d_name, &buf) == -1) {
            perror("stat() error");
            continue;
        }

        if (!S_ISDIR(buf.st_mode)) {
            printf("Nazwa pliku: %s, rozmiar: %lld bajtow\n", dir->d_name, (long long)buf.st_size);
            total_size += buf.st_size;
        }
    }

    closedir(d);
    printf("Sumaryczny rozmiar plików: %lld bajtow\n", total_size);

    return EXIT_SUCCESS;
}
