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


#ifndef INCLUDED_OSIS_H
#define INCLUDED_OSIS_H


#include "libraries.h"
#include <glib.h>
#include <gtk/gtk.h>
#include "utilities.h"


class OsisRoot
{
public:
  OsisRoot (WriteText * wt, const ustring& name, const ustring& description);
  ~OsisRoot ();
private:
  WriteText * mywt;
};


class OsisBook
{
public:
  OsisBook (WriteText * wt, const ustring& english_book);
  ~OsisBook ();
  ustring book;
private:
  WriteText * mywt;
};


class OsisChapter
{
public:
  OsisChapter (WriteText * wt, const ustring& book, unsigned int chapter_in);
  ~OsisChapter ();
  unsigned int chapter;
private:
  WriteText * mywt;
};


class OsisVerse
{
public:
  OsisVerse (WriteText * wt, const ustring& book, unsigned int chapter, const ustring& verse);
  ~OsisVerse ();
private:
  WriteText * mywt;
};


class OsisParagraph
{
public:
  OsisParagraph (WriteText * wt);
  ~OsisParagraph ();
private:
  WriteText * mywt;
};


#endif
