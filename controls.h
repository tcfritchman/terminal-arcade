#include <stdio.h>

typedef enum key_control {
    left_arrow,
    right_arrow,
    up_arrow,
    down_arrow
} key_control;

key_control get_key_immediate(int timeout);