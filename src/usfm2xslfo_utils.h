/*
** Copyright (©) 2003-2008 Teus Benschop.
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
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
**  
*/


#ifndef INCLUDED_USFM2XSLFO_UTILS_H
#define INCLUDED_USFM2XSLFO_UTILS_H


#include "libraries.h"
#include "style.h"


enum Usfm2XslFoStyleType {
  u2xtIdentifierBook,
  u2xtIdentifierComment,
  u2xtIdentifierRunningHeader,
  u2xtIdentifierLongTOC,
  u2xtIdentifierShortTOC,
  u2xtIdentifierBookAbbreviation,
  u2xtParagraphMainTitle,
  u2xtParagraphSubTitle,
  u2xtParagraphSectionHeading,
  u2xtParagraphNormalParagraph,
  u2xtInlineText,
  u2xtChapterNumber,
  u2xtVerseNumber,
  u2xtFootNoteStart,
  u2xtEndNoteStart,
  u2xtFootEndNoteStandardContent,
  u2xtFootEndNoteContent,
  u2xtFootEndNoteContentWithEndmarker,
  u2xtFootEndNoteParagraph,
  u2xtCrossreferenceStart,
  u2xtCrossreferenceStandardContent,
  u2xtCrossreferenceContent,
  u2xtCrossreferenceContentWithEndmarker,
  u2xtPublication,
  u2xtTableOfContents,
  u2xtPreface,
  u2xtIntroduction,
  u2xtGlossary,
  u2xtConcordance,
  u2xtIndex,
  u2xtMapIndex,
  u2xtCover,
  u2xtSpine,
  u2xtPicture,
  u2xtPageBreak,
  u2xtTableElementRow,
  u2xtTableElementHeading,
  u2xtTableElementCell,
  u2xtColumnBalancer,
  u2xtElastic,
  u2xtDumpEndnotes,
  u2xtInsertion,
  u2xtDeletion,
  u2xtLineSpacing,
  u2xtKeepOnPage,
  u2xtFontFamilySizeLineHeight
};


class Usfm2XslFoStyle
{
public:
  Usfm2XslFoStyle (const ustring& marker_in);
  ~Usfm2XslFoStyle ();
  ustring marker;
  Usfm2XslFoStyleType type;
  double fontsize;
  int fontpercentage;
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
  bool print_chapter_number_at_first_verse;
  bool book_starts_new_page;
  bool book_starts_odd_page;
  bool bible_note_apocrypha_only;
  bool spacious_notecaller;
  bool print_in_left_running_header;
  bool print_in_right_running_header;
  NoteNumberingType note_numbering_type;
  ustring note_numbering_user_sequence;
  NoteNumberingRestartType note_numbering_restart_type;
  EndnotePositionType endnote_position_type;
  ustring dump_endnotes_upon_encountering_this_marker;
  int line_height_percentage;
  int table_column_number;
private:
};


vector <Usfm2XslFoStyle> usfm2xslfo_read_stylesheet (const ustring& stylesheet);


class XslFoTableCell
{
public:
  XslFoTableCell (Usfm2XslFoStyle * style_in, const ustring& text_in);
  Usfm2XslFoStyle * style;
  ustring text;
};


class XslFoTableRow
{
public:
  XslFoTableRow (int dummy);
  vector <XslFoTableCell> cells;
};


class XslFoPortion
{
public:
  XslFoPortion (unsigned int book_in, unsigned int chapter_from_in, const ustring& verse_from_in, unsigned int chapter_to_in, const ustring& verse_to_in);
  unsigned int book;
  unsigned int chapter_from;
  ustring verse_from;
  unsigned int chapter_to;
  ustring verse_to;
};


#endif
