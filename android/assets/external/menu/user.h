/*
Copyright (©) 2003-2015 Teus Benschop.

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


#ifndef INCLUDED_MENU_USER_H
#define INCLUDED_MENU_USER_H


#include <config/libraries.h>
#include <libxml/xmlwriter.h>


struct Menu_User_Item
{
  string id;
  string href;
  string text;
  vector <Menu_User_Item> * submenu;
};


class Menu_User
{
public:
  Menu_User (void * webserver_request_in);
  ~Menu_User ();
  string create (string request);
private:
  void * webserver_request;
  vector <Menu_User_Item> * mainmenu (string request);
  vector <Menu_User_Item> * usermenu ();
  void submenu (xmlTextWriterPtr xmlwriter, vector <Menu_User_Item> * menu);
};


#endif
