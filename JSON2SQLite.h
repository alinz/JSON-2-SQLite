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
#ifndef __JSON_2_SQLITE__
#define __JSON_2_SQLITE__

namespace mz
{
	class JSON2SQLite
	{
	public:
		JSON2SQLite( const char* json, const char* ini, const char* db );
		~JSON2SQLite();

		bool IsReady();
		bool BuildTables();
		bool DeleteAllRecords();
		bool Action();
	
	private:
		void* mp_json;
		void* mp_db;
		void* mp_ini;
	};
}

#endif