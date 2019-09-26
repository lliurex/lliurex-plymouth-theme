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
#define LX_MAX_PALETTE  32

#define LX_COLOR_BACKGROUND 0
#define LX_COLOR_TEXT 1
#define LX_COLOR_FOREGROUND_1 2
#define LX_COLOR_FOREGROUND_2 3
#define LX_COLOR_FOREGROUND_3 4

#include "log.h"
#include "text.h"
#include "cmdline.h"

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
    ply_event_loop_t* loop;
    ply_boot_splash_mode_t mode;
    
    /* seconds between frames, usually 1/fps */
    double interval;
    
    struct {
        ply_image_t* logo;
    } image;
    
    lx_screen_t screen[LX_MAX_SCREENS];
    size_t screens;
    
    double percent;
    
    uint32_t palette[LX_MAX_PALETTE];
    
    lx_font_t* font;
    lx_text_t* message;
    lx_text_t* status;
};

/*
* converts rgba to argb used by plymouth buffers
*/
static uint32_t rgba_to_argb(uint32_t color)
{
    uint32_t tmp=color>>8;
    
    tmp = tmp | ((color<<24) & 0xff000000);
    
    return tmp;
}

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
        if (plugin->screen[n].display==pixel_display) {
            screen=&plugin->screen[n];
            break;
        }
    }
    
    // could this happen?
    if (!screen) {
        lx_log_error("Invalid display callback");
        return;
    }
    
    uint32_t progress_bar_color=0;
    
    switch (plugin->mode) {
        case PLY_BOOT_SPLASH_MODE_BOOT_UP:
            progress_bar_color=plugin->palette[LX_COLOR_FOREGROUND_1];
        break;
        
        case PLY_BOOT_SPLASH_MODE_SHUTDOWN:
            progress_bar_color=plugin->palette[LX_COLOR_FOREGROUND_2];
        break;
        
        case PLY_BOOT_SPLASH_MODE_UPDATES:
            progress_bar_color=plugin->palette[LX_COLOR_FOREGROUND_3];
        break;
    }
    
    //fill brackground
    ply_rectangle_t rect;
    
    rect.x=x;
    rect.y=y;
    rect.width=width;
    rect.height=height;
    
    ply_pixel_buffer_fill_with_hex_color(pixel_buffer,
                                         &rect,
                                         plugin->palette[LX_COLOR_BACKGROUND]);
    
    //logo
    rect.width = ply_image_get_width(plugin->image.logo);
    rect.height = ply_image_get_height(plugin->image.logo);
    
    rect.x = (width/2) - (rect.width/2);
    rect.y = (height/2) - (rect.height/2);
    
    ply_pixel_buffer_t* lpx = ply_image_get_buffer(plugin->image.logo);
    ply_pixel_buffer_fill_with_buffer(pixel_buffer,lpx,rect.x,rect.y);
    
    //progress bar
    uint32_t* data = ply_pixel_buffer_get_argb32_data(pixel_buffer);
    
    int pw = plugin->percent*width;
    int ph = height-48;
    
    for (int j=0;j<8;j++) {
        for (int i=0;i<pw;i++) {
            int px = i;
            int py = ph - j;
            
            data[px+py*width] = progress_bar_color;
        }
    }
    
    //message
    if (plugin->message) {
        rect.width = ply_pixel_buffer_get_width(plugin->message->buffer);
        rect.height = ply_pixel_buffer_get_height(plugin->message->buffer);
        
        rect.x = (width/2) - (rect.width/2);
        rect.y = height*0.75;
        
        ply_pixel_buffer_fill_with_buffer(pixel_buffer,plugin->message->buffer,rect.x,rect.y);
    }
    
    //status
    if (plugin->status) {
        rect.width = ply_pixel_buffer_get_width(plugin->status->buffer);
        rect.height = ply_pixel_buffer_get_height(plugin->status->buffer);
        
        rect.x = (width/2) - (rect.width/2);
        rect.y = (height*0.75)+48;
        
        ply_pixel_buffer_fill_with_buffer(pixel_buffer,plugin->status->buffer,rect.x,rect.y);
    }
}

static void
detach_from_event_loop (ply_boot_splash_plugin_t* plugin)
{
    lx_log_debug(__PRETTY_FUNCTION__);
    plugin->loop=NULL;
}

static void
on_timeout (ply_boot_splash_plugin_t* plugin)
{
    if (plugin==NULL || plugin->loop==NULL) {
        return;
    }
    
    //update all screens!
    for (size_t n=0;n<plugin->screens;n++) {
        
        ply_pixel_display_draw_area(plugin->screen[n].display,
            0,0,
            ply_pixel_display_get_width(plugin->screen[n].display),
            ply_pixel_display_get_height(plugin->screen[n].display));
    }
    
    
    // program another timeout
    ply_event_loop_watch_for_timeout (plugin->loop,
                                plugin->interval,
                                (ply_event_loop_timeout_handler_t) on_timeout,
                                plugin);
}

/* Plugin interface functions */

static ply_boot_splash_plugin_t*
create_plugin (ply_key_file_t* key_file)
{
    lx_log_debug(__PRETTY_FUNCTION__);
    
    ply_boot_splash_plugin_t* plugin=0;
    
    plugin=calloc(1,sizeof(ply_boot_splash_plugin_t));
    
    //setup resources
    char* path;
    path=ply_key_file_get_value (key_file, "config", "path");
    
    lx_log_debug("path: %s",path);
    
    char filename[128];
    sprintf(filename,"%s/logo.png",path);
    
    plugin->image.logo=ply_image_new(filename);
    
    //default palette values
    plugin->palette[0]=0xeff0f1ff; //background
    plugin->palette[1]=0xff808080; //text
    plugin->palette[2]=0xff3daee9; //foreground 1
    plugin->palette[3]=0xffda4453; //foreground 2
    
    //load palette
    for (int n=0;n<LX_MAX_PALETTE;n++) {
        char id[4];
        sprintf(id,"p%02d",n);
        
        if (ply_key_file_has_key(key_file,"palette",id)) {
            char* value=ply_key_file_get_value (key_file, "palette",id);
            if (n>0) {
                plugin->palette[n]=rgba_to_argb(strtol(value,NULL,16));
            }
            else {
                plugin->palette[n]=strtol(value,NULL,16);
            }
        }
    }
    
    sprintf(filename,"%s/font.ttf",path);
    plugin->font=lx_font_new(filename,16,plugin->palette[LX_COLOR_TEXT]);
    
    //setup fps
    if (ply_key_file_has_key(key_file,"config","fps")) {
        char* value=ply_key_file_get_value (key_file, "config", "fps");
        long int fps=strtol(value,NULL,10);
        
        if (fps<0 || fps>500) {
            lx_log_error("FPS out of range:%d",fps);
            fps=60;
        }
        
        plugin->interval=1.0/fps;
        lx_log_info("FPS:%d",fps);
    }
    else {
        plugin->interval=1.0/30.0;
        lx_log_info("Using default FPS: 30");
    }
    
    size_t num_options;
    char** options = lx_cmdline_get(&num_options);
    
    for (size_t n=0;n<num_options;n++) {
        lx_log_debug("cmdline option %s",options[n]);
        free(options[n]);
    }
    
    free(options);
    
    return plugin;
}

static void
destroy_plugin (ply_boot_splash_plugin_t* plugin)
{
    lx_log_debug(__PRETTY_FUNCTION__);
    
    lx_font_delete(plugin->font);
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
            if (plugin->screen[n].display==NULL) {
                plugin->screen[n].display=display;
                plugin->screen[n].id=id;
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
        if (plugin->screen[n].display==display) {
            plugin->screen[n].display=NULL;
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
    
    plugin->loop=loop;
    plugin->mode=mode;
    
    /* load resources */
    if (!ply_image_load(plugin->image.logo)) {
        lx_log_error("Failed to load logo image");
        return false;
    }
    //exit callback
    ply_event_loop_watch_for_exit (loop, (ply_event_loop_exit_handler_t)
                                       detach_from_event_loop,
                                       plugin);
    
    //setup a timeout
    ply_event_loop_watch_for_timeout (loop,
                                          1.0 / 30.0,
                                          (ply_event_loop_timeout_handler_t)
                                          on_timeout, plugin);
    return true;
}

static void
system_update (ply_boot_splash_plugin_t *plugin,
              int progress)
{
    lx_log_debug(__PRETTY_FUNCTION__);
    lx_log_debug("updating: %d",progress);
}

static void
update_status (ply_boot_splash_plugin_t* plugin,
               const char* status)
{
    lx_log_debug(__PRETTY_FUNCTION__);
    lx_log_debug(status);
    
    if (plugin->status) {
        lx_text_delete(plugin->status);
        plugin->status=NULL;
    }
    
    char* cpy=strdup(status);
    char* processed=cpy;
    
    for (int n=0;n<strlen(cpy)-1;n++) {
        if (cpy[n]==':') {
            processed=&cpy[n+1];
        }
    }
    
    //ignore default messages
    if (processed!=cpy) {
        plugin->status=lx_text_new(plugin->font,processed);
    }
    
    free(cpy);
}

static void
on_boot_output (ply_boot_splash_plugin_t *plugin,
               const char* output,
               size_t size)
{
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
    lx_log_info("root mounted event");
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
    
    if (plugin->message) {
        lx_text_delete(plugin->message);
        plugin->message=NULL;
    }
    
    plugin->message=lx_text_new(plugin->font,message);
    
    lx_log_debug("message:%s",message);
}

static void
hide_message (ply_boot_splash_plugin_t* plugin,
              const char* message)
{
    lx_log_debug(__PRETTY_FUNCTION__);
    
    lx_text_delete(plugin->message);
    plugin->message=NULL;
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
            .system_update        = system_update,
            .update_status        = update_status,
            .on_boot_output       = on_boot_output,
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