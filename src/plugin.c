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

#define LX_MAX_SCREENS 4

#include "log.h"

#include <ply-boot-splash-plugin.h>
#include <ply-logger.h>
#include <ply-image.h>
#include <ply-label.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*
 * Screen
 */
typedef struct {
    uint32_t id;
    ply_pixel_display_t* display;
} lx_screen_t;

/*
 * Plugin structure
*/
struct _ply_boot_splash_plugin 
{
    struct image {
        ply_image_t* logo;
    };
    
    lx_screen_t screen[LX_MAX_SCREENS];
    size_t screens;
    
    double percent;
    
    bool visible;
    bool running;
    
    struct color {
        uint32_t background;
        uint32_t foreground;
    };
};

/* Plugin callbacks */

static void on_draw (void* user_data,
    ply_pixel_buffer_t* pixel_buffer,
    int x,
    int y,
    int width,
    int height,
    ply_pixel_display_t* pixel_display)
{
    ply_boot_splash_plugin_t* plugin = (ply_boot_splash_plugin_t*)user_data;
    lx_screen_t* screen=NULL;
    
    for (size_t n=0;n<plugin->screens;n++) {
        if (plugin->screen[n]->display==pixel_display) {
            screen=&plugin->screen[n];
            break;
        }
    }
    
    // could this happen?
    if (!screen) {
        lx_log_error("Invalid display callback");
        return;
    }
    
    //fill brackground
    ply_pixel_buffer_fill_with_hex_color(pixel_buffer,
                                         &rect,plugin->color.background);
    
    //logo
    int lx,ly,lw,lh;
    
    lw = ply_image_get_width(plugin->image.logo);
    lh = ply_image_get_height(plugin->image.logo);
    
    lx = (width/2) - (lw/2);
    ly = (height/2) - (lh/2);
    
    ply_pixel_buffer_t* lpx = ply_image_get_buffer(plugin->image.logo);
    ply_pixel_buffer_fill_with_buffer(pixel_buffer,lpx,lx,ly);
    
    //progress bar
    uint32_t* data = ply_pixel_buffer_get_argb32_data(pixel_buffer);
    
    int pw = plugin->percent*width;
    int ph = (height*0.9);
    
    for (int j=0;j<8;j++) {
        for (int i=0;i<pw;i++) {
            int px = i;
            int py = ph - j;
            
            data[px+py*width] = plugin->color.foreground;
        }
    }
}

/* Plugin interface functions */

static ply_boot_splash_plugin_t*
create_plugin (ply_key_file_t* key_file)
{
    lx_log_debug(__PRETTY_FUNCTION__);
    
    ply_boot_splash_plugin_t* plugin=0;
    
    plugin=calloc(1,sizeof(ply_boot_splash_plugin_t));
    
    /* setup resources */
    
    char* path;
    path=ply_key_file_get_value (key_file, "config", "path");
    
    lx_log_debug("path: %s",path);
    
    char filename[128];
    sprintf(filename,"%s/logo.png");
    
    plugin->image.logo=ply_image_new(filename);
    
    /* setup colors */
    
    if (ply_key_file_has_key(key_file,"palette","background")) {
        char* value=ply_key_file_get_value (key_file, "config", "background");
        plugin->color.background=strtol(value,NULL,16);
    }
    else {
        plugin->color.background=0xeff0f1ff;
    }
    
    if (ply_key_file_has_key(key_file,"palette","foreground")) {
        char* value=ply_key_file_get_value (key_file, "config", "foreground");
        plugin->color.foreground=strtol(value,NULL,16);
    }
    else {
        plugin->color.foreground=0xff3daee9;
    }
    
    return plugin;
}

static void
destroy_plugin (ply_boot_splash_plugin_t* plugin)
{
    lx_log_debug(__PRETTY_FUNCTION__);
    
    free(plugin);
}

static void
set_keyboard (ply_boot_splash_plugin_t* plugin,
              ply_keyboard_t* keyboard)
{
    lx_log_debug(__PRETTY_FUNCTION__);
}

static void
unset_keyboard (ply_boot_splash_plugin_t* plugin,
                ply_keyboard_t* keyboard)
{
    lx_log_debug(__PRETTY_FUNCTION__);
}

static void
add_pixel_display (ply_boot_splash_plugin_t* plugin,
                   ply_pixel_display_t* display)
{
    lx_log_debug(__PRETTY_FUNCTION__);
    
    static uint32_t id=0;
    
    if (plugin->screens<=LX_MAX_SCREENS) {
        for (size_t n=0;n<LX_MAX_SCREENS;n++) {
            if (plugin->screen[n]->display==NULL) {
                plugin->screen[n]->display=display;
                plugin->screen[n]->id=id;
                id++;
                
                ply_pixel_display_set_draw_handler (display,
                                    (ply_pixel_display_draw_handler_t)on_draw,
                                    plugin);
                break;
            }
        }
        plugin->screens++;
    }
    else {
        lx_log_info("Ignoring new pixel display");
    }
}

static void
remove_pixel_display (ply_boot_splash_plugin_t* plugin,
                      ply_pixel_display_t* display)
{
    lx_log_debug(__PRETTY_FUNCTION__);
    
    for (size_t n=0;n<LX_MAX_SCREENS;n++) {
        if (plugin->screen[n]->display==display) {
            plugin->screen[n]->display=NULL;
            plugin->screens--;
        }
    }
}

static bool
show_splash_screen (ply_boot_splash_plugin_t* plugin,
                    ply_event_loop_t* loop,
                    ply_buffer_t* boot_buffer,
                    ply_boot_splash_mode_t mode)
{
    lx_log_debug(__PRETTY_FUNCTION__);
    
    /* load resources */
    if (!ply_image_load(plugin->image.logo)) {
        lx_log_error("Failed to load logo image");
        return false;
    }
    
    return true;
}

static void
update_status (ply_boot_splash_plugin_t* plugin,
               const char* status)
{
    lx_log_debug(__PRETTY_FUNCTION__);
    lx_log_info(status);
}

static void
on_boot_progress (ply_boot_splash_plugin_t* plugin,
                  double duration,
                  double percent_done)
{
    plugin->percent=percent_done;
}

static void
hide_splash_screen (ply_boot_splash_plugin_t* plugin,
                    ply_event_loop_t* loop)
{
    lx_log_debug(__PRETTY_FUNCTION__);
}

static void
on_root_mounted (ply_boot_splash_plugin_t* plugin)
{
    lx_log_debug(__PRETTY_FUNCTION__);
}

static void
become_idle (ply_boot_splash_plugin_t* plugin,
             ply_trigger_t* idle_trigger)
{
    lx_log_debug(__PRETTY_FUNCTION__);
    ply_trigger_pull (idle_trigger, NULL);
}

static void
display_normal (ply_boot_splash_plugin_t* plugin)
{
    lx_log_debug(__PRETTY_FUNCTION__);
}

static void
display_password (ply_boot_splash_plugin_t* plugin,
                  const char* prompt,
                  int bullets)
{
    lx_log_debug(__PRETTY_FUNCTION__);
}

static void
display_question (ply_boot_splash_plugin_t* plugin,
                  const char* prompt,
                  const char* entry_text)
{
    lx_log_debug(__PRETTY_FUNCTION__);
}

static void
display_message (ply_boot_splash_plugin_t* plugin,
                 const char* message)
{
    lx_log_debug(__PRETTY_FUNCTION__);
}

static void
hide_message (ply_boot_splash_plugin_t* plugin,
              const char* message)
{
    lx_log_debug(__PRETTY_FUNCTION__);
}

/*
 * Get plugin function table
*/
ply_boot_splash_plugin_interface_t *
ply_boot_splash_plugin_get_interface (void)
{
    lx_log_debug(__PRETTY_FUNCTION__);
    
    static ply_boot_splash_plugin_interface_t plugin_interface =
    {
            .create_plugin        = create_plugin,
            .destroy_plugin       = destroy_plugin,
            .set_keyboard         = set_keyboard,
            .unset_keyboard       = unset_keyboard,
            .add_pixel_display    = add_pixel_display,
            .remove_pixel_display = remove_pixel_display,
            .show_splash_screen   = show_splash_screen,
            .update_status        = update_status,
            .on_boot_progress     = on_boot_progress,
            .hide_splash_screen   = hide_splash_screen,
            .on_root_mounted      = on_root_mounted,
            .become_idle          = become_idle,
            .display_normal       = display_normal,
            .display_password     = display_password,
            .display_question     = display_question,
            .display_message      = display_message,
            .hide_message         = hide_message,
    };

    return &plugin_interface;
}