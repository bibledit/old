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


#ifndef INCLUDED_HIGHLIGHT_H
#define INCLUDED_HIGHLIGHT_H


#include "libraries.h"
#include <glib.h>
#include <gtk/gtk.h>
#include "editor_aids.h"
#include "htmlwriter2.h"


class Highlight
{
public:
  Highlight (GtkTextBuffer * buffer, 
             GtkWidget * textview, 
             const ustring& project, 
             GtkTextTag * tag,
             const ustring& verse);
  ~Highlight ();
  void determine_locations ();
  bool locations_ready;
  void highlight ();
private:
  bool interrupt_thread;
  GtkTextTag * mytag;
  GtkTextBuffer * maintextbuffer;
  GtkTextView * maintextview;
  gint main_start_offset;
  gint main_end_offset;
  vector <gint> highlightwordstarts;
  vector <gint> highlightwordends;
  vector <GtkTextBuffer *> highlightbuffers;
  vector <GtkTextView *> highlightviews;
  void searchwords (GtkTextBuffer * textbuffer, GtkTextView * textview, gint startoffset, gint endoffset);
  void searchwords_find_fast (GtkTextBuffer * textbuffer, 
                              GtkTextIter * beginbound, GtkTextIter * endbound,
                              const ustring & searchword, bool casesensitive,
                              vector <GtkTextIter>& wordstart, vector<GtkTextIter>& wordend);
  void searchwords_find_slow (GtkTextBuffer * textbuffer, 
                              GtkTextIter * beginbound, GtkTextIter * endbound,
                              const ustring & searchword,
                              bool casesensitive, bool globbing, bool matchbegin, bool matchend,
                              vector <GtkTextIter>& wordstart, vector<GtkTextIter>& wordend);
  void searchwords_in_area (GtkTextBuffer * textbuffer, 
                            vector <GtkTextIter>& start, vector<GtkTextIter>& end,
                            bool area_id, bool area_intro, bool area_heading, bool area_chapter, 
                            bool area_study, bool area_notes, bool area_xref, bool area_verse);
  void remove_previous_highlights (GtkTextBuffer * textbuffer);
};


bool searchwords_find_fast (const ustring& text, 
                            const vector <ustring>& searchwords, const vector <bool>& wholewords, const vector <bool>& casesensitives, 
                            vector <size_t>& startpositions, vector <size_t>& lengths);


#endif
