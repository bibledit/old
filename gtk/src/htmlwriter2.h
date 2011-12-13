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


#ifndef INCLUDED_HTMLWRITER2_H
#define INCLUDED_HTMLWRITER2_H


#include "libraries.h"
#include <libxml/xmlwriter.h>
#include "types.h"


class HtmlWriter2
{
public:
  HtmlWriter2 (const ustring& title);
  ~HtmlWriter2 ();
  void heading_open (unsigned int level);
  void heading_close ();
  void paragraph_open ();
  void paragraph_close ();
  void form_open (const gchar * name, const gchar * action, const gchar * method);
  void form_close ();
  void input_open (const gchar * name, const gchar * type, unsigned int size, const gchar * value);
  void input_close ();
  void text_add (const ustring& text);
  void hyperlink_add (const ustring& url, const ustring& text);
  void bold_open();
  void bold_close();
  void italics_open ();
  void italics_close ();
  void finish ();
  ustring html;
private:
  xmlBufferPtr buffer;
  xmlTextWriterPtr writer;
  bool heading_opened;
  bool paragraph_opened;
  int bold_level;
  int italics_level;
};


#endif
