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


#ifndef INCLUDED_LOCALIZED_BOOKS_H
#define INCLUDED_LOCALIZED_BOOKS_H


#include "libraries.h"


class BookLocalization
{
public:
  BookLocalization (const ustring& language_in, const ustring& filename);
  ustring language;
  vector <unsigned int> id;
  vector <ustring> name;
  vector <ustring> name_casefold;
  vector <ustring> abbreviation;
  vector <ustring> abbreviation_casefold;
};


class BookLocalizations
{
public:
  BookLocalizations (int dummy);
  vector <ustring> localizations_get ();
  unsigned int name2id (const ustring& language, const ustring& name);
  unsigned int abbrev2id (const ustring& language, const ustring& abbrev);
  unsigned int abbrev2id_loose (const ustring& language, const ustring& abbrev);
  ustring id2name (const ustring& language, unsigned int id);
  ustring id2abbrev (const ustring& language, unsigned int id);
private:
  vector <ustring> available_localizations;
  vector <ustring> available_filenames;
  ustring filename_get_localization (ustring filename);
  vector <BookLocalization> loaded_localizations;
  unsigned int language_pointer_get (ustring language);
};


map <unsigned int, ustring> general_adapted_booknames_read ();
void general_adapted_booknames_write (map <unsigned int, ustring>& booknames);
map <ustring, unsigned int> general_adapted_booknames_fill_up (map <unsigned int, ustring>& mapping);


#endif
