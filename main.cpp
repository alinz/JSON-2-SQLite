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
#include "JSON2SQLite.h"
#include "../lib/sqlite3.h"

#ifdef _DEBUG
#	pragma comment(lib, "../lib/sqlite3_d.lib")
#else
#	pragma comment(lib, "../lib/sqlite3.lib")
#endif

int main()
{
	sqlite3 *db;
	int rc = sqlite3_open("hello.db", &db);

	mz::JSON2SQLite parser("data.json", "struct.ini", 0);
//	parser.Action();
//	parser.BuildTables();
	parser.DeleteAllRecords();
	return 0;
}