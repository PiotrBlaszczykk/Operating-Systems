#pragma once
#include <stdbool.h>

void prepare_cell_info(); 
char *create_grid();
void destroy_grid(char *grid);
void draw_grid(char *grid);
void init_grid(char *grid);
bool is_alive(int row, int col, char *grid);
void *thread_function(void *arg);
void update_grid(char *src, char *dst);
