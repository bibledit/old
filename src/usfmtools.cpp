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
#include "utilities.h"
#include "usfm.h"
#include "usfmtools.h"
#include "gtkwrappers.h"
#include "tiny_utilities.h"
#include "categorize.h"


ustring usfm_extract(ustring & line)
// This returns the usfm from the line, e.g. \id.
// The usfm is removed from the line.
{
  ustring s = trim(line);
  ustring returnvalue = "";
  if (s.length() > 1) {
    if (s.substr(0, 1) == "\\") {
      // Sometimes markers are not followed by a space, but by another marker.
      // So check for the backslash too.
      // Sometimes markers are followed by e.g. \x immediately, without a space
      // in between. This \x has been transformed to fo:inline code already, so
      // check for the "<" character too.
      size_t endposition = s.find_first_of(" <\\,.:;”!?", 1);
      if (endposition == string::npos) {
        line.clear();
        returnvalue = s;
      } else {
        returnvalue = s.substr(0, endposition);
        line.erase(0, endposition);
        line = trim(line);
      }
    }
  }
  return returnvalue;
}

ustring usfm_extract_marker(ustring & line)
// Returns the usfm marker from the line, but without
// the first backslash, so e.g. id.
// The backslash and the usfm marker are removed from the line.
{
  ustring returnvalue = usfm_extract(line);
  if (returnvalue.length() > 0)
    returnvalue.erase(0, 1);    // Remove backslash.
  return returnvalue;
}

int usfm_search_marker_from(const ustring & line, unsigned int from)
// Searches for any usfm (whether it exists or not).
// It starts searching at position "from".
// It returns the position where it found the usfm, or string::npos is not found.
{
  return line.find("\\", from);
}

unsigned int usfm_search_marker_from_length(const ustring & line, unsigned int from)
// Returns the length of the usfm that is found at position "from".
{
  ustring s;
  s = line.substr(from, line.length());
  return usfm_extract(s).length();
}

bool usfm_search_marker(const ustring & line, ustring & marker, size_t & position, size_t & length, bool & opening_marker)
/*
Searches for any marker in "line". 
Returns true when found or else it returns false.
Puts the marker itself, without backslashes and all, into "marker".
If it is an opening marker, sets "opening_marker" true.
E.g. this is a line:
  And \nd God\nd* said:
The variables will be set so:
marker = nd;
position = 4;
length = 4;
opening_marker = true;
*/
{
  // Search for the marker.
  position = line.find("\\");
  // Not found: bail out.
  if (position == string::npos)
    return false;
  // Deal with the rest of the line.
  ustring remaining_part;
  remaining_part = line.substr(position, line.length() - position);
  if (!remaining_part.empty())
    remaining_part.erase(0, 1);
  // Get end of marker.
  size_t endposition = remaining_part.find_first_of(" *\\");
  // No endposition found: the marker is at the end of the line.
  if (endposition == string::npos)
    endposition = remaining_part.length();
  // We've a marker -> set other variables.
  marker = remaining_part.substr(0, endposition);
  opening_marker = (remaining_part.substr(endposition, 1) == " ");
  if (remaining_part.substr(endposition, 1) == "\\") {
    length = endposition + 1;
  } else {
    length = endposition + 2;
  }
  if (endposition == remaining_part.length())
    length = endposition + 1;
  return true;
}

vector < ustring > usfm_get_all_markers(const ustring & line)
// Returns all markers that are found in the line.
{
  vector < ustring > markers;
  ustring marker;
  ustring line2(line);
  marker = usfm_extract_marker_within_line(line2);
  while (!marker.empty()) {
    markers.push_back(marker);
    marker = usfm_extract_marker_within_line(line2);
  }
  return markers;
}

ustring usfm_extract_marker_within_line(ustring & line)
/*
Returns the usfm marker from the line, but without the first backslash, e.g. id.
The backslash and the usfm marker are removed from the line.
It searches within the line too.
*/
{
  ustring returnvalue = trim(usfm_extract_within_line(line));
  if (returnvalue.length() > 0)
    returnvalue.erase(0, 1);    // Remove backslash.
  return returnvalue;
}

ustring usfm_extract_within_line(ustring & line)
/*
This returns the usfm from the line, e.g. \id.
The usfm is removed from the line.
*/
{
  line = trim(line);
  ustring returnvalue;
  size_t offposition;
  offposition = line.find("\\");
  if (offposition != string::npos) {
    line.erase(0, offposition);
    size_t endposition;
    endposition = line.find_first_of(" *", 1);
    if (endposition != string::npos) {
      returnvalue = line.substr(0, ++endposition);
      line.erase(0, endposition);
    } else {
      returnvalue = line;
      line.clear();
    }
  }
  return returnvalue;
}

void split_line_on_marker(ustring marker, ustring & line)
// Inserts newlines just before amy place where "marker" occurs.
{
  marker.insert(0, "\\");
  marker.append(" ");
  size_t position = 0;
  position = line.find(marker, position);
  while (position != string::npos) {
    line.insert(position, "\n");
    position++;
    position++;
    position = line.find(marker, position);
  }
}

ustring get_usfm_id_from_file(const ustring & filename)
// Gets the Paratext ID from a file.
// Deals with an awkward Windows habit to put \xEF\xBB\xBF at the start of some textfiles.
{
  // If the file is too short, probably no USfM file.
  if (file_get_size(filename) < 20)
    return "";
  // Get contents.
  gchar *contents;
  g_file_get_contents(filename.c_str(), &contents, NULL, NULL);
  // If we got nothing, then there is no ID.
  if (!contents)
    return "";
  // Clear contents up.
  contents = de_windows_notepad(contents);
  gchar *contents_shortened = g_strndup(contents, 100);
  ParseLine parseline(contents_shortened);
  g_free(contents);
  g_free(contents_shortened);
  // Look for an ID.
  for (unsigned int i = 0; i < parseline.lines.size(); i++) {
    ustring line = trim(parseline.lines[i]);
    ustring marker = usfm_extract_marker(line);
    if (usfm_is_id(marker)) {
      try {
        return line.substr(0, 3);
      }
      catch(exception & ex) {
        cerr << ex.what() << endl;
      }
    }
  }
  return "";
}

void split_line_into_usfm_and_text(const ustring & line, ustring & marker, ustring & text)
/*
Splits all text into marker and text. 
E.g. "\v 1 In the beginning ..." is split into:
- \v 1 
- In the beginning ...
*/
{
  // Do the first split.
  text = line;
  marker = usfm_extract_marker(text);
  // If this line did not have any usfm, we're through.
  if (marker.empty())
    return;
  // Certain usfms, like for chapter and verse, need to have the numbers too.
  if (usfm_is_chapter(marker) || usfm_is_verse(marker)) {
    ustring number;
    number = number_in_string(text);
    text.erase(0, number.length());
    text = trim(text);
    marker.append(" ");
    marker.append(number);
  }
  // The backslash was extracted, so put it back.
  marker.insert(0, "\\");
}

void usfm_handle_inline_text(ustring & line, UsfmInlineMarkers * inline_markers, XmlFoBlock * block, InlineMarkersType inlinemarkerstype, set < ustring > *used_styles)
/*
1. Usfm text that has the opening marker, but not the closing marker, may result
   in malformed xslfo output. To ensure this does not occur, the opening tag
   is only placed in the xslfo output when the closing usfm is found too.
2. For inline text markers, the space following the opening marker is
   part of the marker, and not part of the word.
3. In rare cases, while comparing, it occurred that the closing marker comes
   before the opening marker. Though both markers are in, they are not in the 
   right order, and so should not be transformed else an error occurs in the
   xslfo formatter.

Variables:
line - the line that contains the usfm text to be modified.
inline_markers - all available inline text markers.
block - the xmlfo block - not always used.
inlinemarkerstype - we handle inline text for some type of document.
used_styles - the styles actually converted - not always used.
*/
{
  // Go through all markers, and look for them in the line.
  for (unsigned int i = 0; i < inline_markers->opening_markers.size(); i++) {
    // Allow for a marker to occur more than once in the line.
    bool still_working = true;
    size_t begin_position = 0;
    size_t end_position = 0;
    while (still_working) {
      still_working = false;
      // Search for a opener after the position of the previous closer.
      begin_position = line.find(inline_markers->opening_markers[i], end_position);
      if (begin_position != string::npos) {
        // Search for a closer after the position of the previous opener.
        end_position = line.find(inline_markers->closing_markers[i], begin_position);
        if (end_position != string::npos) {
          // Check that the closer comes after the opener.
          if (end_position > begin_position) {
            // At this stage we have both the opening and closing marker, 
            // and the closer comes after the opener, 
            // so we're safe to replace the markers with the xslfo tags.
            size_t position = begin_position;
            size_t length = inline_markers->opening_markers[i].length();
            ustring openingmarkers;
            switch (inlinemarkerstype) {
            case imXslFo:
              openingmarkers.clear();
              break;
            case imSword:
              openingmarkers = inline_markers->opening_sword_markup(i);
              break;
            case imOpenDocument:
              openingmarkers = inline_markers->opening_odt_markup(i);
            }
            line.replace(position, length, openingmarkers);
            position = end_position - length + openingmarkers.length();
            length = inline_markers->closing_markers[i].length();
            switch (inlinemarkerstype) {
            case imXslFo:
              break;
            case imSword:
              line.replace(position, length, inline_markers->closing_sword_markup);
              break;
            case imOpenDocument:
              line.replace(position, length, inline_markers->closing_odt_markup);
            }
            // Store marker used.
            if (used_styles)
              used_styles->insert(inline_markers->markers[i]);
            // Next iteration.
            still_working = true;
          }
        }
      }
    }
  }
}

void usfm_remove_inline_text_markers(ustring & line, UsfmInlineMarkers * inline_markers)
// Removes inline text markers.
{
  // Go through all markers, and remove them from the line.
  for (unsigned int i = 0; i < inline_markers->opening_markers.size(); i++) {
    if (line.find(inline_markers->opening_markers[i]) != string::npos) {
      replace_text(line, inline_markers->opening_markers[i], "");
      replace_text(line, inline_markers->closing_markers[i], "");
    }
  }
}

#define NUMBER_OF_MARKERS_IdentificationInformation 10
char *USFM_MARKERS_IdentificationInformation[NUMBER_OF_MARKERS_IdentificationInformation] = {
  (char *)"id",
  (char *)"ide",
  (char *)"rem",
  (char *)"h",
  (char *)"h1",
  (char *)"h2",
  (char *)"h3",
  (char *)"toc1",
  (char *)"toc2",
  (char *)"toc3"
};

#define NUMBER_OF_MARKERS_IntroductionTitlesHeadings 11
char *USFM_MARKERS_IntroductionTitlesHeadings[NUMBER_OF_MARKERS_IntroductionTitlesHeadings] = {
  (char *)"imt",
  (char *)"imt1",
  (char *)"imt2",
  (char *)"imt3",
  (char *)"imt4",
  (char *)"is",
  (char *)"is1",
  (char *)"is2",
  (char *)"is3",
  (char *)"is4",
  (char *)"imte"
};

#define NUMBER_OF_MARKERS_IntroductionParagraphsPoetry 14
char *USFM_MARKERS_IntroductionParagraphsPoetry[NUMBER_OF_MARKERS_IntroductionParagraphsPoetry] = {
  (char *)"ip",
  (char *)"ipi",
  (char *)"im",
  (char *)"imi",
  (char *)"imq",
  (char *)"ipr",
  (char *)"ipq",
  (char *)"ib",
  (char *)"iex",
  (char *)"iq",
  (char *)"iq1",
  (char *)"iq2",
  (char *)"iq3",
  (char *)"iq4"
};

#define NUMBER_OF_MARKERS_IntroductionOtherElements 8
char *USFM_MARKERS_IntroductionOtherElements[NUMBER_OF_MARKERS_IntroductionOtherElements] = {
  (char *)"iot",
  (char *)"io",
  (char *)"io1",
  (char *)"io2",
  (char *)"io3",
  (char *)"io4",
  (char *)"ior",
  (char *)"ie"
};

#define NUMBER_OF_MARKERS_Titles 11
char *USFM_MARKERS_Titles[NUMBER_OF_MARKERS_Titles] = {
  (char *)"mt",
  (char *)"mt1",
  (char *)"mt2",
  (char *)"mt3",
  (char *)"mt4",
  (char *)"mte",
  (char *)"mte1",
  (char *)"mte2",
  (char *)"mte3",
  (char *)"mte4",
  (char *)"d"
};

#define NUMBER_OF_MARKERS_Headings 15
char *USFM_MARKERS_Headings[NUMBER_OF_MARKERS_Headings] = {
  (char *)"ms",
  (char *)"ms1",
  (char *)"ms2",
  (char *)"ms3",
  (char *)"ms4",
  (char *)"mr",
  (char *)"s",
  (char *)"s1",
  (char *)"s2",
  (char *)"s3",
  (char *)"s4",
  (char *)"sr",
  (char *)"r",
  (char *)"rq",
  (char *)"sp"
};

#define NUMBER_OF_MARKERS_ChaptersAndVerses 8
char *USFM_MARKERS_ChaptersAndVerses[NUMBER_OF_MARKERS_ChaptersAndVerses] = {
  (char *)"c",
  (char *)"ca",
  (char *)"cl",
  (char *)"cp",
  (char *)"cd",
  (char *)"v",
  (char *)"va",
  (char *)"vp"
};

#define NUMBER_OF_MARKERS_Paragraphs 22
char *USFM_MARKERS_Paragraphs[NUMBER_OF_MARKERS_Paragraphs] = {
  (char *)"p",
  (char *)"m",
  (char *)"pmo",
  (char *)"pm",
  (char *)"pmc",
  (char *)"pmr",
  (char *)"pi",
  (char *)"pi1",
  (char *)"pi2",
  (char *)"pi3",
  (char *)"pi4",
  (char *)"mi",
  (char *)"nb",
  (char *)"cls",
  (char *)"b",
  (char *)"pc",
  (char *)"pr",
  (char *)"ph",
  (char *)"ph1",
  (char *)"ph2",
  (char *)"ph3",
  (char *)"ph4"
};

#define NUMBER_OF_MARKERS_Lists 5
char *USFM_MARKERS_Lists[NUMBER_OF_MARKERS_Lists] = {
  (char *)"li",
  (char *)"li1",
  (char *)"li2",
  (char *)"li3",
  (char *)"li4"
};

#define NUMBER_OF_MARKERS_PoetryElements 16
char *USFM_MARKERS_PoetryElements[NUMBER_OF_MARKERS_PoetryElements] = {
  (char *)"q",
  (char *)"q1",
  (char *)"q2",
  (char *)"q3",
  (char *)"q4",
  (char *)"qr",
  (char *)"qc",
  (char *)"qs",
  (char *)"qa",
  (char *)"qac",
  (char *)"qm",
  (char *)"qm1",
  (char *)"qm2",
  (char *)"qm3",
  (char *)"qm4",
  (char *)"b"
};

#define NUMBER_OF_MARKERS_TableElements 25
char *USFM_MARKERS_TableElements[NUMBER_OF_MARKERS_TableElements] = {
  (char *)"tr",
  (char *)"th",
  (char *)"th1",
  (char *)"th2",
  (char *)"th3",
  (char *)"th4",
  (char *)"th5",
  (char *)"thr",
  (char *)"thr1",
  (char *)"thr2",
  (char *)"thr3",
  (char *)"thr4",
  (char *)"thr5",
  (char *)"tc",
  (char *)"tc1",
  (char *)"tc2",
  (char *)"tc3",
  (char *)"tc4",
  (char *)"tc5",
  (char *)"tcr",
  (char *)"tcr1",
  (char *)"tcr2",
  (char *)"tcr3",
  (char *)"tcr4",
  (char *)"tcr5"
};

#define NUMBER_OF_MARKERS_Footnotes 12
char *USFM_MARKERS_Footnotes[NUMBER_OF_MARKERS_Footnotes] = {
  (char *)"f",
  (char *)"fe",
  (char *)"fr",
  (char *)"fk",
  (char *)"fq",
  (char *)"fqa",
  (char *)"fl",
  (char *)"fp",
  (char *)"fv",
  (char *)"ft",
  (char *)"fdc",
  (char *)"fm"
};

#define NUMBER_OF_MARKERS_CrossReferences 6
char *USFM_MARKERS_CrossReferences[NUMBER_OF_MARKERS_CrossReferences] = {
  (char *)"x",
  (char *)"xo",
  (char *)"xk",
  (char *)"xq",
  (char *)"xt",
  (char *)"xdc"
};

#define NUMBER_OF_MARKERS_ExtendedStudyNotes 9
char *USFM_MARKERS_ExtendedStudyNotes[NUMBER_OF_MARKERS_ExtendedStudyNotes] = {
  (char *)"env",
  (char *)"enw",
  (char *)"enk",
  (char *)"enc",
  (char *)"cat1",
  (char *)"cat2",
  (char *)"cat3",
  (char *)"cat4",
  (char *)"cat5"
};

#define NUMBER_OF_MARKERS_SpecialText 13
char *USFM_MARKERS_SpecialText[NUMBER_OF_MARKERS_SpecialText] = {
  (char *)"qt",
  (char *)"nd",
  (char *)"tl",
  (char *)"dc",
  (char *)"bk",
  (char *)"sig",
  (char *)"pn",
  (char *)"wj",
  (char *)"k",
  (char *)"sls",
  (char *)"ord",
  (char *)"add",
  (char *)"lit"
};

#define NUMBER_OF_MARKERS_CharacterStyles 6
char *USFM_MARKERS_CharacterStyles[NUMBER_OF_MARKERS_CharacterStyles] = {
  (char *)"no",
  (char *)"bd",
  (char *)"it",
  (char *)"bdit",
  (char *)"em",
  (char *)"sc"
};

#define NUMBER_OF_MARKERS_SpacingsAndBreaks 1
char *USFM_MARKERS_SpacingsAndBreaks[NUMBER_OF_MARKERS_SpacingsAndBreaks] = {
  (char *)"pb"
};

#define NUMBER_OF_MARKERS_SpecialFeatures 6
char *USFM_MARKERS_SpecialFeatures[NUMBER_OF_MARKERS_SpecialFeatures] = {
  (char *)"fig",
  (char *)"pro",
  (char *)"w",
  (char *)"wh",
  (char *)"wg",
  (char *)"ndx"
};

#define NUMBER_OF_MARKERS_PeripheralMaterials 15
char *USFM_MARKERS_PeripheralMaterials[NUMBER_OF_MARKERS_PeripheralMaterials] = {
  (char *)"pub",
  (char *)"toc",
  (char *)"pref",
  (char *)"intro",
  (char *)"conc",
  (char *)"glo",
  (char *)"idx",
  (char *)"maps",
  (char *)"cov",
  (char *)"spine",
  (char *)"k1",
  (char *)"k2",
  (char *)"p1",
  (char *)"p2",
  (char *)"pubinfo"
};

/*
Other markers that may occur in the text.
/restore
  This is a marker inserted by Paratext when restoring a Paratext project backup
  file, if the switch "Append this description to every retored file" is checked
  in the Restore dialogue. And so, it needs to be present in the stylesheet 
  (so that checking tools do not report a false error), but it is not really 
  part of USFM.
*/

void usfm_categorize_markers_internal(unsigned int marker_count, set < ustring > &marker_set, char *marker_definitions[], UsfmCategory category, vector < ustring > &markers, vector < UsfmCategory > &categories)
// Internal repetitive function used by usfm_categorize_markers.
{
  for (unsigned int i = 0; i < marker_count; i++) {
    set < ustring >::const_iterator found_position;
    found_position = marker_set.find(marker_definitions[i]);
    if (found_position != marker_set.end()) {
      markers.push_back(marker_definitions[i]);
      categories.push_back(category);
      marker_set.erase(found_position);
    }
  }
}

void usfm_categorize_markers(vector < ustring > &markers, vector < UsfmCategory > &categories)
// Reorders markers and puts them in the right category.
{
  // Put all markers into a set, from where they will be ordered.
  // After ordering known markers, any remaining markers will be put in 
  // category Other.
  set < ustring > set_of_markers(markers.begin(), markers.end());
  // Clear markers and categories. Here we'll put the result of ordering them.
  markers.clear();
  categories.clear();
  usfm_categorize_markers_internal(NUMBER_OF_MARKERS_IdentificationInformation, set_of_markers, USFM_MARKERS_IdentificationInformation, ucIdentificationInformation, markers, categories);
  usfm_categorize_markers_internal(NUMBER_OF_MARKERS_IntroductionTitlesHeadings, set_of_markers, USFM_MARKERS_IntroductionTitlesHeadings, ucIntroductionTitlesHeadings, markers, categories);
  usfm_categorize_markers_internal(NUMBER_OF_MARKERS_IntroductionParagraphsPoetry, set_of_markers, USFM_MARKERS_IntroductionParagraphsPoetry, ucIntroductionParagraphsPoetry, markers, categories);
  usfm_categorize_markers_internal(NUMBER_OF_MARKERS_IntroductionOtherElements, set_of_markers, USFM_MARKERS_IntroductionOtherElements, ucIntroductionOtherElements, markers, categories);
  usfm_categorize_markers_internal(NUMBER_OF_MARKERS_Titles, set_of_markers, USFM_MARKERS_Titles, ucTitles, markers, categories);
  usfm_categorize_markers_internal(NUMBER_OF_MARKERS_Headings, set_of_markers, USFM_MARKERS_Headings, ucHeadings, markers, categories);
  usfm_categorize_markers_internal(NUMBER_OF_MARKERS_ChaptersAndVerses, set_of_markers, USFM_MARKERS_ChaptersAndVerses, ucChaptersAndVerses, markers, categories);
  usfm_categorize_markers_internal(NUMBER_OF_MARKERS_Paragraphs, set_of_markers, USFM_MARKERS_Paragraphs, ucParagraphs, markers, categories);
  usfm_categorize_markers_internal(NUMBER_OF_MARKERS_Lists, set_of_markers, USFM_MARKERS_Lists, ucLists, markers, categories);
  usfm_categorize_markers_internal(NUMBER_OF_MARKERS_PoetryElements, set_of_markers, USFM_MARKERS_PoetryElements, ucPoetryElements, markers, categories);
  usfm_categorize_markers_internal(NUMBER_OF_MARKERS_TableElements, set_of_markers, USFM_MARKERS_TableElements, ucTableElements, markers, categories);
  usfm_categorize_markers_internal(NUMBER_OF_MARKERS_Footnotes, set_of_markers, USFM_MARKERS_Footnotes, ucFootnotes, markers, categories);
  usfm_categorize_markers_internal(NUMBER_OF_MARKERS_CrossReferences, set_of_markers, USFM_MARKERS_CrossReferences, ucCrossReferences, markers, categories);
  usfm_categorize_markers_internal(NUMBER_OF_MARKERS_ExtendedStudyNotes, set_of_markers, USFM_MARKERS_ExtendedStudyNotes, ucExtendedStudyNotes, markers, categories);
  usfm_categorize_markers_internal(NUMBER_OF_MARKERS_SpecialText, set_of_markers, USFM_MARKERS_SpecialText, ucSpecialText, markers, categories);
  usfm_categorize_markers_internal(NUMBER_OF_MARKERS_CharacterStyles, set_of_markers, USFM_MARKERS_CharacterStyles, ucCharacterStyles, markers, categories);
  usfm_categorize_markers_internal(NUMBER_OF_MARKERS_SpacingsAndBreaks, set_of_markers, USFM_MARKERS_SpacingsAndBreaks, ucSpacingsAndBreaks, markers, categories);
  usfm_categorize_markers_internal(NUMBER_OF_MARKERS_SpecialFeatures, set_of_markers, USFM_MARKERS_SpecialFeatures, ucSpecialFeatures, markers, categories);
  usfm_categorize_markers_internal(NUMBER_OF_MARKERS_PeripheralMaterials, set_of_markers, USFM_MARKERS_PeripheralMaterials, ucPeripheralMaterials, markers, categories);
  // Dump any remaining markers in the "Other" category.
  vector < ustring > other_markers(set_of_markers.begin(), set_of_markers.end());
  for (unsigned int i = 0; i < other_markers.size(); i++) {
    markers.push_back(other_markers[i]);
    categories.push_back(ucNonstandardStyles);
  }
}

ustring usfm_get_category_name(UsfmCategory category)
{
  ustring name;
  switch (category) {
  case ucIdentificationInformation:
    {
      name = "Identification Information";
      break;
    }
  case ucIntroductionTitlesHeadings:
    {
      name = "Introduction Titles and Headings";
      break;
    }
  case ucIntroductionParagraphsPoetry:
    {
      name = "Introduction Paragraphs and Poetry";
      break;
    }
  case ucIntroductionOtherElements:
    {
      name = "Introduction Other Elements";
      break;
    }
  case ucTitles:
    {
      name = "Titles";
      break;
    }
  case ucHeadings:
    {
      name = "Headings";
      break;
    }
  case ucChaptersAndVerses:
    {
      name = "Chapters and Verses";
      break;
    }
  case ucParagraphs:
    {
      name = "Paragraphs";
      break;
    }
  case ucLists:
    {
      name = "Lists";
      break;
    }
  case ucPoetryElements:
    {
      name = "Poetry Elements";
      break;
    }
  case ucTableElements:
    {
      name = "Table Elements";
      break;
    }
  case ucFootnotes:
    {
      name = "Footnotes";
      break;
    }
  case ucCrossReferences:
    {
      name = "Cross References";
      break;
    }
  case ucExtendedStudyNotes:
    {
      name = "Extended Study Notes";
      break;
    }
  case ucSpecialText:
    {
      name = "Special Text";
      break;
    }
  case ucCharacterStyles:
    {
      name = "Character Styles";
      break;
    }
  case ucSpacingsAndBreaks:
    {
      name = "Spacings and Breaks";
      break;
    }
  case ucSpecialFeatures:
    {
      name = "Special Features";
      break;
    }
  case ucPeripheralMaterials:
    {
      name = "Peripheral Materials";
      break;
    }
  case ucNonstandardStyles:
    {
      name = "Non-standard Styles";
      break;
    }
  }
  return name;
}

ustring usfm_get_full_opening_marker(const ustring & marker)
{
  return "\\" + marker + " ";
}

ustring usfm_get_full_closing_marker(const ustring & marker)
{
  return "\\" + marker + "*";
}

bool usfm_basic_markers_present(Usfm & usfm, bool gui)
// Returns true if the basic markers are present in usfm: id, c, v.
// Gives message if one is missing.
{
  vector < ustring > missing_styles;
  ustring style;
  bool idok;
  style = "id";
  idok = usfm.is_identifier(style) && usfm.identifier_get_subtype(style) == itBook;
  if (!idok)
    missing_styles.push_back(style + " - a book identifier");
  style = "c";
  if (!usfm.is_chapter_number(style))
    missing_styles.push_back(style + " - a chapter number");
  style = "v";
  if (!usfm.is_verse_number(style))
    missing_styles.push_back(style + " - a verse number");
  if (missing_styles.empty())
    return true;
  ustring message;
  message = "Not all basic styles are in the stylesheet.\n" "Printing cannot continue.\n" "Please add the following styles to the stylesheet:";
  for (unsigned int i = 0; i < missing_styles.size(); i++)
    message.append("\n" + missing_styles[i]);
  if (gui)
    gtkw_dialog_error(NULL, message);
  else
    cout << message << endl;
  return false;
}

ustring usfm_notes_handle_nesting(const ustring & line, const ustring & standardparagraph_opener, const ustring & standardparagraph_closer, const set < ustring > &notemarkers)
/*
Handle compliance with the following bit extracted from Usfm 2:

  By default footnote codes are not nested (i.e. they are not embedded within 
  each other). The use of each new code cancels out the 'environment' of the 
  previous. However, an alternative markup may be used requiring each internal 
  marker to be explicitly closed (e.g. \f + \fk...\fk*...\f*.). The examples 
  below include variants which use the "nested", and "non-nested" approach. The 
  \ft code is used in the non-nested markup to indicate a return to normal 
  explanatory footnote text. If the nested approach is used, each code must be 
  explicitly closed, and the unmarked text within \f ..\f* is assumed to be 
  normal footnote text.

  \f + \fk Issac:\ft In Hebrew means "laughter"\f*
  \f + \fk Issac:\fk* In Hebrew means "laughter"\f*

  \f + \fr 1.14 \fq religious festivals;\ft or \fq seasons.\f*
  \f + \fr 1.14\fr* \fq religious festivals;\fq* or \fq seasons.\fq*\f*
 
  \f + \fr 2.4 \fk The \nd Lord\nd*: \ft See \nd Lord\nd* in Word List.\f*
  \f + \fr 2.4\fr* \fk The \nd Lord\nd*:\fk* See \nd Lord\nd* in Word List.\f*
 
  \f + \fr 3.20 \fk Adam: \ft This name in Hebrew means "humanity".\f*
  \f + \fr 3.20\fr* \fk Adam:\fk* This name in Hebrew means "humanity".\f*
  
The strategy to handle this is:
* As the \ft is now being used for the formatting of the standard parargraph, 
  this marker can be disregarded here and hence removed.
* Make everything to behave as inline text. This implies everything is closed
  with an endmarker.

*/
{
  // Variables.
  ustring result;
  size_t position;

  // Work on a copy.
  ustring note(line);

  // If the note starts with the normal text marker, take it out, as this is 
  // the default format already set in the fo-block.
  if (note.find(standardparagraph_opener) == 0) {
    note.erase(0, standardparagraph_opener.length());
  }
  // Also remove any endmarkers for the normal text.
  position = note.find(standardparagraph_closer);
  while (position != string::npos) {
    note.erase(position, standardparagraph_closer.length());
    position = note.find(standardparagraph_closer);
  }

  /*
     Deal with the markers. 
     USFM allows two ways of positioning the styles. 
     One where the markers have no endmarker, and one where they have. 
     A mixture can occur too.

     For example, this code ...:
     \fr 1.7 \fq untie his sandals: \ft This was the duty of a slave.
     ... becomes:
     \fr 1.7 \fr*\fq untie his sandals: \fq*This was the duty of a slave.

     Another example, this code ...: 
     \fr 1.7 \fq untie \nd Jesus'\nd* sandals: \ft This was the duty of a slave.
     ... becomes:
     \fr 1.7 \fr*\fq untie \fq*\nd Jesus'\nd*\fq sandals: \fq*This was the duty of a slave.

     Steps:
     - Look for the next marker.
     - If it is an opening marker, 
     see if anything needs to be closed, close it, and move all to the output.
     - If it is a closing marker, 
     just copy it to the output, with the text. If anything needs to be reopened, do that too.
   */

  // Variables.
  ustring current_open_marker;
  // Deal with all note text until ready.
  while (!note.empty()) {
    // Find next marker.
    ustring marker;
    size_t length;
    bool opener;
    if (usfm_search_marker(note, marker, position, length, opener)) {
      // Another marker found.
      // If there's any text before the marker, copy that to the output.
      if (position > 0) {
        result.append(note.substr(0, position));
      }
      // Store the full marker.
      ustring full_marker = note.substr(position, length);
      // Take that text out from the note.
      note.erase(0, position + length);
      // Is the marker specific for out note?
      if (notemarkers.find(marker) != notemarkers.end()) {
        // Marker is note related: handle it.
        if (opener) {
          // It's an opening marker.
          // Close anything previous.
          if (!current_open_marker.empty()) {
            result.append(usfm_get_full_closing_marker(current_open_marker));
          }
          // Add this opener too.
          result.append(usfm_get_full_opening_marker(marker));
          // Set currently opened marker for next cycle.
          current_open_marker = marker;
        } else {
          // It is a closing marker.
          // Add it.
          result.append(usfm_get_full_closing_marker(marker));
          // Set currently opened marker for next cycle.
          current_open_marker.clear();
        }
      } else {
        // It is a foreign marker. Just copy it to the output.
        result.append(full_marker);
      }
    } else {
      // No more markers: copy remaining text to output.
      result.append(note);
      note.clear();
      // If any marker was open, close it.
      if (!current_open_marker.empty())
        result.append(usfm_get_full_closing_marker(current_open_marker));
    }
  }

  // Take out all opening and closing markers for the standard note text. 
  // They are not needed because the paragraph already defaults to this style.
  replace_text(result, standardparagraph_opener, "");
  replace_text(result, standardparagraph_closer, "");

  // Return the result.
  return result;
}

ustring get_erase_code_till_next_marker(ustring & line, size_t marker_position, size_t marker_length, bool trimcode)
// Get and erase the usfm code between this marker and the next one.
{
  line.erase(0, marker_position + marker_length);

  ustring code;

  ustring marker;
  size_t marker2_position;
  size_t marker2_length;
  bool is_opener;
  bool marker_found = usfm_search_marker(line, marker, marker2_position, marker2_length, is_opener);
  if (marker_found) {
    code = line.substr(0, marker2_position);
  } else {
    code = line;
  }

  line.erase(0, code.length());

  if (trimcode)
    code = trim(code);

  return code;
}


ustring usfm_get_verse_number (ustring& usfmcode, bool clean, bool remove)
// Gets the verse number from usfm code.
// clean: cleanup the verse number.
// remove: remove the verse code from the usfm code.
{
  ustring verse;
  
  size_t position = usfmcode.find (" ");
  if (position == string::npos) {
    verse = usfmcode;
    if (remove)
      usfmcode.clear();
  } else {
    verse = usfmcode.substr (0, position);
    if (remove)
      usfmcode.erase (0, position);
  }

  if (clean) 
    verse = trim (verse);
  
  return verse;  
}


void usfm_dissect_figure (ustring& usfmcode, const ustring& marker_text, size_t marker_length, ustring& desc, ustring& file, ustring& size, ustring& loc, ustring& copy, ustring& cap, ustring& ref)
// Gets the various bits that make up a figure.
// desc    Picture description in English. Does not get printed.
// file    Illustration filename. Does not get printed.
// size    Picture size. Does not get printed. Valid options:
//         col:  insert picture inline within current text column.
//         span: insert picture spanning text columns, at top or bottom of page.
// loc     Picture location/range. Does not get printed.
// copy    Picture copyright info. Will be used to give the appropriate picture credits.
// cap     Picture caption. This will be printed with the illustration.
// ref     Picture reference (e.g. Luke 19.5). This will be printed with the illustration.
{
  // The picture comes in the form of, e.g. "|biblesociety.gif|col||||"
  ustring rawpicture = get_erase_code_till_next_marker(usfmcode, 0, marker_length, true);

  // Remove the ending marker too, normally "fig*".
  ustring endingmarker = usfm_get_full_closing_marker(marker_text);
  if (usfmcode.find(endingmarker) == 0) {
    usfmcode.erase(0, endingmarker.length());
  }

  // Parse the figure data into its pieces.
  Parse parse(rawpicture, false, "|");
  desc.clear();
  file.clear();
  size.clear();
  loc.clear();
  copy.clear();
  cap.clear();
  ref.clear();
  for (unsigned int i = 0; i < parse.words.size(); i++) {
    if (i == 0)
      desc = parse.words[i];
    if (i == 1)
      file = parse.words[i];
    if (i == 2)
      size = parse.words[i];
    if (i == 3)
      loc = parse.words[i];
    if (i == 4)
      copy = parse.words[i];
    if (i == 5)
      cap = parse.words[i];
    if (i == 6)
      ref = parse.words[i];
  }
}


bool usfm_code_available (ustring& small_store, ustring& big_store, size_t store_size)
/*
When working with large chunks of USFM code, manipulating this code is slow.
This function aims to reduce the number operations on the big string,
so that most operations are done on the small string. And these are fast.
If the size of the small store falls below the store size, 
it transfers text from the big store to the small store.
The function returns true if there's text left in the small store.
*/
{
  if (small_store.length() < store_size) {
    if (!big_store.empty()) {
      ustring transfer = big_store.substr (0, store_size);
      big_store.erase (0, store_size);
      small_store.append (transfer);
    }
  }
  return !small_store.empty();
}


ustring usfm_get_verse_text_only (const ustring& line)
// Takes a line of USFM code, and returns the text of the verse only,
// that is, without notes or what.
{
  ustring text (line);
  size_t pos = line.find ("\\v ");
  if (pos == 0) {
    text.erase (0, 3);
    usfm_get_verse_number (text, false, true);
  }
  if (text.empty()) {
    text = "<empty>";
  } else {
    CategorizeLine cl(text);
    text = cl.verse;
  }
  return text;
}
