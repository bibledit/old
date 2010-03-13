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


#ifndef INCLUDED_MAPPING_H
#define INCLUDED_MAPPING_H


#include "libraries.h"
#include "reference.h"


class Mapping
{
public:
  Mapping (const ustring& name, unsigned int book);
  ~Mapping ();
  void me_to_original (int mychapter, const ustring& myverse,
                       vector<int>& original_chapter, vector<int>& original_verse);
  void me_to_original (vector<int> mychapter, vector<int> myverse, 
                       vector<int>& original_chapter, vector<int>& original_verse);
  void me_to_original (Reference& reference);
  void original_to_me (int originalchapter, const ustring& originalverse,
                       vector<int>& mychapter, vector<int>& myverse);
  void original_to_me (vector<int> original_chapter, vector<int> original_verse,
                       vector<int>& mychapter, vector<int>& myverse);
  void original_to_me (Reference& reference);
  void book_change (int book);
private:
  ustring myname;
  int mybook;
  void get_original (int mychapter, int myverse,
                     vector<int>& originalchapter, vector<int>& originalverse);
  void get_me (int originalchapter, int originalverse,
               vector<int>& mychapter, vector<int>& myverse);
};


#endif
