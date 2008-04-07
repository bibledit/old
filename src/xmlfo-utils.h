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
** Foundation, Inc.,  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
**  
*/


#ifndef INCLUDED_XMLFO_UTILS_H
#define INCLUDED_XMLFO_UTILS_H


#include "libraries.h"
#include <glib.h>
#include <gtk/gtk.h>


class XmlFoRoot
{
public:
  XmlFoRoot (vector<ustring> * lines);
  ~XmlFoRoot ();
private:
  vector<ustring> * mylines;
};


class XmlFoLayoutMasterSet
{
public:
  XmlFoLayoutMasterSet (vector<ustring> * lines, bool twocolumns);
  ~XmlFoLayoutMasterSet ();
private:
};


class XmlFoPageSequence
{
public:
  XmlFoPageSequence (vector<ustring> * lines, bool force_even_page_count);
  ~XmlFoPageSequence ();
private:
  vector<ustring> * mylines;
};


class XmlFoStaticContent
{
public:
  XmlFoStaticContent (vector<ustring> * lines, bool chapter_left, bool chapter_right);
  ~XmlFoStaticContent ();
private:
};


class XmlFoFlow
{
public:
  XmlFoFlow (vector<ustring> * lines);
  ~XmlFoFlow ();
private:
  vector<ustring> * mylines;
};


class XmlFoBlock
{
public:
  XmlFoBlock (vector<ustring> * lines, double fontsize, int lineheight,
              const ustring& italic_in, const ustring& bold_in, 
              const ustring& underline_in, const ustring& smallcaps_in,
              ustring justification,
              double spacebefore, double spaceafter,
              double leftmargin, double rightmargin,
              double firstlineindent, bool spancolumns,
              bool keepwithnext);
  ~XmlFoBlock ();
  bool italic;
  bool bold;
  bool underline;
  bool smallcaps;
private:
  vector<ustring> * mylines;
  size_t linescount;
};


#endif
