/*
Copyright (©) 2003-2016 Teus Benschop.

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


#ifndef INCLUDED_DATABASE_PRIVILEGES_H
#define INCLUDED_DATABASE_PRIVILEGES_H


#include <config/libraries.h>


class Database_Privileges
{
public:
  static const char * database ();
  static void create ();
  static void upgrade ();
  static void optimize ();
  static bool healthy ();
  static void setBibleBook (string username, string bible, int book, bool write);
  static void getBibleBook (string username, string bible, int book, bool & read, bool & write);
  static int getBibleBookCount ();
  static bool getBibleBookExists (string username, string bible, int book);
  static void removeBibleBook (string username, string bible, int book);
  static void setFeature (string username, int feature, bool enabled);
  static bool getFeature (string username, int feature);
private:
};


#endif
