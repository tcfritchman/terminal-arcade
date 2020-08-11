#include <stdlib.h>

#define GRID_WIDTH 16
#define GRID_HEIGHT 23
#define QUEUE_SIZE 2
#define NUM_SHAPES 7
#define SHAPE_WIDTH 4
#define START_POS 6
#define MARGIN_WIDTH 3

void setup_next_piece();
void remove_row(int row_id);
void clear_rows();
void add_fixed_piece();
void reset_game();
void move_piece_horizontal(int direction);
int move_piece_down();
void rotate_piece(int direction);
u_int16_t *get_grid_buffer();
void draw();