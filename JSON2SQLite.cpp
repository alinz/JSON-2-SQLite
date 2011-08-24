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
#include <iostream>
#include <vector>
#include <string>
#include <map>

#include "JSONParser.h"
#include "JSON2SQLite.h"
#include "INIParser.h"

typedef std::map<std::string, int> _FieldInfoTable;
typedef std::map<std::string, _FieldInfoTable> _TableInfo;

_TableInfo table_info;
mz::TableInfo* ini_table;

void ReplaceStr( std::string& data, char* orginStr, char* newStr );
void FillTableInfo();

mz::JSON2SQLite::JSON2SQLite( const char* json, const char* ini, const char* db )
{
	mp_json = new mz::JSONParser( json );
	mp_ini = new mz::TableInfo( ini );
	ini_table = ((mz::TableInfo*)mp_ini);
	FillTableInfo();
}

mz::JSON2SQLite::~JSON2SQLite()
{
	if(mp_json) delete (mz::JSONParser*)mp_json;
	if(mp_ini) delete (mz::TableInfo*)mp_ini;
}

bool mz::JSON2SQLite::IsReady()
{
	mz::JSONParser* json = (mz::JSONParser*)mp_json;

	//check the db as well to make sure that db is ready and connected.
	bool c1 = json->Validate( 20 );
	
	return c1;
}

bool mz::JSON2SQLite::Action()
{
	mz::JSONParser* parser = (mz::JSONParser*)mp_json;
	mz::JSONParser::Node* tables = parser->GetRootNode();

	if( tables == NULL ) return false;

	while( mz::JSONParser::Node* table = tables->Next() )
	{
		std::string tableName;
		table->GetNodeName(tableName);
		while( mz::JSONParser::Node* row = table->Next() )
		{
			std::string fields = "";
			std::string values = "";
			bool first = true;
			_FieldInfoTable myTable = table_info[tableName];
			while( mz::JSONParser::Node* col = row->Next() )
			{
				std::string field;
				std::string value;

				col->GetNodeName(field);
				col->GetNodeValue(value);

				if(myTable[field] == 'T')
				{
					ReplaceStr(value, "'", "''");
					value = "'" + value + "'";
				}

				if(first)
				{
					fields = field;
					values = value;
					first = false;
				}
				else
				{
					fields += ',' + field;
					values += ',' + value;
				}

				delete col;
			}
			delete row;

			if(first == false)
			{
				std::string sql = "INSERT INTO " + tableName + " (" + fields + ") VALUES (" + values + ");";
				std::cout<<sql<<std::endl;
			}
		}
		delete table;
	}
	delete tables;

	return true;
}

bool mz::JSON2SQLite::BuildTables()
{
	_TableInfo::iterator itr1 = table_info.begin();
	_FieldInfoTable::iterator itr2;
	for(;itr1 != table_info.end(); itr1++)
	{
		bool first = true;
		std::string sql = "";
		for(itr2 = itr1->second.begin(); itr2 != itr1->second.end(); itr2++ )
		{
			std::string type;
			switch(itr2->second)
			{
			case 'T':
				type = "TEXT";
				break;
			case 'I':
				type = "INTEGER";
				break;
			case 'd':
				type = "double";
				break;
			}

			if(first)
			{
				sql = "CREATE TABLE " + itr1->first + " (" + itr2->first + " " + type;
				first = false;
			}
			else
			{
				sql += ", " + itr2->first + " " + type;
			}
		}
		if(sql != "")
		{
			sql += ");";
			std::cout<<sql<<std::endl;
		}
	}
	return false;
	//CREATE TABLE <tablename> (<fieldname> <type> PRIMARY KEY, <fieldname> <Type>, ...);
}

bool mz::JSON2SQLite::DeleteAllRecords()
{
	_TableInfo::iterator itr1 = table_info.begin();
	for(;itr1 != table_info.end(); itr1++)
	{
		std::string sql = "DELETE FROM " + itr1->first + ";";
		std::cout<<sql<<std::endl;
	}

	return false;
}

void ReplaceStr( std::string& data, char* orginStr, char* newStr )
{
    std::string::size_type pos = 0;
	int i1 = strlen(orginStr);
	int i2 = strlen(newStr);
    while ( (pos = data.find(orginStr, pos)) != std::string::npos ) 
	{
        data.replace( pos, i1, newStr );
        pos+=i2;
    }
}

void FillTableInfo()
{
	while(mz::TableInfo::TableNode* table = ini_table->GetNextTable())
	{
		mz::TableInfo::FieldNode* field = table->mp_fields;
		_FieldInfoTable tmpfield;
		while(field)
		{
			std::string fieldName = field->mp_name;
			tmpfield[fieldName] = field->mp_type[0];

			mz::TableInfo::FieldNode* tmp = field;
			field = field->mp_next;
			delete tmp;
		}
		table_info[table->mp_tableName] = tmpfield;
		delete table;
	}
}