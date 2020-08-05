#include "display.h"

int main(int argc, char **argv) {
    set_screen_dimensions(8, 6);
    display_init();

    u_int8_t pixels_array1[8 * 6] = {
        1,3,6,7,2,1,3,2,
        5,6,1,3,6,3,7,6,
        1,2,2,6,4,6,2,4,
        5,6,2,4,1,7,6,2,
        5,2,6,3,5,6,2,1,
        1,3,6,7,2,1,2,2
    };

    u_int8_t pixels_array2[8 * 6] = {
        1,3,6,7,2,1,3,2,
        5,6,0,3,6,0,7,6,
        1,2,2,6,4,6,2,4,
        5,6,0,4,1,0,6,2,
        5,2,6,0,0,6,2,1,
        1,3,6,7,2,1,2,2
    };

    int b = 0;
    while (1) {
        frame_data fd;
        b = get_key_input() == up_arrow ? 0 : 1;
        fd.pixels = b ? pixels_array1 : pixels_array2;
        fd.text = NULL;
        display_update(&fd);
        sleep(1);
    }

    display_destroy();
}