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


#ifndef INCLUDED_SWORD_INSTALLMGR_H
#define INCLUDED_SWORD_INSTALLMGR_H


#include <config/libraries.h>


void sword_installmgr_initialize_configuration ();
void sword_installmgr_synchronize_configuration_with_master ();
void sword_installmgr_list_remote_sources (vector <string> & sources);
void sword_installmgr_refresh_remote_source (string name);
void sword_installmgr_list_remote_modules (string source_name, vector <string> & modules);
void sword_installmgr_install_from_remote (string source_name, string module_name);


#endif
