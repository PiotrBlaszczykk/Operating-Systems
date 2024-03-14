#include <stdio.h>
#include <dlfcn.h> // Do dynamicznego ładowania bibliotek

typedef void (*Hello)();
//typedef int (*CollatzCojecture)(int input);
typedef int (*TestCollatzConvergance)(int input, int max_iter);


int main() {

    void *handle = dlopen("./libdynamic.so", RTLD_LAZY); // Ładowanie biblioteki
    if (!handle) {
        fprintf(stderr, "%s\n", dlerror());
        return 1;
    }

    Hello hello = (Hello)dlsym(handle, "hello");
    if (!hello) {
        fprintf(stderr, "%s\n", dlerror());
        dlclose(handle);
        return 1;
    }


  //  hello(); // Wywołanie funkcji z biblioteki

    TestCollatzConvergance test_collatz_convergence = (TestCollatzConvergance)dlsym(handle, "test_collatz_convergence");
    if (!test_collatz_convergence) {
        fprintf(stderr, "%s\n", dlerror());
        dlclose(handle);
        return 1;
    }

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


    dlclose(handle); // Zamknięcie uchwytu do biblioteki

    return 0;
}

