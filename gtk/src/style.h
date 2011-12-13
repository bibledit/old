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


#ifndef INCLUDED_STYLE_H
#define INCLUDED_STYLE_H


#include "libraries.h"
#include <glib.h> 


enum StyleType {
  stIdentifier, stNotUsedComment, stNotUsedRunningHeader, stStartsParagraph, stInlineText, 
  stChapterNumber, stVerseNumber, stFootEndNote, stCrossreference, stPeripheral,
  stPicture, stPageBreak, stTableElement, stWordlistElement
};
enum IdentifierType {
  itBook, itEncoding, itComment, itRunningHeader, 
  itLongTOC, itShortTOC, itBookAbbrev, 
  itChapterLabel, itPublishedChapterMarker, itCommentWithEndmarker
};
enum FootEndNoteType {
  fentFootnote, fentEndnote, fentStandardContent, fentContent, fentContentWithEndmarker, fentParagraph
};
enum CrossreferenceType {
  ctCrossreference, ctStandardContent, ctContent, ctContentWithEndmarker
};
enum NoteNumberingType {
  nntNumerical, nntAlphabetical, nntUserDefined
};
enum NoteNumberingRestartType {
  nnrtNever, nnrtBook, nnrtChapter
};
enum EndnotePositionType {
  eptAfterBook, eptAfterEverything, eptAtMarker
};
enum ParagraphType {
  ptMainTitle, ptSubTitle, ptSectionHeading, ptNormalParagraph
};
enum PeripheralType {
  ptPublication, ptTableOfContents, ptPreface, ptIntroduction, ptGlossary,
  ptConcordance, ptIndex, ptMapIndex, ptCover, ptSpine
};
enum TableElementType {
  tetRow, tetHeading, tetCell
};
enum WordListElementType {
  wltWordlistGlossaryDictionary, wltHebrewWordlistEntry, wltGreekWordlistEntry, 
  wltSubjectIndexEntry
};


class Style
// Manages settings for one style. DEPRECATED - Use StyleV2 instead for new code.
{
public:
  Style (const ustring& stylesheet, const ustring& style, bool write);
  ~Style ();
  ustring marker;
  ustring name;
  ustring info;
  StyleType type;
  int subtype;
  double fontsize;
  ustring italic;
  ustring bold;
  ustring underline;
  ustring smallcaps;
  bool superscript;
  ustring justification;
  double spacebefore;
  double spaceafter;
  double leftmargin;
  double rightmargin;
  double firstlineindent;
  bool spancolumns;
  unsigned int color;
  bool print;
  bool userbool1;
  bool userbool2;
  bool userbool3;
  int userint1;
  int userint2;
  int userint3;
  ustring userstring1;
  ustring userstring2;
  ustring userstring3;
private:
  ustring mystylesheet;
  bool mywrite;
};


class StyleV2
// Manages settings for one style.
{
public:
  StyleV2 (int dummy);
  ~StyleV2 ();
  ustring marker;
  ustring name;
  ustring info;
  StyleType type;
  int subtype;
  double fontsize;
  ustring italic;
  ustring bold;
  ustring underline;
  ustring smallcaps;
  bool superscript;
  ustring justification;
  double spacebefore;
  double spaceafter;
  double leftmargin;
  double rightmargin;
  double firstlineindent;
  bool spancolumns;
  unsigned int color;
  bool print;
  bool userbool1;
  bool userbool2;
  bool userbool3;
  int userint1;
  int userint2;
  int userint3;
  ustring userstring1;
  ustring userstring2;
  ustring userstring3;
};


#endif
