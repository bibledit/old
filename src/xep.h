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


#ifndef INCLUDED_XEP_H
#define INCLUDED_XEP_H


#include "libraries.h"
#include <glib.h>
#include <gtk/gtk.h>
#include "notecaller.h"
#include "progresswindow.h"


bool xep_convert_from_xslfo_to_pdf (const ustring& xslfo, const ustring& pdf, NoteCaller * footnotecaller, NoteCaller * crossreferencecaller);
bool xep_present ();
bool xep_convert_from_fo_to_xep (const ustring& xslfo, const ustring& xep);

class XepElastics
{
public:
  XepElastics (const ustring& filename);
  ~XepElastics ();
private:
  unsigned int minimum_y;
  unsigned int maximum_y;
  void retrieve_extends ();
  unsigned int y_get (const ustring& line);
  void y_set (ustring& line, unsigned int y);
  unsigned int y_from_get (const ustring& line);
  void y_from_set (ustring& line, unsigned int y);
  void pull_elastics (vector <ustring>& lines, unsigned int page_start, unsigned int page_end);
};

bool xep_convert_from_xep_to_pdf (const ustring& xep, const ustring& pdf);

bool xep_find_installation_package (ustring& path);
bool xep_find_license (ustring& path);
ustring xep_install (const ustring& java, const ustring& package, const ustring& license);
void xep_check ();
ustring xep_version ();


#endif
