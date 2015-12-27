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


#ifndef INCLUDED_SWORD_LOGIC_H
#define INCLUDED_SWORD_LOGIC_H


#include <config/libraries.h>


string sword_logic_get_path ();
void sword_logic_refresh_module_list ();
string sword_logic_module_list_path ();
string sword_logic_get_source (string line);
string sword_logic_get_remote_module (string line);
string sword_logic_get_installed_module (string line);
string sword_logic_get_version (string line);
string sword_logic_get_name (string line);
void sword_logic_install_module (string source, string module);
void sword_logic_uninstall_module (string module);
vector <string> sword_logic_get_available ();
vector <string> sword_logic_get_installed ();
string sword_logic_get_text (string source, string module, int book, int chapter, int verse);
void sword_logic_update_installed_modules ();
void sword_logic_trim_modules ();
string sword_logic_installing_module_text ();
string sword_logic_virtual_url (const string & module, int book, int chapter, int verse);


#endif
