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

#include "i18.h"

#include <string.h>
#include <stdlib.h>

char _lang[32];

lx_message_t messages[]={
    {0,"Please remove the installation medium, then press ENTER: "},
    {1,"Message 1"},
    {2,"Message 2"},
    {0xff,""}
};

lx_translation_t translations[]={
    {0,"es","Por favor, saque el medio de instalacion y presione INTRO"},
    {1,"es","Mensaje 1"},
    {2,"es","Mensaje 2"},
    {0xff,"",""}
};

void lx_i18_set_lang(const char* lang)
{
    strncpy(_lang,lang,32);
}

const char* lx_i18(const char* message)
{
    lx_message_t* found=NULL;
    lx_message_t* msg=messages;
    
    while(msg->id!=0xff) {
        if (strcmp(message,msg->message)==0) {
            found=msg;
            break;
        }
        msg++;
    }
    
    //message not found
    if (!found) {
        return message;
    }
    
    lx_translation_t* q=NULL;
    lx_translation_t* t=translations;
    
    while (t->id!=0xff) {
        if (t->id==found->id) {
            if (strcmp(_lang,t->lang)==0) {
                q=t;
                break;
            }
        }
        t++;
    }
    
    //no suitable translation found
    if (!q) {
        return message;
    }
    
    return q->message;
}