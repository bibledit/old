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

#ifndef INCLUDED_SCRIPTS_H
#define INCLUDED_SCRIPTS_H

#include "libraries.h"
#include <glib.h>

enum ScriptType {stSed, stTECkit, stFree, stEnd};

ustring scripts_straight_through();
ustring script_prefix(ScriptType scripttype);
ustring script_suffix(ScriptType scripttype, bool binary = false);
vector <ustring> scripts_get_all();
bool script_available(const ustring& script);
ustring script_filter(const ustring& scriptname, bool straightthrough, const ustring& inputfile, const ustring& outputfile);
ustring script_get_path(const ustring& script, ScriptType * scripttype, bool binary = false);
ustring script_get_path(const ustring& script, ScriptType scripttype, bool binary = false);
ustring script_temporal_script_file();
ustring script_temporal_input_file();
ustring script_temporal_output_file();
ustring script_temporal_error_file();
ustring script_get_named_type(ScriptType scripttype);
bool script_sed_installed();
bool script_teckit_installed();
const gchar * script_sed_binary();
const gchar * script_teckit_compiler();
const gchar * script_teckit_txtconverter();
const gchar * script_free_input_identifier();
const gchar * script_free_output_identifier();
void script_encode_usfm_file(const ustring& filename);
void script_decode_usfm_file(const ustring& filename);

#endif
