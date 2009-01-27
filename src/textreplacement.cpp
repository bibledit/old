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

#include "textreplacement.h"
#include "settings.h"
#include "utilities.h"

bool text_replacement_on()
{
  extern Settings *settings;
  return settings->genconfig.text_replacement_get();
}

void text_replacement_get_words(vector < ustring > &originals, vector < ustring > &replacements)
{
  // Get the parameters.
  extern Settings *settings;
  originals = settings->genconfig.text_replacement_originals_get();
  replacements = settings->genconfig.text_replacement_replacements_get();
  size_t count = MIN(originals.size(), replacements.size());

  // If there are none, initialize the words to standard values.
  if (count == 0) {

    originals.push_back("<<");
    replacements.push_back("“");

    originals.push_back(">>");
    replacements.push_back("”");

    originals.push_back("<");
    replacements.push_back("‘");

    originals.push_back(">");
    replacements.push_back("’");

    // Support for USFM !$: fixed space.
    originals.push_back("!$");
    replacements.push_back(" ");

    // Support for USFM //: line break.
    originals.push_back("//");
    replacements.push_back("\\b ");

    count = originals.size();
  }
  // Do the longer ones first, the shorter ones later.
  {
    vector < ustring > sorted_orig;
    vector < ustring > sorted_repl;
    for (unsigned int i = 0; i < count; i++) {
      if (originals[i].length() >= 5) {
        sorted_orig.push_back(originals[i]);
        sorted_repl.push_back(replacements[i]);
      }
    }
    for (unsigned int i = 0; i < count; i++) {
      if (originals[i].length() == 4) {
        sorted_orig.push_back(originals[i]);
        sorted_repl.push_back(replacements[i]);
      }
    }
    for (unsigned int i = 0; i < count; i++) {
      if (originals[i].length() == 3) {
        sorted_orig.push_back(originals[i]);
        sorted_repl.push_back(replacements[i]);
      }
    }
    for (unsigned int i = 0; i < count; i++) {
      if (originals[i].length() == 2) {
        sorted_orig.push_back(originals[i]);
        sorted_repl.push_back(replacements[i]);
      }
    }
    for (unsigned int i = 0; i < count; i++) {
      if (originals[i].length() == 1) {
        sorted_orig.push_back(originals[i]);
        sorted_repl.push_back(replacements[i]);
      }
    }
    originals = sorted_orig;
    replacements = sorted_repl;
  }
}

void text_replacement(vector < ustring > &lines)
{
  // If there is nothing to do, bail out.
  if (!text_replacement_on())
    return;

  // Get parameters.
  vector < ustring > originals;
  vector < ustring > replacements;
  text_replacement_get_words(originals, replacements);

  // Do the replacements.
  for (unsigned int ln = 0; ln < lines.size(); ln++) {
    for (unsigned int i = 0; i < originals.size(); i++) {
      replace_text(lines[ln], originals[i], replacements[i]);
    }
  }
}

void text_replacement(ustring & line)
{
  // If there is nothing to do, bail out.
  if (!text_replacement_on())
    return;

  // Get parameters.
  vector < ustring > originals;
  vector < ustring > replacements;
  text_replacement_get_words(originals, replacements);

  // Do the replacements.
  for (unsigned int i = 0; i < originals.size(); i++) {
    replace_text(line, originals[i], replacements[i]);
  }
}
