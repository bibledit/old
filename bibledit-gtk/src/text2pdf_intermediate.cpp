/*
 ** Copyright (©) 2003-2009 Teus Benschop.
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

#include "text2pdf_intermediate.h"
#include "tiny_utilities.h"
#include "ustring.h"

bool text2pdf_intermediary_1_double(vector < ustring > *lines, const ustring & command, double parameter)
// Write intermediary text.
{
  if (!lines)
    return false;
  ustring text = command + "|" + convert_to_string(parameter);
  lines->push_back(text);
  return true;
}

void text2pdf_intermediary_1_double_get(const vector < ustring > &input, double &parameter)
// Retrieve intermediary text
{
  parameter = 0;
  if (input.size() > 0)
    parameter = convert_to_double(input[0]);
}

bool text2pdf_intermediary_2_double(vector < ustring > *lines, const ustring & command, double parameter1, double parameter2)
// Write intermediary text.
{
  if (!lines)
    return false;
  ustring text = command + "|" + convert_to_string(parameter1) + "|" + convert_to_string(parameter2);
  lines->push_back(text);
  return true;
}

void text2pdf_intermediary_2_double_get(const vector < ustring > &input, double &parameter1, double &parameter2)
// Retrieve intermediary text
{
  parameter1 = 0;
  parameter2 = 0;
  if (input.size() > 0)
    parameter1 = convert_to_double(input[0]);
  if (input.size() > 1)
    parameter2 = convert_to_double(input[1]);
}

bool text2pdf_intermediary_4_double(vector < ustring > *lines, const ustring & command, double parameter1, double parameter2, double parameter3, double parameter4)
// Write intermediary text.
{
  if (!lines)
    return false;
  ustring text = command + "|" + convert_to_string(parameter1) + "|" + convert_to_string(parameter2) + "|" + convert_to_string(parameter3) + "|" + convert_to_string(parameter4);
  lines->push_back(text);
  return true;
}

void text2pdf_intermediary_4_double_get(const vector < ustring > &input, double &parameter1, double &parameter2, double &parameter3, double &parameter4)
// Retrieve intermediary text
{
  parameter1 = 0;
  parameter2 = 0;
  parameter3 = 0;
  parameter4 = 0;
  if (input.size() > 0)
    parameter1 = convert_to_double(input[0]);
  if (input.size() > 1)
    parameter2 = convert_to_double(input[1]);
  if (input.size() > 2)
    parameter3 = convert_to_double(input[2]);
  if (input.size() > 3)
    parameter4 = convert_to_double(input[3]);
}

bool text2pdf_intermediary_void(vector < ustring > *lines, const ustring & command)
// Write intermediary text.
{
  if (!lines)
    return false;
  lines->push_back(command);
  return true;
}

bool text2pdf_intermediary_1_bool(vector < ustring > *lines, const ustring & command, bool parameter)
// Write intermediary text.
{
  if (!lines)
    return false;
  ustring text = command + "|" + convert_to_string(parameter);
  lines->push_back(text);
  return true;
}

void text2pdf_intermediary_1_bool_get(const vector < ustring > &input, bool & parameter)
// Retrieve intermediary text
{
  parameter = false;
  if (input.size() > 0)
    parameter = convert_to_bool(input[0]);
}

bool text2pdf_intermediary_1_int(vector < ustring > *lines, const ustring & command, int parameter)
// Write intermediary text.
{
  if (!lines)
    return false;
  ustring text = command + "|" + convert_to_string(parameter);
  lines->push_back(text);
  return true;
}

void text2pdf_intermediary_1_int_get(const vector < ustring > &input, int &parameter)
// Retrieve intermediary text
{
  parameter = 0;
  if (input.size() > 0)
    parameter = convert_to_int(input[0]);
}

bool text2pdf_intermediary_2_int(vector < ustring > *lines, const ustring & command, int parameter1, int parameter2)
// Write intermediary text.
{
  if (!lines)
    return false;
  ustring text = command + "|" + convert_to_string(parameter1) + "|" + convert_to_string(parameter2);
  lines->push_back(text);
  return true;
}

void text2pdf_intermediary_2_int_get(const vector < ustring > &input, int &parameter1, int &parameter2)
// Retrieve intermediary text
{
  parameter1 = 0;
  parameter2 = 0;
  if (input.size() > 0)
    parameter1 = convert_to_int(input[0]);
  if (input.size() > 1)
    parameter2 = convert_to_int(input[1]);
}

bool text2pdf_intermediary_1_ustring(vector < ustring > *lines, const ustring & command, const ustring & parameter)
// Write intermediary text.
{
  if (!lines)
    return false;
  ustring text = command + "|" + parameter;
  lines->push_back(text);
  return true;
}

void text2pdf_intermediary_1_ustring_get(const vector < ustring > &input, ustring & parameter)
// Retrieve intermediary text
{
  parameter.clear();
  if (input.size() > 0)
    parameter = input[0];
}
