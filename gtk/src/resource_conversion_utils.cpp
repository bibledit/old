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

#include "resource_conversion_utils.h"
#include "tiny_utilities.h"
#include "gwrappers.h"
#include "progresswindow.h"
#include "roman.h"

const gchar *resource_conversion_type_to_text(ResourceConversionType type)
{
  switch (type) {
  case rctChapterStartsAtPatternVerseOneStartsAtChapterVerseStartsAtPattern:
    return "Chapter starts at a pattern, verse 1 starts at chapter, verse starts at a pattern";
  case rctEnd:
    return "";
  }
  return "";
}

ResourceConversionType resource_conversion_text_to_type(const ustring & text)
{
  for (unsigned int i = 0; i < rctEnd; i++) {
    if (text == resource_conversion_type_to_text(ResourceConversionType(i)))
      return (ResourceConversionType) i;
  }
  return rctEnd;
}

const gchar *resource_conversion_anchor_prefix()
// Every anchor that the resource converter inserts start with this prefix.
{
  return "be_anc_";
}

ustring resource_conversion_create_anchor_chapter_verse(unsigned int chapter, unsigned int verse)
// Create an anchor for the resource converter that includes chapter and verse.
{
  ustring anchor;
  anchor.append("<a name=\"");
  anchor.append(resource_conversion_anchor_prefix());
  anchor.append(convert_to_string(chapter));
  anchor.append("_");
  anchor.append(convert_to_string(verse));
  anchor.append("\"></a>");
  return anchor;
}

void resource_conversion_insert_anchors(vector < ustring > &lines, const ustring & chapter_prefix, unsigned int chapter_pattern, const ustring & chapter_suffix, const ustring & verse_prefix, unsigned int verse_pattern, const ustring & verse_suffix)
// Inserts the appropriate anchors in the lines.
{
  // Chapter patterns to look for.
  vector < ustring > chapter_patterns;
  for (unsigned int i = 0; i <= 150; i++) {
    chapter_patterns.push_back(chapter_prefix + integer_or_roman_numeral(i, chapter_pattern) + chapter_suffix);
  }

  // Verse patterns to look for.
  vector < ustring > verse_patterns;
  for (unsigned int i = 0; i <= 150; i++) {
    verse_patterns.push_back(verse_prefix + integer_or_roman_numeral(i, verse_pattern) + verse_suffix);
  }

  // Current chapter.
  unsigned int current_chapter = 0;

  // Go through all the lines, show progress.
  ProgressWindow progresswindow("Processing...", false);
  progresswindow.set_iterate(0, 1, lines.size());
  for (unsigned int i = 0; i < lines.size(); i++) {
    progresswindow.iterate();

    // Insert chapter and verse 1 anchors.    
    for (unsigned int i2 = 0; i2 < chapter_patterns.size(); i2++) {
      size_t pos = lines[i].find(chapter_patterns[i2]);
      if (pos != string::npos) {
        current_chapter = i2;
        pos += chapter_prefix.length();
        lines[i].insert(pos, resource_conversion_create_anchor_chapter_verse(current_chapter, 1));
      }
    }

    // Insert verse anchors.
    for (unsigned int i2 = 0; i2 < verse_patterns.size(); i2++) {
      size_t pos = 0;
      do {
        pos = lines[i].find(verse_patterns[i2], pos);
        if (pos != string::npos) {
          unsigned int verse = i2;
          pos += verse_prefix.length();
          ustring anchor = resource_conversion_create_anchor_chapter_verse(current_chapter, verse);
          lines[i].insert(pos, anchor);
          pos += anchor.length();
          pos += verse_patterns[i2].length();
          pos -= verse_prefix.length();
        }
      } while (pos != string::npos);
    }

  }
}

void resource_conversion_remove_anchors(vector < ustring > &lines)
// This removes any anchors that were inserted by the resource converter.
// This is so as to prevent double anchors from coming in.
{
  // The anchor prefix and suffix to look for. This looks like:
  //  <a name="be_anc_1_15"></a>
  // The "1" being the chapter, and the "15" being the verse.
  ustring prefix = resource_conversion_create_anchor_chapter_verse(0, 0);
  ustring suffix(prefix);
  size_t pos = prefix.find("0");
  prefix.erase(pos, 1000);
  pos = suffix.find_last_of("0");
  pos++;
  suffix.erase(0, pos);

  // Go through all the lines, erasing all relevant anchors.
  for (unsigned int i = 0; i < lines.size(); i++) {
    size_t pos = 0;
    do {
      pos = lines[i].find(prefix, pos);
      if (pos != string::npos) {
        size_t pos2 = lines[i].find(suffix, pos);
        if (pos2 != string::npos) {
          lines[i].erase(pos, pos2 - pos + suffix.length());
        } else {
          pos++;
        }
      }
    } while (pos != string::npos);
  }
}
