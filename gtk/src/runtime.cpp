/*
** Copyright (©) 2003-2013 Teus Benschop.
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


#include "runtime.h"
#include "gwrappers.h"
#include "gtkwrappers.h"


bool runtime_check_internal (RuntimeType type, ustring& message)
{
  ustring package;
  switch (type) {
    case rtXeTeX: package = "texlive-xetex"; break;
    case rtPHP:   package = "php5-cli"; break;
    case rtLast:  return true;
  }
  const gchar * program = runtime_program (type);
  if (gw_find_program_in_path(program)) {
    return true;
  }
  message = "Could not find program \"";
  message.append (program);
  message.append ("\". Install package " + package + " to resolve this.");
  gw_message (message); 
  return false;  
}


const gchar * runtime_program (RuntimeType type)
{
  switch (type) {
    case rtXeTeX: return "xetex";
    case rtPHP:   return "php";
    case rtLast:  return NULL;
  }
  return NULL;
}


void runtime_initialize ()
// Does an initial check of all required runtime requirements.
{
  for (unsigned int i = 0; i < rtLast; i++) {
    ustring s;
    runtime_check_internal (RuntimeType (i), s);
  }
}


bool runtime_check (RuntimeType type)
{
  ustring message;
  if (runtime_check_internal (type, message)) {
    return true;
  }
  gtkw_dialog_warning (NULL, message);
  return false;
}
