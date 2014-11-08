/*
Copyright (©) 2003-2014 Teus Benschop.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#include <styles/logic.h>
#include <locale/translate.h>
#include <filter/string.h>


// This contains the styles logic.
// categoryText - Returns the $category as human readable text.
string styles_logic_category_text (string category)
{
  if (category == "id")  return gettext ("Identification information");
  if (category == "ith") return gettext ("Introduction titles and headings");
  if (category == "ipp") return gettext ("Introduction paragraphs and poetry");
  if (category == "ioe") return gettext ("Introduction other elements");
  if (category == "t"  ) return gettext ("Titles");
  if (category == "h"  ) return gettext ("Headings");
  if (category == "cv" ) return gettext ("Chapters and verses");
  if (category == "p"  ) return gettext ("Paragraphs");
  if (category == "l"  ) return gettext ("Lists");
  if (category == "pe" ) return gettext ("Poetry elements");
  if (category == "te" ) return gettext ("Table elements");
  if (category == "f"  ) return gettext ("Footnotes");
  if (category == "x"  ) return gettext ("Crossreferences");
  if (category == "xsn") return gettext ("Extended study notes");
  if (category == "st" ) return gettext ("Special text");
  if (category == "cs" ) return gettext ("Character styles");
  if (category == "sb" ) return gettext ("Spacings and breaks");
  if (category == "sf" ) return gettext ("Special features");
  if (category == "pm" ) return gettext ("Peripheral materials");
  return gettext ("Extra styles");
}


// Returns the $type as human readable text.
string styles_logic_type_text (int type)
{
  if (type == StyleTypeIdentifier     ) return gettext ("is an identifier");
  if (type == StyleTypeStartsParagraph) return gettext ("starts a new paragraph");
  if (type == StyleTypeInlineText     ) return gettext ("is inline text with endmarker");
  if (type == StyleTypeChapterNumber  ) return gettext ("is a chapter number");
  if (type == StyleTypeVerseNumber    ) return gettext ("is a verse number");
  if (type == StyleTypeFootEndNote    ) return gettext ("is a footnote or endnote");
  if (type == StyleTypeCrossreference ) return gettext ("is a crossreference");
  if (type == StyleTypePeripheral     ) return gettext ("is a peripheral element");
  if (type == StyleTypePicture        ) return gettext ("is a picture");
  if (type == StyleTypePageBreak      ) return gettext ("starts a new page");
  if (type == StyleTypeTableElement   ) return gettext ("is a table element");
  if (type == StyleTypeWordlistElement) return gettext ("is a word list element");
  return "--";
}


// subtypeText - Returns the $subtype as human readable text.
string styles_logic_subtype_text (int type, int subtype)
{
  if (type == StyleTypeIdentifier) {
    if (subtype == IdentifierSubtypeBook                  ) return gettext ("identifies the book");
    if (subtype == IdentifierSubtypeEncoding              ) return gettext ("identifies the encoding");
    if (subtype == IdentifierSubtypeComment               ) return gettext ("is a comment");
    if (subtype == IdentifierSubtypeRunningHeader         ) return gettext ("is a running header");
    if (subtype == IdentifierSubtypeLongTOC               ) return gettext ("is long table of contents text");
    if (subtype == IdentifierSubtypeShortTOC              ) return gettext ("is short table of contents text");
    if (subtype == IdentifierSubtypeBookAbbrev            ) return gettext ("is the book abbreviation");
    if (subtype == IdentifierSubtypeChapterLabel          ) return gettext ("is the chapter label");
    if (subtype == IdentifierSubtypePublishedChapterMarker) return gettext ("is the published chapter marker");
    if (subtype == IdentifierSubtypeCommentWithEndmarker  ) return gettext ("is a comment with an endmarker");
  }
  if (type == StyleTypeStartsParagraph) {
    if (subtype == ParagraphSubtypeMainTitle)       return gettext ("is a main title");
    if (subtype == ParagraphSubtypeSubTitle)        return gettext ("is a subtitle");
    if (subtype == ParagraphSubtypeSectionHeading)  return gettext ("is a section heading");
    if (subtype == ParagraphSubtypeNormalParagraph) return gettext ("is a normal paragraph");
  }
  if (type == StyleTypeInlineText) {
  }
  if (type == StyleTypeChapterNumber) {
  }
  if (type == StyleTypeVerseNumber) {
  }
  if (type == StyleTypeFootEndNote) {
    if (subtype == FootEndNoteSubtypeFootnote            ) return gettext ("starts a footnote");
    if (subtype == FootEndNoteSubtypeEndnote             ) return gettext ("starts an endnote");
    if (subtype == FootEndNoteSubtypeStandardContent     ) return gettext ("is standard content");
    if (subtype == FootEndNoteSubtypeContent             ) return gettext ("is content");
    if (subtype == FootEndNoteSubtypeContentWithEndmarker) return gettext ("is content with endmarker");
    if (subtype == FootEndNoteSubtypeParagraph           ) return gettext ("starts another paragraph");
  }
  if (type == StyleTypeCrossreference) {
    if (subtype == ParagraphSubtypeMainTitle      ) return gettext ("starts a crossreference");
    if (subtype == ParagraphSubtypeSubTitle       ) return gettext ("is standard content");
    if (subtype == ParagraphSubtypeSectionHeading ) return gettext ("is content");
    if (subtype == ParagraphSubtypeNormalParagraph) return gettext ("is content with endmarker");
  }
  if (type == StyleTypePeripheral) {
    if (subtype == PeripheralSubtypePublication    ) return gettext ("starts publication data");
    if (subtype == PeripheralSubtypeTableOfContents) return gettext ("starts table of contents");
    if (subtype == PeripheralSubtypePreface        ) return gettext ("starts preface");
    if (subtype == PeripheralSubtypeIntroduction   ) return gettext ("starts introduction");
    if (subtype == PeripheralSubtypeGlossary       ) return gettext ("starts concordance");
    if (subtype == PeripheralSubtypeConcordance    ) return gettext ("starts glossary");
    if (subtype == PeripheralSubtypeIndex          ) return gettext ("starts index");
    if (subtype == PeripheralSubtypeMapIndex       ) return gettext ("starts map index");
    if (subtype == PeripheralSubtypeCover          ) return gettext ("starts cover");
    if (subtype == PeripheralSubtypeSpine          ) return gettext ("starts spine");
  }
  if (type == StyleTypePicture) {
  }
  if (type == StyleTypePageBreak) {
  }
  if (type == StyleTypeTableElement) {
    if (subtype == TableElementSubtypeRow    ) return gettext ("starts a new row");
    if (subtype == TableElementSubtypeHeading) return gettext ("is a column heading");
    if (subtype == TableElementSubtypeCell   ) return gettext ("is cell data");
  }
  if (type == StyleTypeWordlistElement) {
    if (subtype == WorListElementSubtypeWordlistGlossaryDictionary) return gettext ("is a wordlist / glossary / dictionary entry");
    if (subtype == WorListElementSubtypeHebrewWordlistEntry       ) return gettext ("is a Hebrew wordlist entry");
    if (subtype == WorListElementSubtypeGreekWordlistEntry        ) return gettext ("is a Greek wordlist entry");
    if (subtype == WorListElementSubtypeSubjectIndexEntry         ) return gettext ("is a subject index entry");
  }
  return "--";
}


// Returns true if the fontsize is relevant for $type and $subtype
bool styles_logic_fontsize_is_relevant (int type, int subtype)
{
  switch (type) {
    case StyleTypeStartsParagraph : return true;
    case StyleTypeChapterNumber   : return true;
    case StyleTypeFootEndNote :
    {
      switch (subtype) {
        case FootEndNoteSubtypeStandardContent : return true;
        case FootEndNoteSubtypeParagraph       : return true;
      }
      break;
    }
    case StyleTypeCrossreference :
    {
      switch (subtype) {
        case CrossreferenceSubtypeStandardContent : return true;
      }
      break;
    }
    case StyleTypeTableElement :
    {
      switch (subtype) {
        case TableElementSubtypeHeading : return true;
        case TableElementSubtypeCell    : return true;
      }
      break;
    }
  }
  return false;
}


// Returns true if the italic, bold, etc. settings are relevant for $type and $subtype.
bool styles_logic_italic_bold_underline_smallcaps_are_relevant (int type, int subtype)
{
  switch (type) {
    case StyleTypeStartsParagraph : return true;
    case StyleTypeInlineText      : return true;
    case StyleTypeChapterNumber   : return true;
    case StyleTypeVerseNumber     : return true;
    case StyleTypeFootEndNote     : return true;
    case StyleTypeCrossreference  : return true;
    case StyleTypeTableElement :
    {
      switch (subtype) {
        case TableElementSubtypeHeading : return true;
        case TableElementSubtypeCell    : return true;
      }
      break;
    }
  }
  return false;
}


// Returns true if the italic, bold, etc. settings are fully applicable for $type and $subtype. Full means it also has inherit and toggle values.
bool styles_logic_italic_bold_underLine_smallcaps_are_full (int type, int subtype)
{
  switch (type) {
    case StyleTypeInlineText  : return true;
    case StyleTypeVerseNumber : return true;
    case StyleTypeFootEndNote :
    {
      switch (subtype) {
        case FootEndNoteSubtypeFootnote             : return true;
        case FootEndNoteSubtypeEndnote              : return true;
        case FootEndNoteSubtypeContent              : return true;
        case FootEndNoteSubtypeContentWithEndmarker : return true;
      }
      break;
    }
    case StyleTypeCrossreference :
    {
      switch (subtype) {
        case CrossreferenceSubtypeCrossreference       : return true;
        case CrossreferenceSubtypeContent              : return true;
        case CrossreferenceSubtypeContentWithEndmarker : return true;
      }
      break;
    }
  }
  return false;
}


// Returns the $value as human readable text.
string styles_logic_off_on_inherit_toggle_text (int value)
{
  if (value == ooitOff    ) return gettext ("Off");
  if (value == ooitOn     ) return gettext ("On");
  if (value == ooitInherit) return gettext ("Inherit");
  if (value == ooitToggle ) return gettext ("Toggle");
  return "--";
}


// Returns true if the superscript setting is relevant for $type and $subtype
bool styles_logic_superscript_is_relevant (int type, int subtype)
{
  switch (type) {
    case StyleTypeInlineText  : return true;
    case StyleTypeVerseNumber : return true;
    case StyleTypeFootEndNote :
    {
      switch (subtype) {
        case FootEndNoteSubtypeFootnote        : return true;
        case FootEndNoteSubtypeEndnote         : return true;
        case FootEndNoteSubtypeStandardContent : return true;
        case FootEndNoteSubtypeParagraph       : return true;
      }
      break;
    }
    case StyleTypeCrossreference :
    {
      switch (subtype) {
        case CrossreferenceSubtypeCrossreference        : return true;
        case CrossreferenceSubtypeStandardContent       : return true;
      }
      break;
    }
  }
  return false;
}


// Returns true if the paragraph treats are relevant for $type and $subtype
bool styles_logic_paragraph_treats_are_relevant (int type, int subtype)
{
  switch (type) {
    case StyleTypeStartsParagraph : return true;
    case StyleTypeChapterNumber   : return true;
    case StyleTypeFootEndNote :
    {
      switch (subtype) {
        case FootEndNoteSubtypeStandardContent : return true;
        case FootEndNoteSubtypeParagraph       : return true;
      }
      break;
    }
    case StyleTypeCrossreference :
    {
      switch (subtype) {
        case CrossreferenceSubtypeStandardContent : return true;
      }
      break;
    }
    case StyleTypeTableElement :
    {
      switch (subtype) {
        case TableElementSubtypeHeading : return true;
        case TableElementSubtypeCell    : return true;
      }
      break;
    }
  }
  return false;
}


// Returns the $value as human readable text.
string styles_logic_alignment_text (int value)
{
  if (value == AlignmentLeft   ) return gettext ("Start (Left in left-to-right script)");
  if (value == AlignmentCenter ) return gettext ("Center");
  if (value == AlignmentRight  ) return gettext ("End (Right in left-to-right script)");
  if (value == AlignmentJustify) return gettext ("Justify");
  return "--";
}


// Returns true if the columns are relevant for $type and $subtype
bool styles_logic_columns_are_relevant (int type, int subtype)
{
  if (subtype) {};
  switch (type) {
    case StyleTypeStartsParagraph : return true;
    case StyleTypeChapterNumber   : return true;
  }
  return false;
}


// Returns true if the color is relevant for $type and $subtype
bool styles_logic_color_is_relevant (int type, int subtype)
{
  switch (type) {
    case StyleTypeInlineText  : return true;
    case StyleTypeVerseNumber : return true;
    case StyleTypeFootEndNote :
    {
      switch (subtype) {
        case FootEndNoteSubtypeContent              : return true;
        case FootEndNoteSubtypeContentWithEndmarker : return true;
      }
      break;
    }
    case StyleTypeCrossreference :
    {
      switch (subtype) {
        case CrossreferenceSubtypeContent              : return true;
        case CrossreferenceSubtypeContentWithEndmarker : return true;
      }
      break;
    }
  }
  return false;
}


// Returns true if the print setting is relevant for $type and $subtype
bool styles_logic_print_is_relevant (int type, int subtype)
{
  switch (type) {
    case StyleTypeVerseNumber : return true;
    case StyleTypeFootEndNote :
    {
      switch (subtype) {
        case FootEndNoteSubtypeFootnote : return true;
        case FootEndNoteSubtypeEndnote  : return true;
      }
      break;
    }
    case StyleTypeCrossreference :
    {
      switch (subtype) {
        case CrossreferenceSubtypeCrossreference  : return true;
      }
      break;
    }
  }
  return false;
}


// Returns the function of userbool1 for type and subtype
int styles_logic_get_userbool1_function (int type, int subtype)
{
  if (type == StyleTypeChapterNumber) {
    return UserBool1PrintChapterAtFirstVerse;
  }
  if (type == StyleTypeIdentifier) {
    if (subtype == IdentifierSubtypeBook)
      return UserBool1IdStartsNewPage;
  }
  if (type == StyleTypeFootEndNote) {
    if ((subtype != FootEndNoteSubtypeFootnote) && (subtype == FootEndNoteSubtypeEndnote))
      return UserBool1NoteAppliesToApocrypha;
  }
  if (type == StyleTypeCrossreference) {
    if (subtype != CrossreferenceSubtypeCrossreference)
      return UserBool1NoteAppliesToApocrypha;
  }
  if (type == StyleTypeVerseNumber) {
    return UserBool1VerseRestartsParagraph;
  }
  return UserBool1None;
}


// Returns the text of userbool1's function.
string styles_logic_get_userbool1_text (int function)
{
  switch (function) {
    case UserBool1PrintChapterAtFirstVerse : return gettext ("Print chapter number at first verse");
    case UserBool1IdStartsNewPage          : return gettext ("Start on a new page");
    case UserBool1NoteAppliesToApocrypha   : return gettext ("Refers to the Apocrypha");
    case UserBool1VerseRestartsParagraph   : return gettext ("Restart paragraph");
  }
  return "--";
}


// Returns the function of userbool2 for type and subtype
int styles_logic_get_userbool2_function (int type, int subtype)
{
  if (type == StyleTypeIdentifier) {
    if (subtype == IdentifierSubtypeRunningHeader)
      return UserBool2RunningHeaderLeft;
  }
  if (type == StyleTypeChapterNumber) {
    return UserBool2ChapterInLeftRunningHeader;
  }
  return UserBool2None;
}


// Returns the text of userbool2's function.
string styles_logic_get_userbool2_text (int function)
{
  switch (function) {
    case UserBool2IdStartsOddPage            : return gettext ("New page starts with an odd number (not implemented due to limitations in OpenDocument)");
    case UserBool2ChapterInLeftRunningHeader : return gettext ("Print chapter number in the running header of the left page");
    case UserBool2RunningHeaderLeft          : return gettext ("Print this in the running header of the left page");
  }
  return "";
}


// Returns the function of userbool3 for type and subtype
int styles_logic_get_userbool3_function (int type, int subtype)
{
  if (type == StyleTypeIdentifier) {
    if (subtype == IdentifierSubtypeRunningHeader)
      return UserBool3RunningHeaderRight;
  }
  if (type == StyleTypeChapterNumber) {
    return UserBool3ChapterInRightRunningHeader;
  }
  return UserBool3None;
}


// Returns the text of userbool3's function.
string styles_logic_get_userbool3_text (int function)
{
  switch (function) {
    case UserBool3ChapterInRightRunningHeader : return gettext ("Print chapter number in the running header of the right page");
    case UserBool3RunningHeaderRight          : return gettext ("Print this in the running header of the right page");
  }
  return "";
}


// Returns the function of userint1 for type and subtype
int styles_logic_get_userint1_function (int type, int subtype)
{
  if (type == StyleTypeFootEndNote) {
    if (subtype == FootEndNoteSubtypeFootnote)
      return UserInt1NoteNumbering;
    if (subtype == FootEndNoteSubtypeEndnote)
      return UserInt1NoteNumbering;
  }
  if (type == StyleTypeCrossreference) {
    if (subtype == CrossreferenceSubtypeCrossreference)
      return UserInt1NoteNumbering;
  }
  if (type == StyleTypeTableElement) {
    if (subtype == TableElementSubtypeHeading)
      return UserInt1TableColumnNumber;
    if (subtype == TableElementSubtypeCell)
      return UserInt1TableColumnNumber;
  }
  return UserInt1None;
}


// Returns the value as human readable text for note numbering.
string styles_logic_note_numbering_text (int value)
{
  if (value == NoteNumbering123 ) return "1, 2, 3 ...";
  if (value == NoteNumberingAbc ) return "a, b, c ...";
  if (value == NoteNumberingUser) return gettext ("User defined sequence");
  return convert_to_string (value);
}


// Returns the function of userint2 for type and subtype
int styles_logic_get_userint2_function (int type, int subtype)
{
  if (type == StyleTypeFootEndNote) {
    if (subtype == FootEndNoteSubtypeFootnote)
      return UserInt2NoteNumberingRestart;
    if (subtype == FootEndNoteSubtypeEndnote)
      return UserInt2EndnotePosition;
  }
  if (type == StyleTypeCrossreference) {
    if (subtype == CrossreferenceSubtypeCrossreference)
      return UserInt2NoteNumberingRestart;
  }
  return UserInt2None;
}


// Returns the value as human readable text for when to restart the note numbering.
string styles_logic_note_restart_numbering_text (int value)
{
  if (value == NoteRestartNumberingNever       ) return gettext ("Never");
  if (value == NoteRestartNumberingEveryBook   ) return gettext ("Every book");
  if (value == NoteRestartNumberingEveryChapter) return gettext ("Every chapter");
  return convert_to_string (value);
}


// Returns the value as human readable text for the position of the endnotes.
string styles_logic_end_note_position_text (int value)
{
  if (value == EndNotePositionAfterBook) return gettext ("After each book");
  if (value == EndNotePositionVeryEnd  ) return gettext ("After everything else");
  if (value == EndNotePositionAtMarker ) return gettext ("Upon encountering a certain marker");
  return convert_to_string (value);
}


// Returns the function of userint3 for type and subtype
int styles_logic_get_userint3_function (int type, int subtype)
{
  if (type) {};
  if (subtype) {};
  return UserInt3None;
}


// Returns the function of userstring1 for type and subtype
int styles_logic_get_userstring1_function (int type, int subtype)
{
  if (type == StyleTypeFootEndNote) {
    if (subtype == FootEndNoteSubtypeFootnote)
      return UserString1NoteNumberingSequence;
    if (subtype == FootEndNoteSubtypeEndnote)
      return UserString1NoteNumberingSequence;
  }
  if (type == StyleTypeCrossreference) {
    if (subtype == CrossreferenceSubtypeCrossreference)
      return UserString1NoteNumberingSequence;
  }
  if (type == StyleTypeWordlistElement) {
    return UserString1WordListEntryAddition;
  }
  return UserString1None;
}


// Returns the function of userstring2 for type and subtype
int styles_logic_get_userstring2_function (int type, int subtype)
{
  if (type == StyleTypeFootEndNote) {
    if (subtype == FootEndNoteSubtypeEndnote)
      return UserString2DumpEndnotesHere;
  }
  return UserString2None;
}


// Returns the function of userstring3 for type and subtype
int styles_logic_get_userstring3_function (int type, int subtype)
{
  if (type) {};
  if (subtype) {};
  return UserString3None;
}


// It returns true if the combination of type and subtype start a new line in well-formed USFM. Otherwise it returns false
bool styles_logic_starts_new_line_in_usfm (int type, int subtype)
{
  switch (type) {
    case StyleTypeIdentifier :
    {
      return true;
      break;
    }
    case StyleTypeNotUsedComment :
    {
      return true;
      break;
    }
    case StyleTypeNotUsedRunningHeader :
    {
      return true;
      break;
    }
    case StyleTypeStartsParagraph :
    {
      return true;
      break;
    }
    case StyleTypeInlineText :
    {
      return false;
      break;
    }
    case StyleTypeChapterNumber :
    {
      return true;
      break;
    }
    case StyleTypeVerseNumber :
    {
      return true;
      break;
    }
    case StyleTypeFootEndNote :
    {
      return false;
      break;
    }
    case StyleTypeCrossreference :
    {
      return false;
      break;
    }
    case StyleTypePeripheral :
    {
      return true;
      break;
    }
    case StyleTypePicture :
    {
      return true;
      break;
    }
    case StyleTypePageBreak :
    {
      return true;
      break;
    }
    case StyleTypeTableElement :
    {
      if (subtype == TableElementSubtypeRow) return true;
      return false;
      break;
    }
    case StyleTypeWordlistElement :
    {
      return false;
      break;
    }
  }
  return true;
}

