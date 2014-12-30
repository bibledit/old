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


#ifndef INCLUDED_TASKS_LOGIC
#define INCLUDED_TASKS_LOGIC


#include <config/libraries.h>


#define ROTATEJOURNAL "rotatejournal"
#define RECEIVEEMAIL "receiveemail"
#define SENDEMAIL "sendemail"
#define REINDEXBIBLES "reindexbibles"
#define REINDEXNOTES "reindexnotes"
#define CREATECSS "createcss"
#define IMPORTUSFM "importusfm"


string tasks_logic_folder ();
void tasks_logic_queue (string command, vector <string> parameters = { });


#endif
