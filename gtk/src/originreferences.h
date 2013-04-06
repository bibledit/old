/*
** Copyright (©) 2003-2013 Teus Benschop.
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


#ifndef INCLUDED_ORIGINREFERENCES_H
#define INCLUDED_ORIGINREFERENCES_H


#include <gtk/gtk.h>
#include "ustring.h"
#include "types.h"


class OriginReferences
{
public:
  OriginReferences (const ustring& project, 
                    bool affectfootnotes, bool affectendnotes, bool affectxrefs, 
                    OriginReferencesActionType action,
                    bool includebook, bool includechapter, const ustring& separator, bool includeverse, const ustring& suffix,
                    bool gui);
  ~OriginReferences ();
protected:
private:
  void get_relevant_markers ();
  void handle_notes (ustring& line, 
                     int book, int chapter, const ustring& verse, 
                     const ustring& note_opener, const ustring& note_closer, 
                     const ustring& origin_ref_opener, const ustring& origin_ref_closer,
                     const ustring& text_opener, const ustring& text_closer);
  void remove_reference (ustring& line, const ustring& opener, const ustring& closer);
  void add_reference (ustring& line, const ustring& opener, const ustring& reference);
  void add_text_marker (ustring& line, const ustring& referenceopener, const ustring& referencecloser, const ustring& textopener, const ustring& textcloser);
  OriginReferencesActionType myaction;
  ustring stylesheet;
  ustring language;
  bool myincludebook, myincludechapter, myincludeverse;
  ustring myseparator, mysuffix;
  ustring footnote_opener;
  ustring footnote_closer;
  ustring endnote_opener;
  ustring endnote_closer;
  ustring xref_opener;
  ustring xref_closer;
  ustring foot_end_note_origin_reference_opener;
  ustring foot_end_note_origin_reference_closer;
  ustring xref_origin_reference_opener;
  ustring xref_origin_reference_closer;
  ustring foot_end_note_text_opener;
  ustring foot_end_note_text_closer;
  ustring xref_text_opener;
  ustring xref_text_closer;
  unsigned int notescount, affectedcount, finecount, errorcount;
};


ustring origin_reference_produce (bool includebook, unsigned int book, const ustring& language,
                                  bool includechapter, int chapter, const ustring& separator, 
                                  bool includeverse, const ustring& verse, const ustring& suffix,
                                  bool example);


#endif
