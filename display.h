#include <stdlib.h>

typedef struct frame_data {
    u_int8_t *pixels;
    char *text;
} frame_data;

typedef enum key_control {
    left_arrow,
    right_arrow,
    up_arrow,
    down_arrow,
    none
} key_control;

/* Configuration functions */
void set_screen_dimensions(u_int16_t w, u_int16_t h);
/* Initialization & Destruction */
void display_init();
void display_destroy();
/* Update Screen */
void display_update(frame_data *f);
/* Get Keyboard Input */
key_control get_key_input();
