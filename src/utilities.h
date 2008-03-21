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


#ifndef INCLUDED_UTILITIES_H
#define INCLUDED_UTILITIES_H


#include "libraries.h"
#include <glib.h>
#include <gtk/gtk.h>


ustring number_in_string (const ustring & str);
unsigned int digit_count_in_string (const ustring & str);
ustring upperCase (const ustring & s);
ustring lowerCase (const ustring & s);
ustring remove_spaces (const ustring & s);
void write_lines (const ustring & file, vector < ustring > &lines);
ustring temporary_file (const ustring & filename);
ustring string_reverse (const ustring & s);
ustring double_apostrophy (const ustring & line);
unsigned int file_get_modification_time (const ustring & filename);
unsigned int file_get_size (const ustring & filename);
void textbuffer_get_lines (GtkTextBuffer * buffer, vector<ustring>& lines, bool trimline = true);
bool replace_text (ustring& line, const ustring& look_for, const ustring& replace_with);
bool replace_text_between (ustring& line, const ustring& start, const ustring& end, const ustring& replacement);
void quick_sort (vector<unsigned int>& one, vector<ustring>& two, unsigned int beg, unsigned int end);
void quick_sort (vector<ustring>& one, vector<unsigned int>& two, unsigned int beg, unsigned int end);
void quick_sort (vector<unsigned int>& one, vector<unsigned int>& two, unsigned int beg, unsigned int end);
void quick_sort (vector<int>& one, vector<unsigned int>& two, unsigned int beg, unsigned int end);
void quick_sort (vector<ustring>& one, vector<ustring>& two, unsigned int beg, unsigned int end);
void quick_sort (vector<ustring>& one, vector<bool>& two, unsigned int beg, unsigned int end);
gchar * de_windows_notepad (gchar * contents);
ustring spaces (unsigned int count);
void bitpattern_add (ustring& pattern, bool setting);
bool bitpattern_take (ustring& pattern);
ustring character_to_decimal_entity (const ustring& character);
ustring character_to_hexadecimal_entity (const ustring& character);
void string_append_line (ustring& container, const ustring& line);
ustring present_working_directory ();


class ReadDirectories
{
public:
  ReadDirectories (const ustring & path, const ustring & prefix, const ustring & suffix);
  ~ReadDirectories ();
  vector <ustring> directories;
private:
};

class ReadFiles
{
public:
  ReadFiles (const ustring & path, const ustring & prefix, const ustring & suffix);
  ~ReadFiles ();
  vector <ustring> files;
private:
};

class ReadText
{
public:
  ReadText (const ustring & file, bool silent = false, bool trimming = true);
  ~ReadText ();
  vector < ustring > lines;
private:
};

class WriteText
{
public:
  WriteText (const ustring & file);
  ~WriteText ();
  void text (const ustring& text);
private:
  int fd;
};

class Parse
{
public:
  Parse (const ustring & line, bool remove_punctuation = true, const ustring& separator = " ");
  ~Parse ();
  vector < ustring > words;
private:
};

class ParseWords
{
public:
  ParseWords (const ustring& text);
  vector <ustring> words;
private:
};

class ParseLine
{
public:
  ParseLine (const ustring & text);
  ~ParseLine ();
  vector <ustring> lines;
private:
};

#endif
