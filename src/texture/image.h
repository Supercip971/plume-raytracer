#pragma once
#include "../image.h"
#include "texture.h"

typedef struct image_texture
{
    Image self_picture;
    int bytes_per_scan_lines;

} ImageTexture;

Texture image_create(Image image);
