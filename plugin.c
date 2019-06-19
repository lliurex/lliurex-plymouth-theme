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

#include <ply-boot-splash-plugin.h>
#include <ply-logger.h>
#include <ply-image.h>
#include <ply-label.h>

#include <systemd/sd-journal.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef NDEBUG
/*
 * Push debug messages to system journal
 * only defined when compiled with debug mode
*/
static void log_debug(char* format,...)
{
    va_list arg;
    va_start(arg, format);
    sd_journal_printv(LOG_DEBUG,format,arg);
    va_end(arg);
}
#else
#define log_debug(format,...) ((void)0)
#endif

/*
 * Push info messaged to system journal
*/
static void log_info(const char* format,...)
{
    va_list arg;
    va_start(arg, format);
    sd_journal_printv(LOG_INFO,format,arg);
    va_end(arg);
}

/*
 * Push error messages to system journal
*/
static void log_error(const char* format,...)
{
    va_list arg;
    va_start(arg, format);
    sd_journal_printv(LOG_ERR,format,arg);
    va_end(arg);
}

/*
 * Plugin structure
*/
struct _ply_boot_splash_plugin 
{
};

static ply_boot_splash_plugin_t*
create_plugin (ply_key_file_t* key_file)
{
    log_debug(__PRETTY_FUNCTION__);
}

static void
destroy_plugin (ply_boot_splash_plugin_t* plugin)
{
    log_debug(__PRETTY_FUNCTION__);
}

static void
set_keyboard (ply_boot_splash_plugin_t* plugin,
              ply_keyboard_t* keyboard)
{
    log_debug(__PRETTY_FUNCTION__);
}

static void
unset_keyboard (ply_boot_splash_plugin_t* plugin,
                ply_keyboard_t* keyboard)
{
    log_debug(__PRETTY_FUNCTION__);
}

static void
add_pixel_display (ply_boot_splash_plugin_t* plugin,
                   ply_pixel_display_t* display)
{
    log_debug(__PRETTY_FUNCTION__);
}

static void
remove_pixel_display (ply_boot_splash_plugin_t* plugin,
                      ply_pixel_display_t* display)
{
    log_debug(__PRETTY_FUNCTION__);
}

static bool
show_splash_screen (ply_boot_splash_plugin_t* plugin,
                    ply_event_loop_t* loop,
                    ply_buffer_t* boot_buffer,
                    ply_boot_splash_mode_t mode)
{
    log_debug(__PRETTY_FUNCTION__);
}

static void
update_status (ply_boot_splash_plugin_t* plugin,
               const char* status)
{
    log_debug(__PRETTY_FUNCTION__);
    log_info(status);
}

static void
on_boot_progress (ply_boot_splash_plugin_t* plugin,
                  double duration,
                  double percent_done)
{
}

static void
hide_splash_screen (ply_boot_splash_plugin_t* plugin,
                    ply_event_loop_t* loop)
{
    log_debug(__PRETTY_FUNCTION__);
}

static void
on_root_mounted (ply_boot_splash_plugin_t* plugin)
{
    log_debug(__PRETTY_FUNCTION__);
}

static void
become_idle (ply_boot_splash_plugin_t* plugin,
             ply_trigger_t* idle_trigger)
{
    log_debug(__PRETTY_FUNCTION__);
    ply_trigger_pull (idle_trigger, NULL);
}

static void
display_normal (ply_boot_splash_plugin_t* plugin)
{
    log_debug(__PRETTY_FUNCTION__);
}

static void
display_password (ply_boot_splash_plugin_t* plugin,
                  const char* prompt,
                  int bullets)
{
    log_debug(__PRETTY_FUNCTION__);
}

static void
display_question (ply_boot_splash_plugin_t* plugin,
                  const char* prompt,
                  const char* entry_text)
{
    log_debug(__PRETTY_FUNCTION__);
}

static void
display_message (ply_boot_splash_plugin_t* plugin,
                 const char* message)
{
    log_debug(__PRETTY_FUNCTION__);
}

static void
hide_message (ply_boot_splash_plugin_t* plugin,
              const char* message)
{
    log_debug(__PRETTY_FUNCTION__);
}

/*
 * Get plugin function table
*/
ply_boot_splash_plugin_interface_t *
ply_boot_splash_plugin_get_interface (void)
{
    log_debug(__PRETTY_FUNCTION__);
    
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