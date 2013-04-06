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


#ifndef INCLUDED_USFMTOOLS_H
#define INCLUDED_USFMTOOLS_H


#include "libraries.h"
#include "usfm.h"
#include "usfm-inline-markers.h"
#include "xmlfo-utils.h"


enum UsfmCategory {ucIdentificationInformation, 
                   ucIntroductionTitlesHeadings,
                   ucIntroductionParagraphsPoetry,
                   ucIntroductionOtherElements,
                   ucTitles,
                   ucHeadings,
                   ucChaptersAndVerses, 
                   ucParagraphs, 
                   ucLists, 
                   ucPoetryElements, 
                   ucTableElements,
                   ucFootnotes, 
                   ucCrossReferences, 
                   ucExtendedStudyNotes, 
                   ucSpecialText,
                   ucCharacterStyles,
                   ucSpacingsAndBreaks,
                   ucSpecialFeatures,
                   ucPeripheralMaterials, 
                   ucNonstandardStyles};
enum InlineMarkersType {imXslFo, imSword, imOpenDocument};                   
                   

ustring usfm_extract_marker (ustring & line);
int usfm_search_marker_from (const ustring & line, unsigned int from);
unsigned int usfm_search_marker_from_length (const ustring & line, unsigned int from);
bool usfm_search_marker (const ustring& line, ustring& marker, size_t& position, size_t& length, bool& opening_marker);
vector<ustring> usfm_get_all_markers (const ustring& line);
ustring usfm_extract_marker_within_line (ustring & line);
ustring usfm_extract_within_line (ustring & line);
void split_line_on_marker (ustring marker, ustring& line);                   
ustring get_usfm_id_from_file (const ustring & filename);
void split_line_into_usfm_and_text (const ustring& line, ustring& marker, ustring& text);
void usfm_handle_inline_text (ustring& line, UsfmInlineMarkers * inline_markers, XmlFoBlock * block, InlineMarkersType inlinemarkerstype, set<ustring> * used_styles);
void usfm_remove_inline_text_markers (ustring& line, UsfmInlineMarkers * inline_markers);
void usfm_categorize_markers (vector<ustring>& markers, vector<UsfmCategory>& categories);
ustring usfm_get_category_name (UsfmCategory category);
ustring usfm_get_full_opening_marker (const ustring& marker);
ustring usfm_get_full_closing_marker (const ustring& marker);
bool usfm_basic_markers_present (Usfm& usfm, bool gui);
ustring usfm_notes_handle_nesting (const ustring& line, const ustring& standardparagraph_opener, const ustring& standardparagraph_closer, const set<ustring>& notemarkers);
ustring get_erase_code_till_next_marker (ustring& line, size_t marker_position, size_t marker_length, bool trimcode);
ustring usfm_get_verse_number (ustring& usfmcode, bool clean, bool remove);
void usfm_dissect_figure (ustring& usfmcode, const ustring& marker_text, size_t marker_length, ustring& desc, ustring& file, ustring& size, ustring& loc, ustring& copy, ustring& cap, ustring& ref);
bool usfm_code_available (ustring& small_store, ustring& big_store, size_t store_size);
ustring usfm_get_verse_text_only (const ustring& line);


#endif
