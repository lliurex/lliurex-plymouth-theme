/*
    lliurex plymouth theme
    Copyright (C) 2019  Enrique Medina Gremaldos <quiqueiii@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef LX_PLYMOUTH_TEXT
#define LX_PLYMOUTH_TEXT

#include <ply-pixel-buffer.h>

#include <stdint.h>

typedef struct {
    ply_pixel_buffer_t* buffer;
    char* str;
} lx_text_t;

lx_text_t* lx_text_new(const char* str,uint32_t color);

void lx_text_delete(lx_text_t* text);

#endif