/*
    lliurex plymouth theme
    Copyright (C) 2024  Enrique Medina Gremaldos <quique@necos.es>

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

#include <stdlib.h>
#include <stdint.h>

typedef struct lx_vertex_2i
{
    int32_t x;
    int32_t y;
} lx_vertex_2i_t;

typedef struct lx_color
{
    float data[4];
} lx_color_t;

extern void lx_raster_init(uint32_t* buffer,int width,int height, uint32_t flags);
extern void lx_raster_set_color_u32(uint32_t color);
extern void lx_raster_set_color_4f(float r,float g, float b, float a);
extern void lx_raster_triangle(lx_vertex_2i_t* triangle);
extern void lx_raster_line(lx_vertex_2i_t* line);
extern void lx_raster_generate_noise();
