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

#ifndef INCLUDED_TEXT2PDF_INTERMEDIATE_H
#define INCLUDED_TEXT2PDF_INTERMEDIATE_H

#include "libraries.h"

bool text2pdf_intermediary_1_double(vector <ustring> * lines, const ustring& command, double parameter);
void text2pdf_intermediary_1_double_get(const vector <ustring>& input, double& parameter);

bool text2pdf_intermediary_2_double(vector <ustring> * lines, const ustring& command, double parameter1, double parameter2);
void text2pdf_intermediary_2_double_get(const vector <ustring>& input, double& parameter1, double& parameter2);

bool text2pdf_intermediary_4_double(vector <ustring> * lines, const ustring& command, double parameter1, double parameter2, double parameter3, double parameter4);
void text2pdf_intermediary_4_double_get(const vector <ustring>& input, double& parameter1, double& parameter2, double& parameter3, double& parameter4);

bool text2pdf_intermediary_void(vector <ustring> * lines, const ustring& command);

bool text2pdf_intermediary_1_bool(vector <ustring> * lines, const ustring& command, bool parameter);
void text2pdf_intermediary_1_bool_get(const vector <ustring>& input, bool& parameter);

bool text2pdf_intermediary_1_int(vector <ustring> * lines, const ustring& command, int parameter);
void text2pdf_intermediary_1_int_get(const vector <ustring>& input, int& parameter);

bool text2pdf_intermediary_2_int(vector <ustring> * lines, const ustring& command, int parameter1, int parameter2);
void text2pdf_intermediary_2_int_get(const vector <ustring>& input, int& parameter1, int& parameter2);

bool text2pdf_intermediary_1_ustring(vector <ustring> * lines, const ustring& command, const ustring& parameter);
void text2pdf_intermediary_1_ustring_get(const vector <ustring>& input, ustring& parameter);

#endif
