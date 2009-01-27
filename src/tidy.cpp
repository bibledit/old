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

#include "libraries.h"
#include "tidy.h"
#include "utilities.h"
#include "settings.h"
#include "books.h"
#include "unixwrappers.h"
#include "tiny_utilities.h"

void tidy_pattern(ustring & text, const gchar * pattern, size_t window, const ustring & lookfor, const ustring & replacewith)
/*
This routine replaces "lookfor" with "replacewith" in "text", provided that
a certain length of text, with length "window" agrees with a certain "pattern".
*/
{
  // Bail out of nothing to do.
  if (window > text.length())
    return;
  // Tidy things up.
  for (unsigned int i = 0; i < text.length() - window; i++) {
    ustring subtext = text.substr(i, window);
    if (unix_fnmatch(pattern, subtext)) {
      replace_text(subtext, lookfor, replacewith);
      text.erase(i, window);
      text.insert(i, subtext);
    }
  }
}

void tidy_text(ustring & text)
{
  // No text: bail out.
  if (text.empty())
    return;

  // We need the general configuration for some settings it contains.
  extern Settings *settings;

  // Handle book translation.
  if (settings->genconfig.tidy_translate_get()) {
    // Retrieve the language into which to translate books.
    ProjectConfiguration *projectconfig = settings->projectconfig(settings->genconfig.project_get());
    ustring language = projectconfig->language_get();
    // Get the texts to translate, and the books.
    vector < ustring > texts = settings->genconfig.tidy_texts_get();
    vector < int >books = settings->genconfig.tidy_books_get();
    if (books.size() == texts.size()) {
      // Handle the | character.
      vector < ustring > newtexts;
      vector < ustring > newbooks;
      for (unsigned int i = 0; i < texts.size(); i++) {
        Parse parse(texts[i], false, "|");
        for (unsigned int i2 = 0; i2 < parse.words.size(); i2++) {
          newtexts.push_back(trim(parse.words[i2]));
          newbooks.push_back(books_id_to_abbreviation(language, books[i]));
        }
      }
      // Actually translate the texts, if they are there.
      for (unsigned int i = 0; i < newtexts.size(); i++) {
        replace_text(text, newtexts[i], newbooks[i]);
      }
    }
  }
  // Handle hyphen normalization.
  if (settings->genconfig.tidy_normalize_hyphens_get()) {
    replace_text(text, "–", "-");
  }
  // Handle pattern change, e.g.
  // Isai. 34. 11 - Remove the space between chapter and verse.
  if (settings->genconfig.tidy_space_between_chapter_verse_get()) {
    tidy_pattern(text, "[0-9][.] [0-9]", 4, " ", "");
    // The above pattern changes
    // Isa. 34.11. Jer. 4.23. 1 Lan. 1.1-4. Luke 3.36-38.
    // to
    // Isa. 34.11. Jer. 4.23.1 Lan. 1.1-4. Luke 3.36-38.
    // Notice that the space before "1 Lan." (1 Chronicles) was removed.
    // Insert that space again.
    tidy_pattern(text, "[0-9][.][0-9] ", 4, ".", ". ");
  }
  // Handle another pattern change:
  // 8, 13, 19, 23, 31 - Remove the space in a series of verses.
  if (settings->genconfig.tidy_space_series_verses_get()) {
    tidy_pattern(text, "[0-9][,] [0-9]", 4, " ", "");
  }
  // Handle case that a full stop should end the text.
  if (settings->genconfig.tidy_full_stop_ends_text_get()) {
    if (text.substr(text.length() - 1, 1) != ".")
      text.append(".");
  }
  // Handle pattern change:
  // Jer. 10. 12. & 51. 15. - Change the & to a ; in pattern number-dot-(space)-&-(space)-number.
  if (settings->genconfig.tidy_ampersand_semicolon_get()) {
    tidy_pattern(text, ". & [0-9]", 5, ". &", ";");
  }
  // Remove a space before a (semi)colon, etc.
  if (settings->genconfig.tidy_space_before_punctuation_get()) {
    replace_text(text, " ;", ";");
    replace_text(text, " ,", ",");
    replace_text(text, " ,", ",");
    replace_text(text, " :", ":");
  }
}
