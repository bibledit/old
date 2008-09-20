/*
 ** Copyright (©) 2003-2008 Teus Benschop.
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
 ** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 **  
 */

#include "text2pdf_intermediate.h"
#include "tiny_utilities.h"
#include "ustring.h"

bool intermediary_1_double(vector <ustring> * lines, const ustring& command, double parameter)
// Write intermediary text.
{
  if (!lines)
    return false;
  ustring text = command + "|" + convert_to_string(parameter);
  lines->push_back(text);
  return true;
}

bool intermediary_2_double(vector <ustring> * lines, const ustring& command, double parameter1, double parameter2)
// Write intermediary text.
{
  if (!lines)
    return false;
  ustring text = command + "|" + convert_to_string(parameter1) + "|" + convert_to_string(parameter2);
  lines->push_back(text);
  return true;
}

bool intermediary_4_double(vector <ustring> * lines, const ustring& command, double parameter1, double parameter2, double parameter3, double parameter4) // Todo
// Write intermediary text.
{
  if (!lines)
    return false;
  ustring text = command + "|" + convert_to_string(parameter1) + "|" + convert_to_string(parameter2) + "|" + convert_to_string(parameter3) + "|" + convert_to_string(parameter4);
  lines->push_back(text);
  return true;
}

bool intermediary_void(vector <ustring> * lines, const ustring& command)
// Write intermediary text.
{
  if (!lines)
    return false;
  lines->push_back(command);
  return true;
}

bool intermediary_1_bool(vector <ustring> * lines, const ustring& command, bool parameter)
// Write intermediary text.
{
  if (!lines)
    return false;
  ustring text = command + "|" + convert_to_string(parameter);
  lines->push_back(text);
  return true;
}

bool intermediary_1_int(vector <ustring> * lines, const ustring& command, int parameter)
// Write intermediary text.
{
  if (!lines)
    return false;
  ustring text = command + "|" + convert_to_string(parameter);
  lines->push_back(text);
  return true;
}

bool intermediary_2_int(vector <ustring> * lines, const ustring& command, int parameter1, int parameter2)
// Write intermediary text.
{
  if (!lines)
    return false;
  ustring text = command + "|" + convert_to_string(parameter1) + "|" + convert_to_string(parameter2);
  lines->push_back(text);
  return true;
}

bool intermediary_1_ustring(vector <ustring> * lines, const ustring& command, const ustring& parameter)
// Write intermediary text.
{
  if (!lines)
    return false;
  ustring text = command + "|" + parameter;
  lines->push_back(text);
  return true;
}

