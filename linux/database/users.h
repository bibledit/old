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


#ifndef INCLUDED_DATABASE_USERS_H
#define INCLUDED_DATABASE_USERS_H


#include <config/libraries.h>


class Database_Users
{
public:
  void create ();
  void upgrade ();
  void trim ();
  void optimize ();
  void addNewUser (string username, string password, int level, string email);
  void updateUserPassword (string user, string password);
  bool matchUsernamePassword (string username, string password);
  bool matchEmailPassword (string email, string password);
  string addNewUserQuery (string username, string password, int level, string email);
  string getEmailToUser (string email);
  string getUserToEmail (string user);
  bool usernameExists (string user);
  bool emailExists (string email);
  int getUserLevel (string user);
  void updateUserLevel (string user, int level);
  void removeUser (string user);
  vector <string> getAdministrators ();
  string updateEmailQuery (string username, string email);
  void updateUserEmail (string user, string email);
  vector <string> getUsers ();
  string getmd5 (string user);
  void execute (const string& sql);
private:
  sqlite3 * connect ();
};


#endif
