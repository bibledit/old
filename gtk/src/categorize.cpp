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

#include "utilities.h"
#include "categorize.h"
#include "usfmtools.h"
#include "tiny_utilities.h"
#include "gwrappers.h"

CategorizeChapterVerse::CategorizeChapterVerse(const vector < ustring > &lines)
// Categorizes the lines by adding chapter and verse information to it.
// It expects each \c and \v to start a new line, i.e. the lines are "clean".
// Gives one line per verse.
{
  // Variables.
  unsigned int chapternumber = 0;
  ustring versenumber = "0";
  ustring text;
  unsigned int previous_chapter = 0;
  ustring previous_verse = "0";
  // Go through all the lines.
  for (unsigned int i = 0; i < lines.size(); i++) {
    // Extract the marker, and deal with it.
    ustring ln = lines[i];
    ustring marker = usfm_extract_marker(ln);
    // Chapter. 
    if (marker == "c") {
      ustring number_in_line = number_in_string(ln);
      // Only accept the number if it is well-formed.
      if ((ln.substr(0, number_in_line.length()) == number_in_line) && (!number_in_line.empty())) {
        chapternumber = convert_to_int(number_in_line);
        if (i == 0)
          previous_chapter = chapternumber;     // For one-chapter texts
        versenumber = "0";
      }
    }
    // Verse
    if (marker == "v") {
      versenumber = ln;
      // Make it robust, even handling cases like:
      // - \v 1-2“Moi - No space after verse number.
      size_t spaceposition = versenumber.find_first_not_of("0123456789,-ab");
      if (spaceposition != string::npos) {
        versenumber = versenumber.substr(0, spaceposition);
      }
      // Handle case that the usfm file does not contain the \c 1.
      if ((versenumber != "0") && (chapternumber == 0)) {
        chapternumber = 1;
      }
    }
    // Store data.
    if ((chapternumber != previous_chapter) || (versenumber != previous_verse)) {
      store(previous_chapter, previous_verse, text);
      text.clear();
      previous_chapter = chapternumber;
      previous_verse = versenumber;
    }
    // Save text.
    if (!text.empty())
      text.append("\n");
    text.append(lines[i]);
  }
  if (!text.empty())
    store(chapternumber, versenumber, text);
}

void CategorizeChapterVerse::store(unsigned int chapternumber, const ustring & versenumber, const ustring & text)
{
  chapter.push_back(chapternumber);
  verse.push_back(versenumber);
  line.push_back(text);
}

CategorizeLine::CategorizeLine(const ustring & data)
{
  // As the data may exist of several lines, handle those separately.
  ParseLine parseline(data);
  for (unsigned int ln = 0; ln < parseline.lines.size(); ln++) {

    // Get the line to work on.
    ustring line = parseline.lines[ln];

    // Extract the marker for this line.
    ustring marker = usfm_extract_marker(line);

    // Marker to work with for this line.
    ustring actual_marker;
    if (marker.empty())
      actual_marker = previous_marker;
    else
      actual_marker = marker;

    // Re-insert bible note markers.
    if (marker == footnote_opener(false)) {
      line.insert(0, footnote_opener(true));
    }
    if (marker == endnote_opener(false)) {
      line.insert(0, endnote_opener(true));
    }
    if (marker == xref_opener(false)) {
      line.insert(0, xref_opener(true));
    }
    // Subdivide the line into categories.
    // Each category removes something from this content, until it is empty.

    // Footnotes, endnotes.
    if (!line.empty()) {
      size_t beginposition, endposition;
      ustring opening_marker = footnote_opener(true);
      ustring closing_marker = footnote_closer();
      beginposition = line.find(opening_marker);
      endposition = line.find(closing_marker);
      while ((beginposition != string::npos) && (endposition != string::npos)) {
        ustring notetext;
        notetext = line.substr(beginposition + opening_marker.length(), endposition - beginposition - closing_marker.length());
        line.erase(beginposition, endposition - beginposition + closing_marker.length());
        clear_out_any_marker(notetext);
        append_text(note, notetext);
        beginposition = line.find(opening_marker);
        endposition = line.find(closing_marker);
      }
      opening_marker = endnote_opener(true);
      closing_marker = endnote_closer();
      beginposition = line.find(opening_marker);
      endposition = line.find(closing_marker);
      while ((beginposition != string::npos) && (endposition != string::npos)) {
        ustring notetext;
        notetext = line.substr(beginposition + opening_marker.length(), endposition - beginposition - closing_marker.length());
        line.erase(beginposition, endposition - beginposition + closing_marker.length());
        clear_out_any_marker(notetext);
        append_text(note, notetext);
        beginposition = line.find(opening_marker);
        endposition = line.find(closing_marker);
      }
    }
    // Crossreferences.
    if (!line.empty()) {
      size_t beginposition, endposition;
      ustring opening_marker = xref_opener(true);
      ustring closing_marker = xref_closer();
      beginposition = line.find(opening_marker);
      endposition = line.find(closing_marker);
      while ((beginposition != string::npos) && (endposition != string::npos)) {
        ustring referencetext;
        referencetext = line.substr(beginposition + opening_marker.length(), endposition - beginposition - closing_marker.length());
        line.erase(beginposition, endposition - beginposition + closing_marker.length());
        clear_out_any_marker(referencetext);
        append_text(ref, referencetext);
        beginposition = line.find(opening_marker);
        endposition = line.find(closing_marker);
      }
    }
    // Identifiers.
    if (!line.empty()) {
      if (is_id_marker(actual_marker)) {
        clear_out_any_marker(line);
        append_text(id, line);
        line.clear();
      }
    }
    // Introduction elements.
    if (!line.empty()) {
      if (is_intro_marker(actual_marker)) {
        clear_out_any_marker(line);
        append_text(intro, line);
        line.clear();
      }
    }
    // Headings, titles, labels.
    if (!line.empty()) {
      if (is_head_marker(actual_marker)) {
        clear_out_any_marker(line);
        append_text(head, line);
        line.clear();
      }
    }
    // Chapter text.
    if (!line.empty()) {
      if (is_chap_marker(actual_marker)) {
        clear_out_any_marker(line);
        append_text(chap, line);
        line.clear();
      }
    }
    // Extended study notes. As these use the existing footnote markers, 
    // deal with the study notes first.
    if (!line.empty()) {
      if (is_study_marker(actual_marker)) {
        clear_out_any_marker(line);
        append_text(study, line);
        line.clear();
      }
    }
    // After everything else has been removed, output the rest as main text.
    // This includes the "Verses" group, the "Paragraph Elements", and the
    // "Poetry Elements", the "Table Elements", and the "Special Text and
    // Character Styles", which have been filtered out already above.
    if (!line.empty()) {
      clear_out_any_marker(line);
      append_text(verse, line);
      line.clear();
    }
    // Store previous marker.
    previous_marker = marker;

  }
}

void CategorizeLine::append_text(ustring & container, const ustring & text)
{
  if (!container.empty())
    container.append(" ");
  container.append(text);
}

void CategorizeLine::clear_out_any_marker(ustring & line)
{
  size_t startpos = 0;
  startpos = line.find("\\", startpos);
  while (startpos != string::npos) {
    ustring marker;
    size_t endpos = line.find_first_of(" *", startpos);
    if (endpos == string::npos) {
      marker = line.substr(startpos + 1, line.length() - startpos);
    } else {
      marker = line.substr(startpos + 1, endpos - startpos - 1);
    }
    line.erase(startpos, marker.length() + 2);
    startpos++;
    startpos = line.find("\\", startpos);
  }
}

bool CategorizeLine::is_id_marker(const ustring & marker)
{
  return ((marker == "id") || (marker == "ide") || (marker == "rem") || (marker == "h") || (marker == "h1") || (marker == "h2") || (marker == "h3") || (marker == "toc") || (marker == "toc1") || (marker == "toc2")
          || (marker == "toc3"));
}

bool CategorizeLine::is_intro_marker(const ustring & marker)
{
  return ((marker == "imt") || (marker == "imt1") || (marker == "imt2") || (marker == "imt3") || (marker == "imt4") || (marker == "is") || (marker == "is1") || (marker == "is2") || (marker == "ip") || (marker == "ipi")
          || (marker == "im") || (marker == "imi") || (marker == "ipq") || (marker == "imq") || (marker == "ipr") || (marker == "ib") || (marker == "iex") || (marker == "iq") || (marker == "iq1") || (marker == "iq2")
          || (marker == "iq3") || (marker == "iot") || (marker == "io") || (marker == "io") || (marker == "io1") || (marker == "io2") || (marker == "io3") || (marker == "io4") || (marker == "imte") || (marker == "ie"));
}

bool CategorizeLine::is_head_marker(const ustring & marker)
{
  return ((marker == "mt") || (marker == "mt1") || (marker == "mt2") || (marker == "mt3") || (marker == "mt4") || (marker == "mte") || (marker == "mte1") || (marker == "mte2") || (marker == "mte3")
          || (marker == "mte4") || (marker == "ms") || (marker == "ms1") || (marker == "ms2") || (marker == "ms3") || (marker == "ms4") || (marker == "mr") || (marker == "s") || (marker == "s1") || (marker == "s2")
          || (marker == "s3") || (marker == "s4") || (marker == "sr") || (marker == "r") || (marker == "rq") || (marker == "d") || (marker == "sp"));
}

bool CategorizeLine::is_chap_marker(const ustring & marker)
{
  return ((marker == "c") || (marker == "ca") || (marker == "cl") || (marker == "cp") || (marker == "cd"));
}

bool CategorizeLine::is_study_marker(const ustring & marker)
{
  return ((marker == "env") || (marker == "enw") || (marker == "enk") || (marker == "enc"));
}

ustring CategorizeLine::footnote_opener(bool full)
{
  ustring opener;
  if (full)
    opener.append("\\");
  opener.append("f");
  if (full)
    opener.append(" ");
  return opener;
}


ustring CategorizeLine::footnote_closer()
{
  return "\\f*";
}


ustring CategorizeLine::endnote_opener(bool full)
{
  ustring opener;
  if (full)
    opener.append("\\");
  opener.append("fe");
  if (full)
    opener.append(" ");
  return opener;
}


ustring CategorizeLine::endnote_closer()
{
  return "\\fe*";
}


ustring CategorizeLine::xref_opener(bool full)
{
  ustring opener;
  if (full)
    opener.append("\\");
  opener.append("x");
  if (full)
    opener.append(" ");
  return opener;
}


ustring CategorizeLine::xref_closer()
{
  return "\\x*";
}


bool CategorizeLine::is_verse_marker(const ustring & marker)
// We take the rough and fast road and say that if the marker is nothing else,
// then it is a verse marker.
{
  bool is_verse = true;
  if (is_verse)
    if (is_id_marker(marker))
      is_verse = false;
  if (is_verse)
    if (is_intro_marker(marker))
      is_verse = false;
  if (is_verse)
    if (is_head_marker(marker))
      is_verse = false;
  if (is_verse)
    if (is_chap_marker(marker))
      is_verse = false;
  if (is_verse)
    if (is_study_marker(marker))
      is_verse = false;
  return is_verse;
}


void CategorizeLine::remove_verse_number(const ustring & versenumber)
// This removes the verse number if "versenumber" is found in the text.
{
  size_t pos = verse.find(" ");
  if (pos == string::npos)
    return;
  ustring vs = verse.substr(0, pos);
  if (vs == versenumber)
    verse.erase(0, ++pos);
}
