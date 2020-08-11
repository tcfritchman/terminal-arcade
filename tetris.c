#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "tetris.h"

u_int16_t piece_configs[NUM_SHAPES][4] = {
    // Bits representing the 4x4 grid for each of the 7 shapes.
    // Each grouping of 4 grids contains the shape in each of it's rotations in a clockwise order.
    // Bit Ordering: left to right, top to bottom.
    //
    // I
    {
        0b0100010001000100,
        0b0000111100000000,
        0b0010001000100010,
        0b0000000011110000
     },
    // L
    {
        0b0010111000000000,
        0b0100010001100000,
        0b1110100000000000,
        0b0110001000100000
    },
    // O
    {
        0b0110011000000000,
        0b0110011000000000,
        0b0110011000000000,
        0b0110011000000000
    },
    // J
    {
        0b1110001000000000,
        0b0010001001100000,
        0b1000111000000000,
        0b1100100010000000
    },
    // S
    {
        0b0110110000000000,
        0b1000110001000000,
        0b0110110000000000,
        0b1000110001000000
    },
    // Z
    {
        0b1100011000000000,
        0b0100110010000000,
        0b1100011000000000,
        0b0100110010000000
    },
    // T
    {
        0b0100111000000000,
        0b0100011001000000,
        0b0000111001000000,
        0b0100110001000000
    }
};

struct {
    u_int16_t fixed_tiles[GRID_WIDTH * GRID_HEIGHT];
    u_int16_t left_wall_mask[GRID_WIDTH * GRID_HEIGHT];
    u_int16_t right_wall_mask[GRID_WIDTH * GRID_HEIGHT];
    u_int16_t floor_mask[GRID_WIDTH * GRID_HEIGHT];
    u_int16_t shape_queue[QUEUE_SIZE];
    u_int16_t curr_piece_type;
    u_int16_t curr_piece_conf;
    int curr_piece_position;
} game_data;

u_int16_t grid_buffer[GRID_WIDTH * GRID_HEIGHT];

void setup_next_piece() {
    int shape = rand() % NUM_SHAPES;
    game_data.curr_piece_type = shape;
    game_data.curr_piece_position = START_POS;
    game_data.curr_piece_conf = 0;
}

void remove_row(int row_id) {
    for (int i = row_id; i > 0; i--) {
        for (int j = MARGIN_WIDTH; j < GRID_WIDTH - MARGIN_WIDTH; j++) {
            u_int16_t above_tile = game_data.fixed_tiles[((i - 1) * GRID_WIDTH) + j];
            game_data.fixed_tiles[(i * GRID_WIDTH) + j] = above_tile;
        }
    }
}

void clear_rows() {
    for (int i = 0; i < GRID_HEIGHT - MARGIN_WIDTH; i++) {
        int row_set_total = 0;
        for (int j = MARGIN_WIDTH; j < GRID_WIDTH - MARGIN_WIDTH; j++) {
            u_int16_t is_current_tile_set = game_data.fixed_tiles[(i * GRID_WIDTH) + j];
            if (is_current_tile_set) {
                row_set_total++;
            }
        }
        if (row_set_total == GRID_WIDTH - (2 * MARGIN_WIDTH)) {
            remove_row(i);
        }
    }
}

void add_fixed_piece() {
        for (int i = 0; i < SHAPE_WIDTH; i++) {
        for (int j = 0; j < SHAPE_WIDTH; j++) {
            u_int16_t shape = game_data.curr_piece_type;
            u_int16_t conf = game_data.curr_piece_conf;
            u_int16_t set_bit = (piece_configs[shape][conf] << ((j * SHAPE_WIDTH) + i)) & 0x8000;
            if (set_bit) {
                u_int16_t offset = (j * GRID_WIDTH) + i;
                int index = game_data.curr_piece_position + offset;
                game_data.fixed_tiles[index] = 1;
            }
        }
    }
}

void reset_game() {
    for (int i = 0; i < GRID_WIDTH * GRID_HEIGHT; i++) {
        game_data.fixed_tiles[i] = 0;
    }
    for (int i = 0; i < QUEUE_SIZE; i++) {
        game_data.shape_queue[i] = 0;
    }
    for (int i = 0; i < GRID_WIDTH; i++) {
        for (int j = 0; j < GRID_HEIGHT; j++) {
            int index = (j * GRID_WIDTH) + i; 
            game_data.left_wall_mask[index] = (i < MARGIN_WIDTH) ? 1 : 0;
            game_data.right_wall_mask[index] = (i >= GRID_WIDTH - MARGIN_WIDTH) ? 1 : 0;
            game_data.floor_mask[index] = j >= GRID_HEIGHT - MARGIN_WIDTH ? 1 : 0;
        }
    }
    setup_next_piece();
}

void draw() {
    // draw fixed tiles
    for (int i = 0; i < GRID_WIDTH * GRID_HEIGHT; i++) {
        grid_buffer[i] = game_data.fixed_tiles[i];
    }
    // draw current piece
    for (int i = 0; i < SHAPE_WIDTH; i++) {
        for (int j = 0; j < SHAPE_WIDTH; j++) {
            u_int16_t shape = game_data.curr_piece_type;
            u_int16_t conf = game_data.curr_piece_conf;
            u_int16_t set_bit = (piece_configs[shape][conf] << ((j * SHAPE_WIDTH) + i)) & 0x8000;
            grid_buffer[game_data.curr_piece_position + (j * GRID_WIDTH) + i] |= set_bit;
        }
    }
    // DEBUG print margins
    for (int i = 0; i < GRID_WIDTH * GRID_HEIGHT; i++) {
        grid_buffer[i] = grid_buffer[i] | game_data.left_wall_mask[i] | game_data.right_wall_mask[i] | game_data.floor_mask[i];
    }
}

void debug_print_buf() {
    for (int i = -1; i < GRID_HEIGHT + 1; i++) {
        printf("|");
        for (int j = 0; j < GRID_WIDTH; j++) {
            if (i == -1 || i == GRID_HEIGHT) {
                printf("--");
            } else {
                printf("%s ", grid_buffer[(i * GRID_WIDTH) + j] == 0 ? " " : "#");
            }
        }
        printf("|\n");
    }
}

int collision_detect(u_int16_t shape, u_int16_t conf, int pos, u_int16_t mask[]) {
    for (int i = 0; i < SHAPE_WIDTH; i++) {
        for (int j = 0; j < SHAPE_WIDTH; j++) {
            u_int16_t set_bit = (piece_configs[shape][conf] << ((j * SHAPE_WIDTH) + i)) & 0x8000;
            if (set_bit) {
                u_int16_t offset = (j * GRID_WIDTH) + i;
                u_int16_t index = pos + offset;
                if (mask[index]) return 1;
            }
        }
    }
    return 0;
}

void move_piece_horizontal(int direction) {
    if (direction != -1 && direction != 1) return;
    int new_pos = game_data.curr_piece_position + direction;
    u_int16_t shape = game_data.curr_piece_type;
    u_int16_t conf = game_data.curr_piece_conf;
    if (collision_detect(shape, conf, new_pos, game_data.left_wall_mask) 
        || collision_detect(shape, conf, new_pos, game_data.right_wall_mask) 
        || collision_detect(shape, conf, new_pos, game_data.fixed_tiles)) {
        return;
    }
    game_data.curr_piece_position = new_pos;
}

int move_piece_down() {
    int new_pos = game_data.curr_piece_position + GRID_WIDTH;
    u_int16_t shape = game_data.curr_piece_type;
    u_int16_t conf = game_data.curr_piece_conf;
    if (collision_detect(shape, conf, new_pos, game_data.floor_mask) 
        || collision_detect(shape, conf, new_pos, game_data.fixed_tiles)) {
        return 1;
    }
    game_data.curr_piece_position = new_pos;
    return 0;
}

u_int16_t *get_grid_buffer() {
    return grid_buffer;
}

void rotate_piece(int direction) {
    if (direction != -1 && direction != 1) return;
    // Rotate resulting in wall collision should bump the piece over.
    // Wall collision is only possible if the horizontal position is off the edge of the grid
    // Collision with another block should result in no rotation occuring
    u_int16_t new_conf = (game_data.curr_piece_conf + direction) % 4;
    int new_pos = game_data.curr_piece_position;
    u_int16_t shape = game_data.curr_piece_type;
    u_int16_t conf = new_conf;
    while (collision_detect(shape, conf, new_pos, game_data.left_wall_mask)) {
        new_pos++;
    }
    while (collision_detect(shape, conf, new_pos, game_data.right_wall_mask)) {
        new_pos--;
    }
    if (collision_detect(shape, conf, new_pos, game_data.floor_mask) 
        || collision_detect(shape, conf, new_pos, game_data.fixed_tiles)) {
        return;
    }
    game_data.curr_piece_position = new_pos;
    game_data.curr_piece_conf = new_conf;
}
