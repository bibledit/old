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
#include "utilities.h"
#include "referenceutils.h"
#include "directories.h"
#include "constants.h"
#include "gwrappers.h"
#include "settings.h"
#include "books.h"
#include "unixwrappers.h"
#include "projectutils.h"
#include "tiny_utilities.h"
#include <glib.h>
#include "bible.h"


ustring references_hidden_ones_get_filename(const ustring & project)
{
  return gw_build_filename(directories_get_projects(), project, "hidden-references");
}


vector < ustring > references_hidden_ones_load(const ustring& project)
// Loads the references that are hidden in this project.
{
  ReadText rt(references_hidden_ones_get_filename(project), true, false);
  return rt.lines;
}


void references_hidden_ones_save(const ustring& project, vector < ustring > &references)
// Saves the references that are hidden in this project.
{
  write_lines(references_hidden_ones_get_filename(project), references);
}


bool text_contains_reference(const ustring & text)
/*
Finds out whether the text looks like a reference.
A reference, e.g. Mt.5.5 or Mt.5:5 or John 10:5 follows a certain pattern,
while going through it. Some references are given without the bookname, e.g.
"10.5". Handle these too.
Patterns:
- digit, dot/colon, digit.
*/
{
  return unix_fnmatch("*[0-9][:,.][0-9]*", text);
}


bool text_starts_chapter_verse(const ustring & text)
/*
Returns true if the text starts with a chapter/verse pair.
It looks for these patterns:
- digit, digit, digit, dot/colon, digit
-        digit, digit, dot/colon, digit
-               digit, dot/colon, digit

*/
{
  if (unix_fnmatch("[0-9][0-9][0-9][:.][0-9]*", text))
    return true;
  if (unix_fnmatch("[0-9][0-9][:.][0-9]*", text))
    return true;
  if (unix_fnmatch("[0-9][:.][0-9]*", text))
    return true;
  return false;
}


ReferencesScanner::ReferencesScanner(const ustring & language, int book, const ustring & text)
/*
This extracts references from the text. 
It expects a mix of booknames, abbreviated booknames and chapter and text information.

language: language of the abbreviations
book: currently opened book
text: text to process

Provides: vector of Reference objects.
*/
{
  // If the length of the text is too short for a reference, bail out.
  if (text.length() < 3)
    return;

  // Save and initialize variables.
  current_book = book;
  in_chapter_verse_state = false;
  maximum_book_name_length = 0;

  // Produce a list of all booknames, both full names and abbreviations, 
  // sorted from longest name to shortest name.
  // Make a mapping between alle names, full ones and abbreviations, to book ids.
  {
    vector < unsigned int >bookids = books_type_to_ids(btUnknown);
    for (unsigned int i = 0; i < bookids.size(); i++) {
      ustring bookname;
      bookname = books_id_to_abbreviation(language, bookids[i]);
      if (bookname.length() > maximum_book_name_length)
        maximum_book_name_length = bookname.length();
      all_book_names.push_back(bookname);
      names_or_abbreviations_to_books[bookname] = bookids[i];
      bookname = books_id_to_name(language, bookids[i]);
      if (bookname.length() > maximum_book_name_length)
        maximum_book_name_length = bookname.length();
      all_book_names.push_back(bookname);
      names_or_abbreviations_to_books[bookname] = bookids[i];
    }
    vector < unsigned int >booklengths;
    for (unsigned int i = 0; i < all_book_names.size(); i++) {
      booklengths.push_back(maximum_book_name_length - all_book_names[i].length());
    }
    quick_sort(booklengths, all_book_names, 0, booklengths.size());
  }

  // Process all tokens.
  for (unsigned int pointer = 0; pointer <= text.length(); pointer++) {

    // Scan the next token.
    if (pointer == text.length())
      current_ustring_token.clear();
    else
      current_ustring_token = text.substr(pointer, 1);
    current_unichar_token = g_utf8_get_char(current_ustring_token.c_str());

    // Find out whether this new token brings us in a chapter/verse state.
    bool previous_state = in_chapter_verse_state;
    if (in_chapter_verse_state) {

      // If we are in the chapter/verse state we remain in it as long as any of
      // the following tokens are scanned: any numeral, a dot (.), a comma (,),
      // a colon (:), or a hyphen (-).
      // If anything else is scanned, the chapter/verse state is left.
      // And if a book name starts at the scanned token, then the state if left too.
      if (g_unichar_isdigit(current_unichar_token)) ;
      else if (current_ustring_token == ".") ;
      else if (current_ustring_token == ",") ;
      else if (current_ustring_token == ":") ;
      else if (current_ustring_token == "-") ;
      else
        in_chapter_verse_state = false;
      if (in_chapter_verse_state) {
        ustring chunk_ahead;
        if (pointer < text.length()) {
          chunk_ahead = text.substr(pointer, maximum_book_name_length);
          for (unsigned int i = 0; i < all_book_names.size(); i++) {
            if (chunk_ahead.find(all_book_names[i]) == 0) {
              in_chapter_verse_state = false;
              break;
            }
          }
        }
      }

    } else {

      // If we are not in the chapter/verse state, then if a digit is encountered,
      // and that digit seems to start a chunk of text that seems to start a reference,
      // or if the text just preceding our pointer seems to indicate a book,
      // then we enter the verse/chapter state.
      if (g_unichar_isdigit(current_unichar_token)) {
        ustring chunk_ahead;
        if (pointer < text.length()) {
          chunk_ahead = text.substr(pointer, 5);
          in_chapter_verse_state = text_starts_chapter_verse(chunk_ahead);
        }
        if (!in_chapter_verse_state) {
          if (interprete_book() > 0)
            in_chapter_verse_state = true;
        }
      }

    }

    // If the end of the chapter/verse state has been reached, interprete it.
    if (in_chapter_verse_state != previous_state) {
      if (!in_chapter_verse_state) {
        interprete();
        preceding_text.clear();
        chapter_verse_text.clear();
      }
    }
    // Accumulate some text for the next iteration.
    if (in_chapter_verse_state) {
      chapter_verse_text.append(current_ustring_token);
    } else {
      preceding_text.append(current_ustring_token);
    }

  }
}

void ReferencesScanner::interprete()
{
  // Interprete the data to find the book.
  unsigned int book = interprete_book();
  if (book)
    current_book = book;

  // Interprete the data to find the chapters and verses.
  interprete_chapters_verses();
}

unsigned int ReferencesScanner::interprete_book()
// This routine finds the book, basing itself upon the data collected by the scanner.
{
  // Bail out if there is nothing to interprete. In that case the book remains as is.
  preceding_text = trim(preceding_text);
  if (preceding_text.empty())
    return 0;

  // Go through the list of book names we have, and see if the book is there.
  // Note that these books are sorted from long to short for better book matching.
  // If this were not done, we might find at times that a text like 3 John 3 was 
  // found once at "3 John" and once again at "John", the latter of which is incorrect.
  for (unsigned int i = 0; i < all_book_names.size(); i++) {
    if (all_book_names[i].length() > preceding_text.length())
      continue;
    size_t position = preceding_text.find(all_book_names[i]);
    if (position == string::npos)
      continue;
    if (position == (preceding_text.length() - all_book_names[i].length())) {
      return names_or_abbreviations_to_books[all_book_names[i]];
    }
  }
  return 0;
}

void ReferencesScanner::interprete_chapters_verses()
// This routine finds the chapters and verses in the scanned text.
/*
Some possible texts to interprete:

Text: 10.1
Interpretation: chapter 10 verse 1.
  
Text: 10:1
Interpretation: chapteer 10 verse 1.
  
Text: 2
Interpretation: chapter 2

Text:  24.1-4
Interpretation: chapter 24 verse 1 to 4.
  
Text: 24.14,15,18
Interpretation: chapter 24 verse 14, verse 15 and verse 18.

If we have a case like "Jude 10", then the "10" should be taken as the
verse number. In such cases take chapter 1 and verse 10.
In case of e.g. Genesis 1, take all verses of Genesis 1.

*/
{
  // Change colons to dots.
  replace_text(chapter_verse_text, ":", ".");

  // Get the bits before and after the first dot.
  ustring bit_before_dot;
  ustring bit_after_dot;
  size_t position = chapter_verse_text.find(".");
  if (position == string::npos) {
    bit_before_dot = chapter_verse_text;
    chapter_verse_text.clear();
  } else {
    bit_before_dot = chapter_verse_text.substr(0, position);
    chapter_verse_text.erase(0, ++position);
  }
  bit_before_dot = trim(bit_before_dot);
  bit_after_dot = trim(chapter_verse_text);
  chapter_verse_text.clear();

  // If there is only a "bit before the dot", interprete that properly.
  if (bit_after_dot.empty()) {
    interprete_single_text(bit_before_dot);
    return;
  }
  // Interprete the normal case.
  interprete_verses(bit_before_dot, bit_after_dot);

}

void ReferencesScanner::interprete_single_text(const ustring & text)
{
  if (books_id_to_one_chapter(current_book)) {
    interprete_verses("1", text);
  } else {
    extern Settings *settings;
    vector < ustring > verses = project_get_verses(settings->genconfig.project_get(), current_book, convert_to_int(text));
    for (unsigned int i = 0; i < verses.size(); i++) {
      Reference reference(current_book, convert_to_int(text), verses[i]);
      references.push_back(reference);
    }
  }
}

void ReferencesScanner::interprete_verses(const ustring & chapter, const ustring & verse)
{
  vector < unsigned int >verses = verse_range_sequence(verse);
  for (unsigned int i = 0; i < verses.size(); i++) {
    Reference reference(current_book, convert_to_int(chapter), convert_to_string(verses[i]));
    references.push_back(reference);
  }
}
