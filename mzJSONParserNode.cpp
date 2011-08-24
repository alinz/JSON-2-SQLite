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

bool LookForValue( long& cr, long& vs, long& ve, mz::JSONParser::Node::NodeType& nodeType, FILE* in, const long& m_ve );
void MoveCurrent( long& cr, const long& ve, FILE* in );

mz::JSONParser::Node::Node( long ns, long ne, long vs, long ve, NodeType nodeType, void* file )
{
	m_cr		= vs;
	m_ns		= ns;
	m_ne		= ne;
	m_vs		= vs;
	m_ve		= ve;
	m_nodeType	= nodeType;
	mp_file		= file;
}

mz::JSONParser::Node::~Node()
{
	mp_file = NULL;
}

void mz::JSONParser::Node::GetNodeName( std::string& name )
{
	if( m_ns == 0 )
	{
		name = "";
		return;
	}

	int len = m_ne - m_ns + 1;
	char* buffer = new char[len + 1];
	fseek((FILE*)mp_file, m_ns, SEEK_SET);
	fread(buffer, len, 1, (FILE*)mp_file );
	buffer[len] = '\0';
	name=buffer;
	delete buffer;
}

void mz::JSONParser::Node::GetNodeValue( std::string& value )
{
	if( (m_vs == 0) || (m_vs > m_ve) )
	{
		value = "";
		return;
	}

	int len = m_ve - m_vs + 1;
	char* buffer = new char[len + 1];
	fseek((FILE*)mp_file, m_vs, SEEK_SET);
	fread( buffer, len, 1, (FILE*)mp_file );
	buffer[len] = '\0';
	value = buffer;
}

mz::JSONParser::Node* mz::JSONParser::Node::Next()
{
	if(m_cr >= m_ve) return NULL;

	long ns = 0;
	long ne = 0;
	long vs = 0;
	long ve = 0;
	mz::JSONParser::Node::NodeType nodeType = mz::JSONParser::Node::ERROR;

	FILE* in = (FILE*)mp_file;

	switch(m_nodeType)
	{
		//if the node type is VALUE then there is no next element.
	case mz::JSONParser::Node::VALUE:
		return NULL;

		//if the node type is LIST then we have to look for ", {, [ only.
		//if we found " then it is a value and it's a value
		//if we found { then it is a dict
		//if we found [ then it is a list
		//we also have to find next comma or end of the elements.
		//we also have to look for empty list and empy string and empty dict
	case mz::JSONParser::Node::LIST:
		{
			//look for ", [, { and assigned it to nodeType
			if( LookForValue( m_cr, vs, ve, nodeType, in, m_ve ) == false ) return NULL;

			//move the cur to the next elemenet
			m_cr = ve;
			MoveCurrent( m_cr, m_ve, in );

			//by now we should all the information
			mz::JSONParser::Node* node = new mz::JSONParser::Node( ns, ne, vs, ve, nodeType, in );
			return node;
		}

		//if the node is dict then the first element is the key or name
		//so we have to set the ns and ne as well. then we have to look for :
		//and then the rest is similar to LIST
	case mz::JSONParser::Node::DICT:
		{
			//we can use this function to extarct the name or key
			if( LookForValue( m_cr, ns, ne, nodeType, in, m_ve ) == false ) return NULL;

			//now we have to look for :, if we don;t found it, we set the m_cr to m_ve and return NULL
			m_cr = ne;
			fseek(in, m_cr, SEEK_SET);
			char ch;
			while( (ch = fgetc(in)) != EOF )
			{
				m_cr++;
				if( m_cr >= m_ve )
				{
					m_cr = m_ve;
					return NULL;
				}

				if( ch == ':' )
				{
					break;
				}
			}

			//now we have the similar situation as LIST
			if( LookForValue( m_cr, vs, ve, nodeType, in, m_ve ) == false ) return NULL;

			//move the cur to the next elemenet
			m_cr = ve;
			MoveCurrent( m_cr, m_ve, in );

			//by now we should all the information
			mz::JSONParser::Node* node = new mz::JSONParser::Node( ns, ne, vs, ve, nodeType, in );
			return node;
		}
	}
	return NULL;
}

void mz::JSONParser::Node::Debug()
{
	char nodeTypeString[5];

	switch(m_nodeType)
	{
	case mz::JSONParser::Node::DICT:
		strcpy(nodeTypeString, "DICT");
		break;
	case mz::JSONParser::Node::LIST:
		strcpy(nodeTypeString, "LIST");
		break;
	case mz::JSONParser::Node::VALUE:
		strcpy(nodeTypeString, "VAL");
		break;
	}

	//FILE* in = (FILE*)mp_file;
	//fseek(in, m_ns, SEEK_SET);
	//char ch1 = fgetc(in);

	//fseek(in, m_ne, SEEK_SET);
	//char ch2 = fgetc(in);

	//fseek(in, m_vs, SEEK_SET);
	//char ch3 = fgetc(in);

	//fseek(in, m_ve, SEEK_SET);
	//char ch4 = fgetc(in);

	//printf("\nname_start:%ld '%c'\nname_end:%ld '%c'\nvalue_start:%ld '%c'\nvalue_end:%ld '%c'\nType:%s\n", m_ns, ch1, m_ne, ch2, m_vs, ch3, m_ve, ch4, nodeTypeString);
	printf("name_start:%ld \nname_end:%ld \nvalue_start:%ld \nvalue_end:%ld \nType:%s\n\n", m_ns, m_ne, m_vs, m_ve, nodeTypeString);
}

bool LookForValue( long& cr, long& vs, long& ve, mz::JSONParser::Node::NodeType& nodeType, FILE* in, const long& m_ve )
{
	char ch;
	fseek( in, cr, SEEK_SET );
	while( (ch = fgetc(in)) != EOF )
	{
		cr++;

		switch( ch )
		{
		case '"':
		case '[':
		case '{':
			{
				vs = cr;
				switch( ch )
				{
				case '"':
					nodeType = mz::JSONParser::Node::VALUE;
					ve = mz::GetSize( vs, ch, in );
					break;

				case '[':
					nodeType = mz::JSONParser::Node::LIST;
					ve = mz::GetSize( vs - 1, ch, in ) - 1;
					break;

				case '{':
					nodeType = mz::JSONParser::Node::DICT;
					ve = mz::GetSize( vs - 1, ch, in ) - 1;
					break;
				}
				
				return true;
			}
		}
		if( cr >= m_ve )
		{
			cr = m_ve;
			return false;
		}
	}
	return false;
}

void MoveCurrent( long& cr, const long& ve, FILE* in )
{
	//test the condition, we know that if cr is less than 3 chars behind ve then
	//we don't need to process, because
	if( cr+2 >= ve )
	{
		cr = ve;
		return;
	}

	char ch;
	fseek( in, cr, SEEK_SET );

	while( (ch = fgetc(in)) != EOF )
	{
		cr++;
		if( cr >= ve )
		{
			cr = ve;
			return;
		}
		if( ch == ',' )	return;
	}
}

