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
#ifndef __INI_PARSER__
#define __INI_PARSER__

namespace  mz 
{
    class TableInfo
    {
    public:
        class FieldNode
        {
        public:
            FieldNode()
            {
                mp_name = 0;
                mp_type = 0;
                mp_next = 0;
            }
            ~FieldNode()
            {
                delete [] mp_name;
                delete [] mp_type;
                //if( mp_next ) delete mp_next;
            }
        public:
            char*       mp_name;
            char*       mp_type;
            FieldNode*  mp_next;
        };
        class TableNode
        {
        public:
            TableNode()
            {
                mp_tableName = 0;
                mp_fields    = 0;
            }
            ~TableNode()
            {
                if(mp_tableName) delete [] mp_tableName;
                //if( mp_fields ) delete mp_fields;
            }
        public:
            char* mp_tableName;
            FieldNode* mp_fields;
        };
        
    public:
        TableInfo( const char* filename );
        ~TableInfo();
        
        TableNode* GetNextTable();

    private:
        void* mp_handle;
    };
}

#endif