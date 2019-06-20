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

#include "log.h"

#include <systemd/sd-journal.h>

#ifdef NDEBUG
void lx_log_debug(char* format,...)
{
    va_list arg;
    va_start(arg, format);
    sd_journal_printv(LOG_DEBUG,format,arg);
    va_end(arg);
}
#endif

void lx_log_info(const char* format,...)
{
    va_list arg;
    va_start(arg, format);
    sd_journal_printv(LOG_INFO,format,arg);
    va_end(arg);
}

void lx_log_error(const char* format,...)
{
    va_list arg;
    va_start(arg, format);
    sd_journal_printv(LOG_ERR,format,arg);
    va_end(arg);
}