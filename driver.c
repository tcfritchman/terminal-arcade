#include "display.h"
#include "tetris.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

void get_tetris_pixels(frame_data *fd) {
    int pixel_index = 0;
    u_int16_t *grid_buffer = get_grid_buffer();
    for (int i = 0; i < 23; i++) {
        for (int j = 0; j < 16; j++) {
            if (i < 20 && j > 2 && j < 13) {
                fd->pixels[pixel_index] = grid_buffer[(i * 16) + j] == 0 ? 0 : 1;
                pixel_index++;
            }
        }
    }
}

int tetris() {
    srand(time(NULL));
    reset_game();

    frame_data fd;
    fd.pixels = malloc(200);

    while (1) {
        for (int i = 0; i < 1; i++) { // "Frames" per drop
            draw();
            //debug_print_buf();
            get_tetris_pixels(&fd);
            display_update(&fd);
            //fgets(buf, INPUT_BUFFER_SIZE, stdin);
            //for (int j = 0; j < INPUT_BUFFER_SIZE; j++) {
                key_control input = get_key_input();
                switch (input) {
                    case up_arrow:
                        rotate_piece(1);
                        break;
                    case left_arrow:
                        move_piece_horizontal(-1);
                        break;
                    case down_arrow:
                        // TODO: Move downward
                        break;
                    case right_arrow:
                        move_piece_horizontal(1);
                }
            //}
            usleep(500000);
        }
        int is_piece_finished = move_piece_down();
        if (is_piece_finished) {
            add_fixed_piece();
            clear_rows();
            setup_next_piece();
        }
    }
}

int main(int argc, char **argv) {


    set_screen_dimensions(10, 20);
    display_init();
    tetris();

    // set_screen_dimensions(8, 6);
    // display_init();

    // u_int8_t pixels_array1[8 * 6] = {
    //     1,3,6,7,2,1,3,2,
    //     5,6,1,3,6,3,7,6,
    //     1,2,2,6,4,6,2,4,
    //     5,6,2,4,1,7,6,2,
    //     5,2,6,3,5,6,2,1,
    //     1,3,6,7,2,1,2,2
    // };

    // u_int8_t pixels_array2[8 * 6] = {
    //     1,3,6,7,2,1,3,2,
    //     5,6,0,3,6,0,7,6,
    //     1,2,2,6,4,6,2,4,
    //     5,6,0,4,1,0,6,2,
    //     5,2,6,0,0,6,2,1,
    //     1,3,6,7,2,1,2,2
    // };

    // int b = 0;
    // while (1) {
    //     frame_data fd;
    //     b = get_key_input() == up_arrow ? 0 : 1;
    //     fd.pixels = b ? pixels_array1 : pixels_array2;
    //     fd.text = NULL;
    //     display_update(&fd);
    //     sleep(1);
    // }

    // display_destroy();
}