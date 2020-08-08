#include <stdlib.h>

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