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


#include "libraries.h"
#include <glib.h>
#include "highlight.h"
#include "utilities.h"
#include "gwrappers.h"
#include "usfmtools.h"
#include "categorize.h"
#include "screen.h"
#include "settings.h"
#include "date_time_utils.h"
#include "xmlutils.h"


Highlight::Highlight(GtkTextBuffer * buffer, GtkWidget * textview, const ustring & project, GtkTextTag * tag, const ustring & verse)
{
  // Save and initialize variables.
  maintextbuffer = buffer;
  maintextview = GTK_TEXT_VIEW(textview);
  mytag = tag;
  locations_ready = false;
  interrupt_thread = false;

  // Remove any previous highlights.  
  remove_previous_highlights(maintextbuffer);

  // Determine the boundaries between which to highlight,
  // in order to highlight only the words that are within the right verse.
  {
    GtkTextIter startiter;
    GtkTextIter enditer;
    gtk_text_buffer_get_start_iter(maintextbuffer, &startiter);
    gtk_text_buffer_get_end_iter(maintextbuffer, &enditer);
    GtkTextIter iter = startiter;
    bool started = false;
    bool ended = false;
    bool start = true;
    ustring verse_style = style_get_verse_marker(project);
    while (!gtk_text_iter_is_end(&iter)) {
      ustring paragraph_style, character_style;
      get_styles_at_iterator(iter, paragraph_style, character_style);
      if (start || (character_style == verse_style)) {
        ustring verse_at_iter = get_verse_number_at_iterator(iter, verse_style, "", NULL);
        if (verse == verse_at_iter) {
          if (!started) {
            started = true;
            startiter = iter;
          }
        } else {
          if (started) {
            if (!ended) {
              ended = true;
              enditer = iter;
            }
          }
        }
      }
      start = false;
      gtk_text_iter_forward_char(&iter);
    }
    main_start_offset = gtk_text_iter_get_offset(&startiter);
    main_end_offset = gtk_text_iter_get_offset(&enditer);
  }
}


Highlight::~Highlight()
{
  // Set flag to interrupt the thread.
  interrupt_thread = true;
  // Wait till the locations are ready
  while (!locations_ready) {
    g_usleep(100000);
  }
  // The object destroys itself.
}


void Highlight::searchwords(GtkTextBuffer * textbuffer, GtkTextView * textview, gint startoffset, gint endoffset)
/*
This looks for the positions between startoffset and endoffset of the words to 
be highlighted highlights based upon the word that was searched for.
*/
{
  // Get the iterators to work in between.
  GtkTextIter startiter;
  GtkTextIter enditer;
  gtk_text_buffer_get_iter_at_offset(textbuffer, &startiter, startoffset);
  gtk_text_buffer_get_iter_at_offset(textbuffer, &enditer, endoffset);
  // Find the boundaries for highlighting search words within the iterators.
  extern Settings *settings;
  for (unsigned int i = 0; i < settings->session.highlights.size(); i++) {
    if (interrupt_thread)
      continue;
    vector < GtkTextIter > wordbegin;
    vector < GtkTextIter > wordend;
    if (settings->session.highlights[i].globbing || settings->session.highlights[i].matchbegin || settings->session.highlights[i].matchend) {
      // Advanced and slow highlighting.
      searchwords_find_slow(textbuffer, &startiter, &enditer, settings->session.highlights[i].word, settings->session.highlights[i].casesensitive, settings->session.highlights[i].globbing, settings->session.highlights[i].matchbegin, settings->session.highlights[i].matchend, wordbegin, wordend);
    } else {
      // Basic and fast highlighting.
      searchwords_find_fast(textbuffer, &startiter, &enditer, settings->session.highlights[i].word, settings->session.highlights[i].casesensitive, wordbegin, wordend);
    }
    // If we do searching in areas, check that the boundaries are inside the areas.
    if (settings->session.highlights[i].areatype == atSelection) {
      searchwords_in_area(textbuffer, wordbegin, wordend, settings->session.highlights[i].id, settings->session.highlights[i].intro, settings->session.highlights[i].heading, settings->session.highlights[i].chapter, settings->session.highlights[i].study, settings->session.highlights[i].notes, settings->session.highlights[i].xref, settings->session.highlights[i].verse);
    }
    // Copy any remaining iterators to the main containers.
    for (unsigned int i = 0; i < wordbegin.size(); i++) {
      highlightwordstarts.push_back(gtk_text_iter_get_offset(&wordbegin[i]));
      highlightwordends.push_back(gtk_text_iter_get_offset(&wordend[i]));
      highlightbuffers.push_back(textbuffer);
      highlightviews.push_back(textview);
    }
  }
}


void Highlight::searchwords_find_fast(GtkTextBuffer * textbuffer, GtkTextIter * beginbound, GtkTextIter * endbound, const ustring & searchword, bool casesensitive, vector < GtkTextIter > &wordstart, vector < GtkTextIter > &wordend)
// Searches for words to highlight. For simple highligthing. 
// Is much faster than the slow routine, see there fore more information.
{
  // Variable.
  GtkTextIter begin;
  GtkTextIter end;
  // Extract the line.
  ustring line = gtk_text_buffer_get_slice(textbuffer, beginbound, endbound, false);
  // Deal with case sensitivity.
  ustring case_considerate_search_word(searchword);
  if (!casesensitive)
    case_considerate_search_word = case_considerate_search_word.casefold();
  // Go through the line looking for matches.
  for (unsigned int i = 0; i < line.length(); i++) {
    if (interrupt_thread)
      continue;
    ustring compareline(line.substr(i, searchword.length()));
    // Deal with case sensitivity.
    if (!casesensitive)
      compareline = compareline.casefold();
    // Now compare.
    if (case_considerate_search_word == compareline) {
      // Get the iterators in the textbuffer that belong to this possible match.
      begin = *beginbound;
      gtk_text_iter_forward_chars(&begin, i);
      end = begin;
      gtk_text_iter_forward_chars(&end, searchword.length());
      // Add the boundaries of the word to highlight.
      wordstart.push_back(begin);
      wordend.push_back(end);
    }
  }
}


void Highlight::searchwords_find_slow(GtkTextBuffer * textbuffer, GtkTextIter * beginbound, GtkTextIter * endbound, const ustring & searchword, bool casesensitive, bool globbing, bool matchbegin, bool matchend, vector < GtkTextIter > &wordstart, vector < GtkTextIter > &wordend)
/*
Searches for words to highlight.
Problem when case insensitive searching:
  Character ﬃ was changed to ffi after casefolding, and as that one is 2
  characters longer than the original ﬃ, we ran in problems of searching
  past the line, which gave an exception in Gtk.
The solution was to determine the length of the word from the ones that are 
to highlight, not from the casefolded searchword, but the original one.
*/
{
  // Variable.
  GtkTextIter begin;
  GtkTextIter end;
  // Extract the line.
  ustring line = gtk_text_buffer_get_slice(textbuffer, beginbound, endbound, false);
  // Find all places in this line that have the search word.
  /*
     To do that properly for glob-style pattern matching, for begin/end word
     matching and case (in)sensitivity, we need to open the box of tricks.
     We produce all possible combinations for characters and lengths, e.g.
     We have this text:
     he is
     We then make the following strings from it, and see whether they match:
     "h"
     "he"
     "he "
     "he i"
     "he is"
     "e"
     "e "
     "e i"
     "e is"
     " "
     " i"
     " is"
     "i"
     "is"
     "s"
     Any string matching will then be highlighted.
   */
  // Deal with case sensitivity.
  ustring case_considerate_search_word(searchword);
  if (!casesensitive)
    case_considerate_search_word = case_considerate_search_word.casefold();
  for (unsigned int i = 0; i < line.length(); i++) {
    if (interrupt_thread)
      continue;
    ustring line2(line.substr(0, i + 1));
    for (unsigned int offposition = 0; offposition < line2.length(); offposition++) {
      // Get the line as described above.
      // We use optimization here to get the speed acceptable when we have 
      // long lines. But when globbing is done, because of the characters of 
      // glob-style matching, we don't know how long the searchword might be,
      // we do not use that optimization.
      unsigned int linelength = line2.length() - offposition;
      if (!globbing)
        if (linelength > searchword.length())
          continue;
      ustring compareline(line2.substr(offposition, linelength));
      // Deal with case sensitivity.
      if (!casesensitive)
        compareline = compareline.casefold();
      // Now compare.
      bool match = false;
      if (globbing) {
        if (g_pattern_match_simple(case_considerate_search_word.c_str(), compareline.c_str()))
          match = true;
      } else {
        if (case_considerate_search_word == compareline)
          match = true;
      }
      // Get the iterators in the textbuffer that belong to this possible match.
      if (match) {
        begin = *beginbound;
        gtk_text_iter_forward_chars(&begin, offposition);
        end = begin;
        gtk_text_iter_forward_chars(&end, searchword.length());
      }
      // Deal with begin-word matching.
      if (match) {
        if (matchbegin) {
          if (!gtk_text_iter_starts_word(&begin))
            match = false;
        }
      }
      // Deal with end-word matching.
      if (match) {
        if (matchend) {
          if (!gtk_text_iter_ends_word(&end))
            match = false;
        }
      }
      // Add the boundaries of the word to highlight.
      if (match) {
        wordstart.push_back(begin);
        wordend.push_back(end);
      }
    }
  }
}


void Highlight::searchwords_in_area(GtkTextBuffer * textbuffer, vector < GtkTextIter > &start, vector < GtkTextIter > &end, bool area_id, bool area_intro, bool area_heading, bool area_chapter, bool area_study, bool area_notes, bool area_xref, bool area_verse)
/*
Finds out whether the text within the "start" and "end" iterators is inside
one of the given areas. If not, it removes the iterator from the containers.
*/
{
  // Categorization data
  CategorizeLine categorize("");
  // Go through the iterators, starting at the end (to make erasing it easier).
  for (int it = start.size() - 1; it >= 0; it--) {
    // Get line number of the iterator.
    gint linenumber = gtk_text_iter_get_line(&start[it]);
    // Get the usfm this line starts with.
    ustring usfm;
    {
      GtkTextIter line1;
      gtk_text_buffer_get_iter_at_line(textbuffer, &line1, linenumber);
      GtkTextIter line2 = line1;
      gtk_text_iter_forward_chars(&line2, 10);
      ustring line = gtk_text_iter_get_text(&line1, &line2);
      usfm = usfm_extract_marker(line);
    }
    // See if this usfm is in one of the areas given.
    bool in_area = false;
    if (area_id)
      if (categorize.is_id_marker(usfm))
        in_area = true;
    if (area_intro)
      if (categorize.is_intro_marker(usfm))
        in_area = true;
    if (area_heading)
      if (categorize.is_head_marker(usfm))
        in_area = true;
    if (area_chapter)
      if (categorize.is_chap_marker(usfm))
        in_area = true;
    if (area_study)
      if (categorize.is_study_marker(usfm))
        in_area = true;
    // The variables "area_notes" and "area_xref" are not relevant.
    if (area_verse)
      if (categorize.is_verse_marker(usfm))
        in_area = true;
    // If not in one of the areas, remove this iterator from the container.
    if (!in_area) {
      vector < GtkTextIter >::iterator startiter = start.begin();
      vector < GtkTextIter >::iterator enditer = end.begin();
      for (int i = 0; i < it; i++) {
        startiter++;
        enditer++;
      }
      start.erase(startiter);
      end.erase(enditer);
    }
  }
}


void Highlight::remove_previous_highlights(GtkTextBuffer * textbuffer)
// Removes previous highlights.
// Ensure that removing the tags does not influence the modified status of the textbuffer.
{
  GtkTextIter startiter;
  GtkTextIter enditer;
  gtk_text_buffer_get_start_iter(textbuffer, &startiter);
  gtk_text_buffer_get_end_iter(textbuffer, &enditer);
  bool modified_status = gtk_text_buffer_get_modified(textbuffer);
  gtk_text_buffer_remove_tag(textbuffer, mytag, &startiter, &enditer);
  if (!modified_status)
    gtk_text_buffer_set_modified(textbuffer, false);
}


void Highlight::determine_locations()
// Determine the locations where to highlight text.
{
  // Call the highlight routine for the text.
  searchwords(maintextbuffer, maintextview, main_start_offset, main_end_offset);

  // Set a flag informing the main thread that the locations are available.
  locations_ready = true;
}


void Highlight::highlight()
// This does the actual highlighting.
// Ensure that applying a tag does not change the modified status of the textbuffer.
{
  bool cursor_set = false;
  for (unsigned int i = 0; i < highlightwordstarts.size(); i++) {
    GtkTextIter start, end;
    gtk_text_buffer_get_iter_at_offset(highlightbuffers[i], &start, highlightwordstarts[i]);
    gtk_text_buffer_get_iter_at_offset(highlightbuffers[i], &end, highlightwordends[i]);
    bool modified_status = gtk_text_buffer_get_modified(highlightbuffers[i]);
    gtk_text_buffer_apply_tag(highlightbuffers[i], mytag, &start, &end);
    if (!modified_status)
      gtk_text_buffer_set_modified(highlightbuffers[i], false);
    if (!cursor_set) {
      gtk_text_buffer_place_cursor(highlightbuffers[i], &start);
      screen_scroll_to_iterator(highlightviews[i], &start);
      cursor_set = true;
    }
  }
}


bool searchwords_find_fast (const ustring& text, 
                            const vector <ustring>& searchwords, const vector <bool>& wholewords, const vector <bool>& casesensitives, 
                            vector <size_t>& startpositions, vector <size_t>& lengths)
// Finds occurrences of searchwords in the text.
// text: Text to be looked through.
// searchwords: Search words to look for.
// wholewords / casesensitives: Attributes of the searchwords.
// startpositions: If non-NULL, will be filled with the positions that each searchword starts at.
// lengths: If non-NULL, will be filled with the lengths of the searchwords found.
// Returns whether one or more searchwords were found in the text.
{
  // Clear output containers.
  startpositions.clear();
  lengths.clear();

  // A textbuffer makes searching text easier in this case.
  GtkTextBuffer * textbuffer = gtk_text_buffer_new (NULL);
  gtk_text_buffer_set_text (textbuffer, text.c_str(), -1);
  GtkTextIter startiter;
  gtk_text_buffer_get_start_iter(textbuffer, &startiter);

  bool found = false;

  // Go through all words to look for.
  for (unsigned int i2 = 0; i2 < searchwords.size(); i2++) {

    // Define this search word and its parameters.
    ustring searchword = searchwords[i2];
    bool wholeword = wholewords[i2];
    bool casesensitive = casesensitives[i2];

    // Handle case sensitivity.
    ustring mytext;
    ustring mysearchword;
    if (casesensitive) {
      mytext = text;
      mysearchword = searchword;
    } else {
      mytext = text.casefold();
      mysearchword = searchword.casefold();
    }
    // Find all occurrences of the word.
    size_t position = mytext.find(mysearchword);
    while (position != string::npos) {
      bool temporally_approved = true;
      GtkTextIter approvedstart = startiter;
      GtkTextIter approvedend;
      gtk_text_iter_forward_chars(&approvedstart, position);
      approvedend = approvedstart;
      gtk_text_iter_forward_chars(&approvedend, searchword.length());
      if (wholeword) {
        if (!gtk_text_iter_starts_word(&approvedstart))
          temporally_approved = false;
        if (!gtk_text_iter_ends_word(&approvedend))
          temporally_approved = false;
      }
      if (temporally_approved) {
        found = true;
        startpositions.push_back (position);
        lengths.push_back (searchword.length());
      }
      position = mytext.find(mysearchword, ++position);
    }
  }
 
  // Free textbuffer used.
  g_object_unref (textbuffer);  

  if (found) {
    // Sort the output.
    quick_sort (startpositions, lengths, 0, startpositions.size());
    // Overlapping items need to be combined to avoid crashes.
    xml_combine_overlaps (startpositions, lengths);
  }

  // Return true if anything was found.  
  return found;
}

