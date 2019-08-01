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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char** lx_cmdline_get(size_t* options)
{
    
    char* buffer = calloc(1,4096);
    size_t len;
    
    FILE* f=fopen("/proc/cmdline","r");
    len=fread(buffer,4096,1,f);
    fclose(f);
    
    size_t max_options=8;
    *options=0;
    char** words=calloc(sizeof(char*),max_options);
    char* word = strtok(buffer," ");
    
    while (word!=NULL) {
        
        words[*options]=strdup(word);
        (*options)++;
        
        if (*options == max_options) {
            max_options+=8;
            char** tmp = realloc(words, sizeof(char*)*max_options);
            
            if (tmp) {
                words = tmp;
            }
        }
        word = strtok(NULL," ");
    }
    
    free(buffer);

    return words;
}