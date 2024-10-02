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

#include "raster.h"

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

void lx_raster_set_color_4f(float r,float g, float b, float a)
{
    uint32_t ir = r * 255;
    uint32_t ig = g * 255;
    uint32_t ib = b * 255;
    uint32_t ia = a * 255;

    ig = ig << 8;
    ir = ir << 16;
    ia = ia << 24;

    pixel = ia | ib | ig | ir;
}

static int min(int a,int b)
{
    return (a<b) ? a:b;
}

static int max(int a,int b)
{
    return (a>b) ? a:b;
}

static int32_t orient(int32_t x0, int32_t y0, int32_t x1,int32_t y1, int32_t x2, int32_t y2)
{
    return (x1-x0)*(y2-y0) - (y1-y0)*(x2-x0);
}

void lx_raster_triangle(lx_vertex_2i_t* triangle)
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

    for (int32_t j = top.y; j<=bottom.y;j++) {
        for (int32_t i = top.x; i<=bottom.x;i++) {
            int32_t a1 = orient(triangle[1].x,triangle[1].y,triangle[2].x,triangle[2].y,i,j);
            int32_t a2 = orient(triangle[2].x,triangle[2].y,triangle[0].x,triangle[0].y,i,j);
            int32_t a3 = orient(triangle[0].x,triangle[0].y,triangle[1].x,triangle[1].y,i,j);

            if (a1>=0 && a2>=0 && a3>=0) {
                data[i+j*screen_width] = pixel;
                continue;
            }

            if (a1<0 && a2<0 && a3<0) {
                data[i+j*screen_width] = pixel;
            }

        }
    }
}

/*
 * credits: https://gist.github.com/bert/1085538
 */
void lx_raster_line(lx_vertex_2i_t* line)
{
    int x0 = line[0].x;
    int y0 = line[0].y;
    int x1 = line[1].x;
    int y1 = line[1].y;

    int dx =  abs (x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs (y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2; /* error value e_xy */

    while (1) {
        if (x0 >= 0 && y0 >=0 && x0<(screen_width-1) && y0<(screen_height-1)) {
            data[x0+y0*screen_width] = pixel;
        }

        if (x0 == x1 && y0 == y1) {
            break;
        }

        e2 = 2 * err;

        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        } /* e_xy+e_x > 0 */

        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        } /* e_xy+e_y < 0 */
    }
}
