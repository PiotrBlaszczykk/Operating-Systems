#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

int licznik = 0; 

void handler(int sig, siginfo_t *siginfo, void *context) {

    printf("odebrano sygnal SIGUSR1\n");
    licznik++;

    int tryb = siginfo->si_value.sival_int;
    pid_t pid = siginfo->si_pid;

    switch(tryb) {
      
      case 1:
        
        for(int i = 1; i < 101; i++) {
          printf("%d\n", i);
        }

        kill(pid, SIGUSR1);
        break;

      case 2:
        
        printf("licznik: %d\n", licznik);
        kill(pid, SIGUSR1);
        break;

      case 3:
        
        printf("koniec\n");
        kill(pid, SIGUSR1);
        exit(0);
    }

}

int main() {
    pid_t pid = getpid();
    printf("Catcher, PID: %d\n", pid);

    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_sigaction = &handler;
    act.sa_flags = SA_SIGINFO; 

    sigaction(SIGUSR1, &act, NULL);

    while (1) {
        pause();
    }

    return 0;
}

