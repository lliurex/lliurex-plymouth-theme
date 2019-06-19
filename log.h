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

#ifndef LX_PLYMOUTH_LOG
#define LX_PLYMOUTH_LOG

#ifdef NDEBUG
/*
 * Push debug messages to system journal
 * only defined when compiled with debug mode
*/
void lx_log_debug(char* format,...);
#else
#define lx_log_debug(format,...) ((void)0)
#endif

/*
 * Push info message to system journal
*/
void lx_log_info(const char* format,...);

/*
 * Push error messages to system journal
*/
void lx_log_error(const char* format,...);

#endif