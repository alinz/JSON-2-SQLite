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
#include <string>
#include "JSONParser.h"
#include <stdio.h>

mz::JSONParser::JSONParser( const char* filename )
{
	mp_file = NULL;
	mp_file = fopen(filename, "r");
}

mz::JSONParser::~JSONParser()
{
	FILE* tmp = (FILE*)mp_file;
	if( tmp != NULL )
	{
		fclose(tmp);
	}
}

bool mz::JSONParser::Validate( int depth )
{
	if( mp_file == NULL ) return false;
	mz::Stack<char> stack( depth ); 
	fseek((FILE*)mp_file, 0, SEEK_SET);
	char ch;
	while( (ch = fgetc((FILE*)mp_file)) != EOF )
	{
		switch(ch)
		{
		case '[':
			stack.Push(']');
			break;
		case '{':
			stack.Push('}');
			break;
			case ']':
		case '}':
			{
				char v;
				if( stack.Get(v) )
				{
					if( v != ch ) return false;
					stack.Pop();
				}
				else return false;
			}
			break;
		}
	}
	fseek((FILE*)mp_file, 0, SEEK_SET);
	return true;
}

mz::JSONParser::Node* mz::JSONParser::GetRootNode()
{
	if(mp_file == NULL) return NULL;

	FILE* in = (FILE*)mp_file;

	fseek(in, 0, SEEK_SET);

	char ch;
	long i = 0;
	while( (ch = fgetc(in) ) != EOF )
	{
		switch( ch )
		{
		case '{':
		case '[':
			{
				long ve = mz::GetSize(i, ch, in);
				mz::JSONParser::Node::NodeType nodeType = ( ch == '{' ) ? mz::JSONParser::Node::DICT : mz::JSONParser::Node::LIST;
				mz::JSONParser::Node* node = new mz::JSONParser::Node( 0, 0, i+1, ve-1, nodeType, in );
				return node;
			}
		}
		i++;
	}
	return NULL;
}


//if type is " then, the pos must be the next element after that.
//if type either { ot [, then the pos must be the [ or { elements
long mz::GetSize( long pos, char type, void* file )
{
	if( file == NULL ) return 0;
	FILE* in = (FILE*)file;
	fseek( in, pos, SEEK_SET );
	char ch;
	
	switch( type )
	{
	case '"':
		{
			bool ignore = false;
			long size = 0;
			while( (ch = fgetc(in)) != EOF )
			{
				if( ch == '\\' ) ignore = true;
				else if( ignore == false && ch == '"' ) break;
				else if( ch != '\\' && ignore == true ) ignore = false;
				size++;
			}
			return pos + size - 1;
		}

	case '{':
	case '[':
		{
			mz::Stack<char> stack( 100 );
			char end = (type == '[')? ']' : '}';
			while( (ch = fgetc(in)) != EOF )
			{
				if( ch == type ) stack.Push(end);
				else if( ch == end )
				{
					char v;
					if( stack.Get(v) )
					{
						if( v != ch ) return 0;
						stack.Pop();
					}
					else return 0;
				}
				if( stack.size() == 0 ) return ftell(in) - 1;
			}
		}
	}

	return 0;
}