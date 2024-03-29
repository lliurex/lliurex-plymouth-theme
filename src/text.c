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
#include "log.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

#define LX_TEXT_WS 8
#define LX_TEXT_W 18
#define LX_TEXT_H 16
#define LX_TEXT_INTER 1

FT_Library library;
int ft_counter=0;

static uint16_t* create_unicode(const char* str)
{
    uint16_t* ustr = calloc(strlen(str)*2,sizeof(uint16_t));
    int m=0;
    uint16_t ucode=0;
    
    for (int n=0;n<strlen(str);n++) {
        ucode = 0;
        uint8_t a = str[n];
        if (a<=0x7f) {
            ucode = a;
        }
        else {
            n++;
            uint8_t b = str[n];
            
            if ( ((b&0xc0)==0x80) && ((a&0xe0)==0xc0) ) {
                ucode = b & 0x3f;
                uint16_t hi = (a & 0x1f)<<6;
                
                ucode = ucode | hi;
            }
            else {
                lx_log_error("bad utf-8 format %x%x",a,b);
                ucode = 0;
            }
        }
        
        ustr[m] = ucode;
        m++;
    }
    
    return ustr;
}

lx_font_t* lx_font_new(const char* path,int px,uint32_t color)
{
    lx_font_t* font;
    FT_Error error;
    
    if (ft_counter==0) {
        //ToDo: check error
        error=FT_Init_FreeType(&library);
        lx_log_debug("Loading Freetype:%d",error);
    }
    
    ft_counter++;
    
    font = calloc(1,sizeof(lx_font_t));
    font->path=strdup(path);
    font->px=px;
    font->space=px/2;
    font->color=color;// & 0x00ffffff; //clear alpha
    float red = (color & 0x00ff0000) >> 16;
    float green = (color & 0x0000ff00) >> 8;
    float blue = (color & 0x000000ff);
    
    for (int n=0;n<512;n++) {
        memset(&font->glyph[n],0,sizeof(lx_glyph_t));
    }
    
    error=FT_New_Face(library,path,0,&font->face);
    lx_log_debug("Loading face: %d",error);
    
    error=FT_Set_Pixel_Sizes(font->face,0,px);
    lx_log_debug("Set size: %d",error);

    for (int n=32;n<512;n++) {
        lx_log_debug("loading %d",n);
        //error=FT_Load_Char(font->face,n,FT_LOAD_RENDER);
        FT_UInt index;
        
        index=FT_Get_Char_Index(font->face,n);
        lx_log_debug("loaded:%d",index);
        
        if (index==0) {
            lx_log_info("Ignoring char %d",n);
            continue;
        }
        
        error = FT_Load_Glyph(font->face,index,0);
        
        if (error!=0) {
            lx_log_debug("Failed to load glyph");
        }
        
        FT_GlyphSlot glyph = font->face->glyph;
        
        error = FT_Render_Glyph(glyph,FT_RENDER_MODE_NORMAL);
        
        if (error!=0) {
            lx_log_debug("Failed to render 0");
            continue;
        }
        
        if (!glyph->bitmap.buffer) {
            lx_log_debug("Failed to render 1");
            continue;
        }
        
        font->glyph[n].code=n;
        font->glyph[n].width=glyph->bitmap.width;
        font->glyph[n].height=glyph->bitmap.rows;
        font->glyph[n].left=glyph->bitmap_left;
        font->glyph[n].top=glyph->bitmap_top;
        
        lx_log_debug("char %d w:%d h:%d l:%d t:%d",font->glyph[n].code,
            font->glyph[n].width,
            font->glyph[n].height,
            font->glyph[n].left,
            font->glyph[n].top
        );
        
        font->glyph[n].buffer=ply_pixel_buffer_new(font->glyph[n].width,font->glyph[n].height);

        uint32_t* data = ply_pixel_buffer_get_argb32_data(font->glyph[n].buffer);
        
        uint32_t width =font->glyph[n].width;
        uint32_t height =font->glyph[n].height;
        
        for (int j=0;j<height;j++) {
            for (int i=0;i<width;i++) {
                float alpha = glyph->bitmap.buffer[i+j*width]/255.0f;

                uint32_t pixel;
                uint8_t r = red * alpha;
                uint8_t g = green * alpha;
                uint8_t b = blue * alpha;
                uint8_t a = alpha * 255;

                pixel = (a<<24) | (r<<16) | (g<<8) | b;

                data[i+j*width] = pixel;
            }
        }

    }

    return font;
}

void lx_font_delete(lx_font_t* font)
{
    for (int n=32;n<512;n++) {
        if (font->glyph[n].buffer) {
            ply_pixel_buffer_free(font->glyph[n].buffer);
        }
    }
    
    free(font->path);
    free(font);
    
    ft_counter--;
    
    if (ft_counter==0) {
        FT_Done_FreeType(library);
    }
}

lx_text_t* lx_text_new(lx_font_t* font,const char* str)
{
    lx_text_t* text=0;
    
    text=calloc(1,sizeof(lx_text_t));
    text->str=strdup(str);
    text->font=font;
    text->ustr=create_unicode(str);
    
    int width=0;
    int height=0;
    int top=-100;
    int bottom=100;
    
    lx_log_debug("text:%s,%d",str,strlen(str));
    
    uint16_t* uc=text->ustr;
    while (*uc!=0) {
        uint32_t code = 0x1ff & *uc;
        uc++;
        
        lx_glyph_t* glyph = &font->glyph[code];
        
        if (code==0x20) {
            width+=font->space;
            continue;
        }
        
        if (glyph->code==0) {
            lx_log_debug("char not found:%d",code);
            continue;
        }
        
        width+=glyph->width;
        
        if (glyph->top>top) {
            top = glyph->top;
        }
        
        if ((glyph->top-glyph->height)<bottom) {
            bottom=glyph->top-glyph->height;
        }
    }
    
    int p=bottom-top;
    p=p*p;
    height = sqrtf(p);
    
    width=width+((strlen(str)+1)*LX_TEXT_INTER);
    lx_log_debug("text size in pixels %dx%d",width,height);
    
    text->buffer=ply_pixel_buffer_new(width,height);
    uint32_t* data = ply_pixel_buffer_get_argb32_data(text->buffer);
    
    int x=LX_TEXT_INTER;
    int y=0;
    int baseline = height+bottom;
    
    lx_log_debug("top bottom baseline %d %d %d",top,bottom,baseline);
    
    uc=text->ustr;
    while (*uc!=0) {
        uint32_t code = 0x1ff & *uc;
        uc++;
        
        lx_glyph_t* glyph = &font->glyph[code];
        
        if (code==0x20 || glyph->code==0) {
            x+=font->space;
            continue;
        }
        
        int dif=glyph->height-glyph->top;
        ply_pixel_buffer_fill_with_buffer(text->buffer,glyph->buffer,x,baseline-glyph->top);
        x+=glyph->width+LX_TEXT_INTER;
        
        //x+=rwidth;
        
    }
    
    return text;
}

void lx_text_delete(lx_text_t* text)
{
    if (text) {
        free(text->str);
        free(text->ustr);
        ply_pixel_buffer_free(text->buffer);
        free(text);
    }
}

void lx_text_print(ply_pixel_buffer_t* buffer,lx_font_t* font,int ox,int oy,char* str)
{
    uint16_t* ustr = create_unicode(str);
    uint16_t* uc = ustr;
    int x = ox;
    int y = oy;

    while (*uc!=0) {
        uint32_t code = 0x1ff & *uc;
        uc++;

        lx_glyph_t* glyph = &font->glyph[code];

        if (code==0x20 || glyph->code==0) {
            x+=font->space;
            continue;
        }

        int dif=glyph->height-glyph->top;
        ply_pixel_buffer_fill_with_buffer(buffer,glyph->buffer,x,y-glyph->top);
        x+=glyph->width+LX_TEXT_INTER;
    }

}
