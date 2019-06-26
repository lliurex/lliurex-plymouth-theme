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

#include "text.h"
#include "font8x8_basic.h"

#include <stdlib.h>
#include <string.h>

lx_text_t* lx_text_new(const char* str,uint32_t color)
{
    lx_text_t* text=0;
    
    text=calloc(1,sizeof(lx_text_t));
    text->str=strdup(str);
    
    int width=strlen(str)*16;
    int height=16;
    
    text->buffer=ply_pixel_buffer_new(width,height);
    uint32_t* data = ply_pixel_buffer_get_argb32_data(text->buffer);
    
    int x=0;
    
    for (int n=0;n<strlen(str);n++) {
        char c=str[n];
        if (c>127) {
            c=0;
        }
        
        char* glyph = font8x8_basic[c];
        
        for (int j=0;j<16;j++) {
            char row=glyph[j/2];
            
            for (int i=0;i<16;i++) {
                char value = (row>>(i/2)) & 1;
                if (value==1) {
                    data[(x+i)+j*width]=color;
                }
                else {
                    data[(x+i)+j*width]=0x00000000;
                }
            }
        }
        
        x+=16;
    }
    
    return text;
}

void lx_text_delete(lx_text_t* text)
{
    if (text) {
        free(text->str);
        ply_pixel_buffer_free(text->buffer);
        free(text);
    }
}
