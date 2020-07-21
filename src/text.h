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
#include <ft2build.h>
#include FT_FREETYPE_H
#include <stdint.h>

typedef struct {
    
    uint32_t code;
    int32_t top;
    int32_t left;
    int32_t width;
    int32_t height;
    ply_pixel_buffer_t* buffer;
    
} lx_glyph_t;

typedef struct {
    FT_Face face;
    char* path;
    int32_t px;
    uint32_t color;
    int32_t space;
    lx_glyph_t glyph[512];
    
} lx_font_t;

/*!
 * Creates a new font structure
 * \param path path to a freetype supported font file (ttf,otf,...)
 * \param px desired font height in pixels
 * \return pointer to new lx_font_t
*/
lx_font_t* lx_font_new(const char* path,int32_t px,uint32_t color);

/*!
 * Destroys a font
 * \param font pointer to font
*/
void lx_font_delete(lx_font_t* font);

typedef struct {
    lx_font_t* font;
    ply_pixel_buffer_t* buffer;
    char* str;
    uint16_t* ustr;
} lx_text_t;

/*!
 * Creates a new text surface
 * \param font pointer to already loaded font
 * \param str C string with text
 * \param color argb text color
 * \return pointer to new lx_text_t
*/
lx_text_t* lx_text_new(lx_font_t* font,const char* str);

/*!
 * Destroys text
 * \param text pointer to text
*/
void lx_text_delete(lx_text_t* text);

#endif