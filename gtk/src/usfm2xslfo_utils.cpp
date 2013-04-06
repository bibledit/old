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

#include "usfm2xslfo_utils.h"
#include "usfm.h"
#include "constants.h"

Usfm2XslFoStyle::Usfm2XslFoStyle(const ustring & marker_in)
{
  marker = marker_in;
  type = u2xtIdentifierComment;
  fontsize = 12;
  fontpercentage = 100;
  italic = OFF;
  bold = OFF;
  underline = OFF;
  underline = OFF;
  superscript = false;
  justification = LEFT;
  spacebefore = 0;
  spaceafter = 0;
  leftmargin = 0;
  rightmargin = 0;
  firstlineindent = 0;
  spancolumns = false;
  color = 0;
  print = true;
  print_chapter_number_at_first_verse = false;
  book_starts_new_page = false;
  book_starts_odd_page = false;
  bible_note_apocrypha_only = false;
  print_in_left_running_header = false;
  print_in_right_running_header = false;
  note_numbering_type = nntNumerical;
  note_numbering_restart_type = nnrtChapter;
  endnote_position_type = eptAfterEverything;
  table_column_number = 1;
  restart_paragraph = false;
}

Usfm2XslFoStyle::~Usfm2XslFoStyle()
{
}

vector < Usfm2XslFoStyle > usfm2xslfo_read_stylesheet(const ustring & stylesheet)
/*
This function reads a stylesheet,
and puts all these styles in the right format.
*/
{
  vector < Usfm2XslFoStyle > styles;
  Usfm usfm(stylesheet);
  for (unsigned int i = 0; i < usfm.styles.size(); i++) {
    Usfm2XslFoStyle style(usfm.styles[i].marker);
    style.fontsize = usfm.styles[i].fontsize;
    style.fontpercentage = 100;
    style.italic = usfm.styles[i].italic;
    style.bold = usfm.styles[i].bold;
    style.underline = usfm.styles[i].underline;
    style.smallcaps = usfm.styles[i].smallcaps;
    style.superscript = usfm.styles[i].superscript;
    style.justification = usfm.styles[i].justification;
    style.spacebefore = usfm.styles[i].spacebefore;
    style.spaceafter = usfm.styles[i].spaceafter;
    style.leftmargin = usfm.styles[i].leftmargin;
    style.rightmargin = usfm.styles[i].rightmargin;
    style.firstlineindent = usfm.styles[i].firstlineindent;
    style.spancolumns = usfm.styles[i].spancolumns;
    style.color = usfm.styles[i].color;
    style.print = usfm.styles[i].print;
    switch (usfm.styles[i].type) {
    case stIdentifier:
      {
        switch (usfm.styles[i].subtype) {
        case itBook:
          {
            style.type = u2xtIdentifierBook;
            style.book_starts_new_page = usfm.styles[i].userbool1;
            style.book_starts_odd_page = usfm.styles[i].userbool2;
            break;
          }
        case itEncoding:
        case itComment:
          {
            style.type = u2xtIdentifierComment;
            break;
          }
        case itRunningHeader:
          {
            style.type = u2xtIdentifierRunningHeader;
            style.print_in_left_running_header = usfm.styles[i].userbool2;
            style.print_in_right_running_header = usfm.styles[i].userbool3;
            break;
          }
        case itLongTOC:
          {
            style.type = u2xtIdentifierLongTOC;
            break;
          }
        case itShortTOC:
          {
            style.type = u2xtIdentifierShortTOC;
            break;
          }
        case itBookAbbrev:
          {
            style.type = u2xtIdentifierBookAbbreviation;
            break;
          }
        case itChapterLabel:
          {
            style.type = u2xtChapterLabel;
            break;
          }
        case itPublishedChapterMarker:
          {
            style.type = u2xtPublishedChapterMarker;
            break;
          }
        case itCommentWithEndmarker:
          {
            style.type = u2xtIdentifierCommentWithEndmarker;
            break;
          }
        }
        break;
      }
    case stNotUsedComment:
    case stNotUsedRunningHeader:
      {
        break;
      }
    case stStartsParagraph:
      {
        switch (usfm.styles[i].subtype) {
        case ptMainTitle:
          {
            style.type = u2xtParagraphMainTitle;
            break;
          }
        case ptSubTitle:
          {
            style.type = u2xtParagraphSubTitle;
            break;
          }
        case ptSectionHeading:
          {
            style.type = u2xtParagraphSectionHeading;
            break;
          }
        case ptNormalParagraph:
          {
            style.type = u2xtParagraphNormalParagraph;
            break;
          }
        }
        break;
      }
    case stInlineText:
      {
        style.type = u2xtInlineText;
        break;
      }
    case stChapterNumber:
      {
        style.type = u2xtChapterNumber;
        style.print_chapter_number_at_first_verse = usfm.styles[i].userbool1;
        style.print_in_left_running_header = usfm.styles[i].userbool2;
        style.print_in_right_running_header = usfm.styles[i].userbool3;
        break;
      }
    case stVerseNumber:
      {
        style.type = u2xtVerseNumber;
        style.restart_paragraph = usfm.styles[i].userbool1;
        break;
      }
    case stFootEndNote:
      {
        switch (usfm.styles[i].subtype) {
        case fentFootnote:
          {
            style.type = u2xtFootNoteStart;
            style.note_numbering_type = (NoteNumberingType) usfm.styles[i].userint1;
            style.note_numbering_restart_type = (NoteNumberingRestartType) usfm.styles[i].userint2;
            style.note_numbering_user_sequence = usfm.styles[i].userstring1;
            break;
          }
        case fentEndnote:
          {
            style.type = u2xtEndNoteStart;
            style.note_numbering_type = (NoteNumberingType) usfm.styles[i].userint1;
            style.endnote_position_type = (EndnotePositionType) usfm.styles[i].userint2;
            style.note_numbering_user_sequence = usfm.styles[i].userstring1;
            style.dump_endnotes_upon_encountering_this_marker = usfm.styles[i].userstring2;
            break;
          }
        case fentStandardContent:
          {
            style.type = u2xtFootEndNoteStandardContent;
            style.bible_note_apocrypha_only = usfm.styles[i].userbool1;
            break;
          }
        case fentContent:
          {
            style.type = u2xtFootEndNoteContent;
            style.bible_note_apocrypha_only = usfm.styles[i].userbool1;
            break;
          }
        case fentContentWithEndmarker:
          {
            style.type = u2xtFootEndNoteContentWithEndmarker;
            style.bible_note_apocrypha_only = usfm.styles[i].userbool1;
            break;
          }
        case fentParagraph:
          {
            style.type = u2xtFootEndNoteParagraph;
            style.bible_note_apocrypha_only = usfm.styles[i].userbool1;
            break;
          }
        }
        break;
      }
    case stCrossreference:
      {
        switch (usfm.styles[i].subtype) {
        case ctCrossreference:
          {
            style.type = u2xtCrossreferenceStart;
            style.note_numbering_type = (NoteNumberingType) usfm.styles[i].userint1;
            style.note_numbering_restart_type = (NoteNumberingRestartType) usfm.styles[i].userint2;
            style.note_numbering_user_sequence = usfm.styles[i].userstring1;
            break;
          }
        case ctStandardContent:
          {
            style.type = u2xtCrossreferenceStandardContent;
            style.bible_note_apocrypha_only = usfm.styles[i].userbool1;
            break;
          }
        case ctContent:
          {
            style.type = u2xtCrossreferenceContent;
            style.bible_note_apocrypha_only = usfm.styles[i].userbool1;
            break;
          }
        case ctContentWithEndmarker:
          {
            style.type = u2xtCrossreferenceContentWithEndmarker;
            style.bible_note_apocrypha_only = usfm.styles[i].userbool1;
            break;
          }
        }
        break;
      }
    case stPeripheral:
      {
        switch (usfm.styles[i].subtype) {
        case ptPublication:
          {
            style.type = u2xtPublication;
            break;
          }
        case ptTableOfContents:
          {
            style.type = u2xtTableOfContents;
            break;
          }
        case ptPreface:
          {
            style.type = u2xtPreface;
            break;
          }
        case ptIntroduction:
          {
            style.type = u2xtIntroduction;
            break;
          }
        case ptGlossary:
          {
            style.type = u2xtGlossary;
            break;
          }
        case ptConcordance:
          {
            style.type = u2xtConcordance;
            break;
          }
        case ptIndex:
          {
            style.type = u2xtIndex;
            break;
          }
        case ptMapIndex:
          {
            style.type = u2xtMapIndex;
            break;
          }
        case ptCover:
          {
            style.type = u2xtCover;
            break;
          }
        case ptSpine:
          {
            style.type = u2xtSpine;
            break;
          }
        }
        break;
      }
    case stPicture:
      {
        style.type = u2xtPicture;
        break;
      }
    case stPageBreak:
      {
        style.type = u2xtPageBreak;
        break;
      }
    case stTableElement:
      {
        switch (usfm.styles[i].subtype) {
        case tetRow:
          {
            style.type = u2xtTableElementRow;
            break;
          }
        case tetHeading:
          {
            style.type = u2xtTableElementHeading;
            style.table_column_number = usfm.styles[i].userint1;
            break;
          }
        case tetCell:
          {
            style.type = u2xtTableElementCell;
            style.table_column_number = usfm.styles[i].userint1;
            break;
          }
        }
        break;
      }
    case stWordlistElement:
      {
        switch (usfm.styles[i].subtype) {
        case wltWordlistGlossaryDictionary:
          {
            style.type = u2xtGeneralWordlistEntry;
            style.wordlist_entry_addition = usfm.styles[i].userstring1;
            break;
          }
        case wltHebrewWordlistEntry:
          {
            style.type = u2xtHebrewWordlistEntry;
            style.wordlist_entry_addition = usfm.styles[i].userstring1;
            break;
          }
        case wltGreekWordlistEntry:
          {
            style.type = u2xtGreekWordlistEntry;
            style.wordlist_entry_addition = usfm.styles[i].userstring1;
            break;
          }
        case wltSubjectIndexEntry:
          {
            style.type = u2xtSubjectIndexEntry;
            style.wordlist_entry_addition = usfm.styles[i].userstring1;
            break;
          }
        }
        break;
      }
    }
    styles.push_back(style);
  }
  return styles;
}

XslFoTableCell::XslFoTableCell(Usfm2XslFoStyle * style_in, const ustring & text_in)
{
  style = style_in;
  text = text_in;
}

XslFoTableRow::XslFoTableRow(int dummy)
{
}

ChapterLabel::ChapterLabel(unsigned int book_in, unsigned int chapter_in, ustring label_in)
{
  book = book_in;
  chapter = chapter_in;
  label = label_in;
}
