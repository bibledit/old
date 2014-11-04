/*
Copyright (©) 2003-2014 Teus Benschop.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#ifndef INCLUDED_DATABASE_STYLES_H
#define INCLUDED_DATABASE_STYLES_H


#include <config/libraries.h>


using namespace std;


class Database_Styles
{
public:
  Database_Styles ();
  ~Database_Styles ();
  void create ();
  void optimize ();
  string createStandardSheet (string name = "");
  void createSheet (string sheet);
  vector <string> getSheets ();
  void importXml (string sheet, string xml);
  string exportXml (string name);
  void deleteSheet (string name);
  void addMarker (string sheet, string marker);
  void deleteMarker (string sheet, string marker);
private:
  sqlite3 * connect ();
};


class Database_Styles_Item
{
public:
  Database_Styles_Item ();
  ~Database_Styles_Item ();
  string marker;
  string name;
  string info;
  string category;
  int type;
  int subtype;
  string fontsize;
  int italic;
  int bold;
  int underline;
  int smallcaps;
  int superscript;
  int justification;
  string spacebefore;
  string spaceafter;
  string leftmargin;
  string rightmargin;
  string firstlineindent;
  int spancolumns;
  string color;
  int print;
  int userbool1;
  int userbool2;
  int userbool3;
  int userint1;
  int userint2;
  int userint3;
  string userstring1;
  string userstring2;
  string userstring3;
};


#endif
