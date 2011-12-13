/*
** Copyright (©) 2003-2012 Teus Benschop.
**  
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 3 of the License, or
** (at your option) any later version.
**  
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**  
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
**  
*/

#include "sqlite_reader.h"

SqliteReader::SqliteReader(int dummy)
{
}

SqliteReader::~SqliteReader()
{
}

int SqliteReader::callback(void *userdata, int argc, char **argv, char **column_names)
{
  ((SqliteReader *) userdata)->ustring0.push_back(argv[0]);
  if (argc == 1)
    return 0;
  ((SqliteReader *) userdata)->ustring1.push_back(argv[1]);
  if (argc == 2)
    return 0;
  ((SqliteReader *) userdata)->ustring2.push_back(argv[2]);
  if (argc == 3)
    return 0;
  ((SqliteReader *) userdata)->ustring3.push_back(argv[3]);
  if (argc == 4)
    return 0;
  ((SqliteReader *) userdata)->ustring4.push_back(argv[4]);
  if (argc == 5)
    return 0;
  ((SqliteReader *) userdata)->ustring5.push_back(argv[5]);
  if (argc == 6)
    return 0;
  ((SqliteReader *) userdata)->ustring6.push_back(argv[6]);
  if (argc == 7)
    return 0;
  ((SqliteReader *) userdata)->ustring7.push_back(argv[7]);
  if (argc == 8)
    return 0;
  ((SqliteReader *) userdata)->ustring8.push_back(argv[8]);
  if (argc == 9)
    return 0;
  ((SqliteReader *) userdata)->ustring9.push_back(argv[9]);
  if (argc == 10)
    return 0;
  ((SqliteReader *) userdata)->ustring10.push_back(argv[10]);
  if (argc == 11)
    return 0;
  ((SqliteReader *) userdata)->ustring11.push_back(argv[11]);
  if (argc == 12)
    return 0;
  ((SqliteReader *) userdata)->ustring12.push_back(argv[12]);
  if (argc == 13)
    return 0;
  ((SqliteReader *) userdata)->ustring13.push_back(argv[13]);
  if (argc == 14)
    return 0;
  ((SqliteReader *) userdata)->ustring14.push_back(argv[14]);
  if (argc == 15)
    return 0;
  ((SqliteReader *) userdata)->ustring15.push_back(argv[15]);
  if (argc == 16)
    return 0;
  ((SqliteReader *) userdata)->ustring16.push_back(argv[16]);
  if (argc == 17)
    return 0;
  ((SqliteReader *) userdata)->ustring17.push_back(argv[17]);
  if (argc == 18)
    return 0;
  ((SqliteReader *) userdata)->ustring18.push_back(argv[18]);
  if (argc == 19)
    return 0;
  ((SqliteReader *) userdata)->ustring19.push_back(argv[19]);
  if (argc == 20)
    return 0;
  ((SqliteReader *) userdata)->ustring20.push_back(argv[20]);
  if (argc == 21)
    return 0;
  ((SqliteReader *) userdata)->ustring21.push_back(argv[21]);
  if (argc == 22)
    return 0;
  ((SqliteReader *) userdata)->ustring22.push_back(argv[22]);
  if (argc == 23)
    return 0;
  ((SqliteReader *) userdata)->ustring23.push_back(argv[23]);
  if (argc == 24)
    return 0;
  ((SqliteReader *) userdata)->ustring24.push_back(argv[24]);
  if (argc == 25)
    return 0;
  ((SqliteReader *) userdata)->ustring25.push_back(argv[25]);
  if (argc == 26)
    return 0;
  ((SqliteReader *) userdata)->ustring26.push_back(argv[26]);
  if (argc == 27)
    return 0;
  ((SqliteReader *) userdata)->ustring27.push_back(argv[27]);
  if (argc == 28)
    return 0;
  ((SqliteReader *) userdata)->ustring28.push_back(argv[28]);
  return 0;
}

/*
About other databases than sqlite3.

There is the gdbm database. 
See man gdbm. 
Is it supported on Mac and Windows?
This databaseb does now allow concurrent access and is not thread-safe, 
both of which we need.
*/
