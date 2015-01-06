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


#ifndef INCLUDED_MENU_MAIN_H
#define INCLUDED_MENU_MAIN_H


#include <config/libraries.h>
#include <libxml/xmlwriter.h>


struct Menu_Main_Item
{
  string id;
  string href;
  string text;
  vector <Menu_Main_Item> * submenu;
};


class Menu_Main
{
public:
  Menu_Main (void * webserver_request_in);
  ~Menu_Main ();
  string create ();
private:
  void * webserver_request;
  vector <Menu_Main_Item> * mainmenu ();
  vector <Menu_Main_Item> * biblemenu ();
  vector <Menu_Main_Item> * bible_edit_menu ();
  vector <Menu_Main_Item> * bible_workbench_menu ();
  vector <Menu_Main_Item> * bible_checks_menu ();
  vector <Menu_Main_Item> * notesmenu ();
  vector <Menu_Main_Item> * resourcesmenu ();
  vector <Menu_Main_Item> * changesmenu ();
  vector <Menu_Main_Item> * planningmenu ();
  vector <Menu_Main_Item> * toolsmenu ();
  vector <Menu_Main_Item> * exportssubmenu ();
  vector <Menu_Main_Item> * settingsmenu ();
  vector <Menu_Main_Item> * stylessubmenu ();
  vector <Menu_Main_Item> * helpmenu ();
  void submenu    (xmlTextWriterPtr xmlwriter, vector <Menu_Main_Item> * menu);
  void subsubmenu (xmlTextWriterPtr xmlwriter, vector <Menu_Main_Item> * menu);
};


#endif
