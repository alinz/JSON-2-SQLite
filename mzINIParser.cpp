/*
	JSON2SQLite v0.1
    Copyright (C) 2011  Ali Najafizadeh

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
#include "INIParser.h"
#include <stdio.h>
#include <string>

enum State
{
    START,
    CAT_OPEN,
    CAT_CLOSE,
    KEY_OPEN,
    KEY_CLOSE,
    VALUE_OPEN,
    VALUE_CLOSE
};

class Handle
{
public:
    FILE* mp_file;
    State m_state;
    
    ~Handle()
    {
        if(mp_file) fclose(mp_file);
    }
};

mz::TableInfo::TableInfo( const char* filename )
{
    mp_handle = NULL;
    Handle* tmp = new Handle();
    tmp->mp_file = NULL;
    tmp->m_state = START;
    tmp->mp_file = fopen(filename, "r");
    
    if( tmp->mp_file == NULL ) return;
    
    mp_handle = tmp;
}

mz::TableInfo::~TableInfo()
{
    if( mp_handle == NULL ) return;
    
    Handle* handle = (Handle*) mp_handle;
    delete handle;
}

mz::TableInfo::TableNode* mz::TableInfo::GetNextTable()
{
    if( mp_handle == NULL ) return NULL;
    
    Handle* handle = (Handle*)mp_handle;
    
    TableNode* tableNode = new TableNode();
    FieldNode* lastNode = NULL;
    
    char ch;
    char buffer[255];
    int i = 0;
    bool one = false;
    while((ch = fgetc(handle->mp_file)) != EOF)
    {
        if(ch == '[')
        {
            handle->m_state = CAT_OPEN;
            if(one) 
            {
                ungetc('[', handle->mp_file);
                break;
            }
            one = true;
            continue;
        }
        else if(ch == ']')
        {
            handle->m_state  = CAT_CLOSE;
            buffer[i] = '\0';
            
            tableNode->mp_tableName = new char[i+1];
            memcpy(tableNode->mp_tableName, buffer, i+1); 
            
            i = 0;
            continue;
        }
        else if(handle->m_state  == CAT_CLOSE && ch == 10)
        {
            continue;
        }
        else if(handle->m_state  == CAT_CLOSE)
        {
            handle->m_state  = KEY_OPEN;
        }
        else if(handle->m_state  == KEY_OPEN && ch == ':')
        {
            handle->m_state  = VALUE_OPEN;
            buffer[i] = '\0';
            
            FieldNode* fieldNode = new FieldNode();
            fieldNode->mp_name = new char[i+1];
            memcpy(fieldNode->mp_name, buffer, i+1);
            
            if( lastNode == NULL )
            {
                tableNode->mp_fields = fieldNode;
                lastNode = tableNode->mp_fields;
            }
            else
            {
                lastNode->mp_next = fieldNode;
                lastNode = fieldNode;
            }
            
            i = 0;
            continue;
        }
        else if(handle->m_state  == VALUE_OPEN && ch == 10)
        {
            handle->m_state  = KEY_OPEN;
           
            lastNode->mp_type = new char[i+1];
            memcpy(lastNode->mp_type, buffer, i);
            lastNode->mp_type[i] = '\0';
            i = 0;
            continue;
        }
        
        buffer[i] = ch;
        i++;
    }  
    
    if( i != 0 )
    {
        if(handle->m_state == VALUE_OPEN)
        {
            lastNode->mp_type = new char[i+1];
            memcpy(lastNode->mp_type, buffer, i);
			lastNode->mp_type[i] = '\0';
        }
    }
    
    if(one)
    {
        return tableNode;
    }
    delete tableNode;
    return NULL;
    
}