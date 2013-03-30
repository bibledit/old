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


#ifndef INCLUDED_CHECK_PARALLEL_PASSAGES_H
#define INCLUDED_CHECK_PARALLEL_PASSAGES_H


#include "libraries.h"
#include "progresswindow.h"
#include "types.h"
#include "ot-nt-parallels.h"


class CheckOTQuotationsInNT
{
public:
  CheckOTQuotationsInNT (const ustring& project, 
                           const vector<unsigned int>& books,  
                           bool includetext);
  ~CheckOTQuotationsInNT ();
  vector <ustring> nt_refs;
  vector <ustring> nt_texts;
  vector <VectorUstring> ot_refs;
  vector <VectorUstring> ot_texts;
  vector <ustring> references;
  vector <ustring> comments;
private:
  ProgressWindow * progresswindow;
};


class CheckParallelPassages
{
public:
  CheckParallelPassages (bool nt,
                         const ustring& project, 
                         const vector<unsigned int>& books,  
                         bool includetext, 
                         bool gui,
                         const ustring & project2="");
  ~CheckParallelPassages ();
  vector <OtNtParallelDataSection> data;
  vector <ustring> references;
  vector <ustring> comments;
private:
  ProgressWindow * progresswindow;
};


#endif
