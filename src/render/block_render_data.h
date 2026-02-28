#include "app/world/cell.h"

static uint8_t BLOCK_TEXTURE_SIZE = 64;

static const char* BLOCK_KIND_TEXTURE_FILENAME[BLOCK_KIND_COUNT] =
{
    // BLOCK_KIND_NONE
    NULL,

    // BLOCK_KIND_EAGLE
    "eagle.png",

    // BLOCK_KIND_LION
    "lion.png",

    // BLOCK_KIND_WOLF
    "wolf.png",

    // BLOCK_KIND_HORSE
    "horse.png",
};