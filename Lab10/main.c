#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

pthread_t renifery[9];
pthread_t mikolaj;

pthread_mutex_t mutex; //= PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

int counter = 0;

void *renifer_function(void *arg) {

  pthread_t tid = pthread_self();
  int random_number;

  while (1) {
    random_number = 5 + (rand() % 6);
    printf("renifer ID: %lu, jestem na wakacjach przez %d sekund\n", tid, random_number);
    sleep(random_number);
    printf("renifer ID: %lu, wracam z wakacji, jest %d reniferow\n", tid, counter +1);

    pthread_mutex_lock(&mutex);
    counter += 1;
    if (counter == 9) {
      pthread_cond_broadcast(&cond);
      printf("budze Mikolaja\n");
    }

    while (counter != 9) {
      pthread_cond_wait(&cond, &mutex);
    }
    pthread_mutex_unlock(&mutex);

    printf("renifer ID: %lu, rozdaje z mikolajem prezenty\n", tid);
    pthread_mutex_lock(&mutex);
    while(counter != 0) {
      pthread_cond_wait(&cond, &mutex);
    }
    pthread_mutex_unlock(&mutex);

    //printf("renifer ID: %lu, skonczylem prace z mikolajem\n", tid);
  }

  pthread_exit(NULL);
}

void *mikolaj_function(void *arg) {

  pthread_t tid = pthread_self();
  int random_number;

  while(1) {

    printf("Mikolaj ID: %lu, spie\n", tid);
    pthread_mutex_lock(&mutex);
    while( counter != 9 ) {
      pthread_cond_wait(&cond, &mutex);
    }
    pthread_mutex_unlock(&mutex);

    printf("Mikolaj ID: %lu, budze sie i lece rozdawac prezenty\n", tid);
    random_number = 2 + (rand() % 3);
    sleep(random_number);

    printf("Mikolaj ID: %lu, skonczylem rozdawac prezenty\n", tid);
    pthread_mutex_lock(&mutex);
    counter = 0;
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);

  }
  pthread_exit(NULL);
}

int main() {

  srand(time(NULL));

  pthread_mutexattr_t attr;
  pthread_mutexattr_init(&attr);
  int type = PTHREAD_MUTEX_RECURSIVE;
  pthread_mutexattr_settype(&attr, type);

  pthread_mutex_init(&mutex, &attr);

  pthread_create(&mikolaj, NULL, mikolaj_function, NULL);
  for (int i = 0; i < 9; i++ ) {
    pthread_create(&renifery[i], NULL, renifer_function, NULL);
  }

  pthread_join(mikolaj, NULL);
  for (int i = 0; i < 9; i ++ ) {
    pthread_join(renifery[i], NULL);
  }

  return 0;
}
