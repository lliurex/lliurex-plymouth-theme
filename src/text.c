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
#include "log.h"

#include <stdlib.h>
#include <string.h>

#define LX_TEXT_WS 8
#define LX_TEXT_W 16
#define LX_TEXT_H 16
#define LX_TEXT_INTER 1

typedef struct {
    uint8_t* data;
    int left;
    int right;
    int width;
} lx_glyph_t;

struct {
    lx_glyph_t glyph[128];
} cache;

static void update_glyph(int u)
{
    char* bitmap = font8x8_basic[u];
    
    cache.glyph[u].left=LX_TEXT_W;
    cache.glyph[u].right=0;
    
    for (int j=0;j<LX_TEXT_H;j++) {
        char row=bitmap[j/2];
        
        for (int i=0;i<LX_TEXT_W;i++) {
            char value = (row>>(i/2)) & 1;
            uint8_t pixel;
            
            if (value==1) {
                pixel=0xff;
                
                if (cache.glyph[u].left>i) {
                    cache.glyph[u].left=i;
                }
                
                if (cache.glyph[u].right<i) {
                    cache.glyph[u].right=i;
                }
            }
            else {
                pixel=0x00;
            }
            
            cache.glyph[u].data[i+j*LX_TEXT_W]=pixel;
        }
    }
    
    if (cache.glyph[u].left==LX_TEXT_W) {
        cache.glyph[u].left=0;
        cache.glyph[u].width=LX_TEXT_W;
    }
    else {
        cache.glyph[u].width=1+(cache.glyph[u].right-cache.glyph[u].left);
    }
    
}

static int update_cache(const char* str)
{
    int rwidth=0;
    for (int n=0;n<strlen(str);n++) {
        int u=str[n];
        
        if (cache.glyph[u].data==NULL) {
            cache.glyph[u].data=malloc(LX_TEXT_W*LX_TEXT_H);
            update_glyph(u);
        }
        
        rwidth+=cache.glyph[u].width;
        
    }
    
    return rwidth;
}

lx_text_t* lx_text_new(const char* str,uint32_t color)
{
    lx_text_t* text=0;
    
    text=calloc(1,sizeof(lx_text_t));
    text->str=strdup(str);
    
    int width = update_cache(str);
    width=width+((strlen(str)+1)*LX_TEXT_INTER);
    
    int height=LX_TEXT_H;
    
    color = color & 0x00ffffff;
    
    text->buffer=ply_pixel_buffer_new(width,height);
    uint32_t* data = ply_pixel_buffer_get_argb32_data(text->buffer);
    
    int x=0;
    
    for (int n=0;n<strlen(str);n++) {
        int u=str[n];
        
        if (u>127) {
            u=0;
        }
        
        if (u==' ') {
            x+=LX_TEXT_WS;
            continue;
        }
        
        int rwidth = cache.glyph[u].width;
        
        x++;
        int start = cache.glyph[u].left;
        int end = cache.glyph[u].right;
        
        for (int j=0;j<LX_TEXT_H;j++) {
            for (int i=start,ii=0;i<=end;i++,ii++) {
                uint32_t alpha = cache.glyph[u].data[i+j*LX_TEXT_W] << 24;
                data[(x+ii)+j*width]=(color | alpha);
            }
        }
        x+=rwidth;
        
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

void lx_text_free_cache()
{
    for (int n=0;n<128;n++) {
        if (cache.glyph[n].data!=NULL) {
            free(cache.glyph[n].data);
        }
    }
}