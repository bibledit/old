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


#ifndef INCLUDED_MENU_LOGIC_H
#define INCLUDED_MENU_LOGIC_H


#include <config/libraries.h>


string menu_logic_href (string href);
string menu_logic_click (string item);

string menu_logic_create_item (string href, string text, bool history);

string menu_logic_translate_menu ();
string menu_logic_search_menu ();
string menu_logic_tools_menu ();
string menu_logic_settings_menu ();

string menu_logic_main_categories (void * webserver_request);

string menu_logic_desktop_category (void * webserver_request);
string menu_logic_translate_category (void * webserver_request);
string menu_logic_search_category (void * webserver_request);
string menu_logic_tools_category (void * webserver_request);
string menu_logic_settings_category (void * webserver_request);
string menu_logic_help_category (void * webserver_request);

bool menu_logic_public_or_guest (void * webserver_request);

string menu_logic_menu_text (string menu_item);
string menu_logic_menu_url (string menu_item);

string menu_logic_breadcrumbs (void * webserver_request, vector <string> crumbs);

string menu_logic_translate_text ();
string menu_logic_search_text ();
string menu_logic_tools_text ();
string menu_logic_settings_text ();
string menu_logic_help_text ();
string menu_logic_public_feedback_text ();
string menu_logic_logout_text ();
string menu_logic_consultation_notes_text ();

#endif
