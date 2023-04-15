// SPDX-FileCopyrightText: 2023 Enrique M.G. <quiqueiii@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "texture.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

uint8_t rnd_table [256] = {208, 39, 119, 237, 95, 70, 212, 193, 211, 4, 191, 98, 172, 163, 218, 12, 58, 73, 25, 249, 62, 135, 86, 227, 151, 166, 247, 222, 34, 67, 175, 134, 135, 201, 212, 140, 241, 180, 145, 252, 74, 167, 166, 4, 16, 95, 215, 153, 108, 77, 231, 166, 230, 52, 229, 79, 181, 60, 77, 96, 138, 114, 1, 54, 108, 48, 78, 90, 28, 63, 100, 58, 14, 159, 190, 80, 20, 55, 99, 151, 85, 12, 203, 129, 44, 126, 195, 214, 102, 19, 20, 67, 44, 164, 227, 162, 219, 32, 115, 184, 129, 131, 75, 177, 173, 147, 65, 99, 211, 176, 113, 54, 36, 255, 86, 93, 243, 182, 108, 149, 69, 13, 253, 133, 192, 27, 132, 216, 237, 14, 120, 2, 250, 49, 246, 112, 9, 16, 236, 231, 164, 80, 90, 173, 171, 189, 202, 109, 224, 45, 67, 60, 59, 234, 111, 136, 32, 94, 18, 187, 8, 44, 122, 237, 116, 248, 240, 67, 226, 228, 222, 10, 243, 217, 172, 160, 84, 215, 115, 255, 197, 19, 83, 246, 122, 246, 239, 108, 137, 130, 59, 49, 218, 200, 51, 94, 125, 72, 243, 24, 55, 109, 164, 69, 42, 165, 254, 209, 221, 10, 187, 185, 127, 29, 83, 150, 24, 112, 148, 113, 115, 233, 157, 37, 33, 24, 173, 71, 159, 221, 152, 39, 160, 214, 188, 78, 164, 76, 139, 254, 24, 3, 43, 94, 54, 230, 119, 103, 83, 242, 212, 34, 57, 24, 105, 198};

static uint8_t get_tile(int tx,int ty)
{
    return rnd_table[ (tx+ty*16) % 256] / 64;
}

static float frand()
{
    return rand()/(float)RAND_MAX;
}

uint32_t lx_texture_get(int x,int y,int tile_size)
{
    int F = tile_size;

    int tx = x/F;
    int ty = y/F;

    uint8_t height = get_tile(tx,ty);

    float base = 0.1 + (frand() * 0.025f) + (height*0.01);
    int left = get_tile(tx-1,ty) - height;
    int right = get_tile(tx+1,ty) - height;
    int up = get_tile(tx,ty-1) - height;
    int bottom = get_tile(tx,ty+1) - height;

    int sx = x % F;
    int sy = y % F;

    float light = 1.0f;
    float shadow = 0.4f;
    float highlight = 1.3f;

    if (sx < 4) {
        if (left<0) {
            light=highlight;
        }
        if (left>0) {
            light=shadow;
        }
    }

    if (sx > (F-4)) {
        if (right<0) {
            light=highlight;
        }
        if (right>0) {
            light=shadow;
        }
    }

    if (sy < 4) {
        if (up<0) {
            light=highlight;
        }
        if (up>0) {
            light=shadow;
        }
    }

    if (sy > (F-4)) {
        if (bottom<0) {
            light=highlight;
        }
        if (bottom>0) {
            light=shadow;
        }
    }

    uint8_t red =  0x9f;
    uint8_t green = 0x9f;
    uint8_t blue =  0x9f;

    light = light*base;

    if (light>1.0f) {
        light = 1.0f;
    }
    if (light<0.0f) {
        light = 0.0f;
    }

    red=red*light;
    green=green*light;
    blue=blue*light;

    return (0xff000000 | red<<16 | green<<8 | blue);
}
