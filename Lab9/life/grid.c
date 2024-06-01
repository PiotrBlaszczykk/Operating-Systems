#include "grid.h"
//#include <bits/pthreadtypes.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <unistd.h>

#define NO_THREADS 10
const int grid_width = 40;
const int grid_height = 40;
pthread_t threads[NO_THREADS];

const int NO_CELLS = grid_width * grid_height;

const int CELLS_PER_THREAD = NO_CELLS / NO_THREADS;

struct cell_range {
  int start; // pierwszy numer komorki dla watku
  int end; // ostatni numer komorku dla watku
};

struct cell_range threads_info[NO_THREADS];


void prepare_cell_info() {

    
  for (int i = 0; i < NO_THREADS - 1; i ++) {

    struct cell_range range;
    range.start = i * CELLS_PER_THREAD;
    range.end = (range.start + CELLS_PER_THREAD) - 1;
    threads_info[i] = range;
         
    }

  threads_info[NO_THREADS - 1].start = (NO_THREADS - 1) * CELLS_PER_THREAD;
  threads_info[NO_THREADS - 1].end = NO_CELLS - 1;

}


char *create_grid()
{
    return malloc(sizeof(char) * grid_width * grid_height);
}

void destroy_grid(char *grid)
{
    free(grid);
}

void draw_grid(char *grid)
{
    for (int i = 0; i < grid_height; ++i)
    {
        // Two characters for more uniform spaces (vertical vs horizontal)
        for (int j = 0; j < grid_width; ++j)
        {
            if (grid[i * grid_width + j])
            {
                mvprintw(i, j * 2, "■");
                mvprintw(i, j * 2 + 1, " ");
            }
            else
            {
                mvprintw(i, j * 2, " ");
                mvprintw(i, j * 2 + 1, " ");
            }
        }
    }

    refresh();
}

void init_grid(char *grid)
{
    for (int i = 0; i < grid_width * grid_height; ++i)
        grid[i] = rand() % 2 == 0;
}

bool is_alive(int row, int col, char *grid)
{

    int count = 0;
    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            if (i == 0 && j == 0)
            {
                continue;
            }
            int r = row + i;
            int c = col + j;
            if (r < 0 || r >= grid_height || c < 0 || c >= grid_width)
            {
                continue;
            }
            if (grid[grid_width * r + c])
            {
                count++;
            }
        }
    }

    if (grid[row * grid_width + col])
    {
        if (count == 2 || count == 3)
            return true;
        else
            return false;
    }
    else
    {
        if (count == 3)
            return true;
        else
            return false;
    }
}

struct grid_info {
  int x;
  char *src;
  char *dst;
};


void *thread_function(void *arg) {

  struct grid_info *dane = (struct grid_info *)arg;

  int x = dane->x; //numer watku 
  char *src = dane->src; // *src 
  char *dst = dane->dst; // *dst 

  int start = threads_info[x].start;
  int end = threads_info[x].end;


  for(int i = start; i <= end; i ++) {
      
      dst[i] = is_alive(i / grid_width, i % grid_width, src);

    }

  pthread_exit(NULL);
}

void update_grid(char *src, char *dst)
{

  struct grid_info info[NO_THREADS];

  for (int i = 0; i < NO_THREADS; i++) {

    info[i].x = i;
    info[i].src = src;
    info[i].dst = dst;

    pthread_create(&threads[i], NULL, thread_function, &info[i]); 
  }

  for (int i = 0; i < NO_THREADS; i++) {
    pthread_join(threads[i], NULL);
  }

}
