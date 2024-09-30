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
#include <math.h>

uint32_t* data;
int screen_width;
int screen_height;

uint32_t pixel;

void lx_raster_init(uint32_t* buffer,int width,int height, uint32_t flags)
{
    data = buffer;
    screen_width = width;
    screen_height = height;
}

void lx_raster_set_color_u32(uint32_t color)
{
    pixel = color;
}

static int min(int a,int b)
{
    return (a<b) ? a:b;
}

static int max(int a,int b)
{
    return (a>b) ? a:b;
}

void lx_raster_triangle(lx_vertex_2i* triangle)
{
    // top-left/bottom-right
    lx_vertex_2i_t top = triangle[0];
    lx_vertex_2i_t bottom = triangle[0];

    for (int n=1;n<3;n++) {
        top.x = min(top.x,triangle[n].x);
        top.y = min(top.y,triangle[n].y);

        bottom.x = max(bottom.x,triangle[n].x);
        bottom.y = max(bottom.y,triangle[n].y);
    }

    top.x = max(top.x,0);
    top.y = max(top.y,0);

    bottom.x = min(bottom.x,screen_width - 1);
    bottom.y = min(bottom.y,screen_height - 1);

    for (int j = top.y; j<=bottom.y;j++) {
        for (int i = top.x; i<=bottom.x;i++) {

        }
    }

}
