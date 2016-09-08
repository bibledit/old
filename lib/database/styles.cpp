/*
Copyright (©) 2003-2016 Teus Benschop.

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


#include <database/styles.h>
#include <database/sqlite.h>
#include <filter/url.h>
#include <filter/string.h>
#include <locale/translate.h>
#include <styles/logic.h>


// This is the database for the styles.
// Resilience: It is hardly written to. 
// Chances of corruption are low.
// All default data is stored in the code in memory, not in a database on disk.


typedef struct
{
  const char * marker;
  const char * name;
  const char * info;
  const char * category;
  int type;
  int subtype;
  float fontsize;
  int italic;
  int bold;
  int underline;
  int smallcaps;
  int superscript;
  int justification;
  float spacebefore;
  float spaceafter;
  float leftmargin;
  float rightmargin;
  float firstlineindent;
  int spancolumns;
  const char * color;
  bool print;
  bool userbool1;
  bool userbool2;
  bool userbool3;
  int userint1;
  int userint2;
  int userint3;
  const char * userstring1;
  const char * userstring2;
  const char * userstring3;
  const char * backgroundcolor;
}
style_record;


style_record styles_table [] =
{
  { /* marker */ "add", /* name */ "* Translational Addition", /* info */ "For a translational addition to the text", /* category */ "st", /* type */ 4, /* subtype */ 0, /* fontsize */ 60 , /* italic */ 1, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "b", /* name */ "Blank Line", /* info */ "May be used to explicitely indicate additional white space between paragraphs. Poetry text stanza break (e.g. stanza break)", /* category */ "p", /* type */ 3, /* subtype */ 3, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "bd", /* name */ "* Bold Text", /* info */ "A character style, use bold text", /* category */ "cs", /* type */ 4, /* subtype */ 0, /* fontsize */ 12 , /* italic */ 0, /* bold */ 1, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "bdit", /* name */ "* BoldItalic Text", /* info */ "A character style, use bold + italic text", /* category */ "cs", /* type */ 4, /* subtype */ 0, /* fontsize */ 12 , /* italic */ 1, /* bold */ 1, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "bk", /* name */ "* Quoted book title", /* info */ "For the quoted name of a book", /* category */ "st", /* type */ 4, /* subtype */ 0, /* fontsize */ 12 , /* italic */ 1, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "c", /* name */ "Chapter Number", /* info */ "Chapter number", /* category */ "cv", /* type */ 5, /* subtype */ 0, /* fontsize */ 30 , /* italic */ 0, /* bold */ 1, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 1, /* userbool2 */ 1, /* userbool3 */ 1, /* userint1 */ 90, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "ca", /* name */ "Alternate Chapter Number", /* info */ "Second (alternate) chapter number (for coding dual versification; useful for places where different traditions of chapter breaks need to be supported in the same translation). Bibledit does nothing with this marker yet.", /* category */ "cv", /* type */ 0, /* subtype */ 2, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 20, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "cd", /* name */ "Chapter Description", /* info */ "Chapter Description (Publishing option D, e.g. in Russian Bibles)", /* category */ "cv", /* type */ 3, /* subtype */ 3, /* fontsize */ 11 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 8, /* spaceafter */ 4, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "cl", /* name */ "Chapter Publishing Label", /* info */ "Chapter label used for translations that add a word such as 'Chapter' before chapter numbers (e.g. Psalms).", /* category */ "cv", /* type */ 0, /* subtype */ 7, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "cls", /* name */ "Closure of an Epistle", /* info */ "Closure of an Epistle", /* category */ "p", /* type */ 3, /* subtype */ 3, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 2, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "conc", /* name */ "Concordance", /* info */ "Back matter concordance.", /* category */ "pm", /* type */ 9, /* subtype */ 3, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "cov", /* name */ "Cover", /* info */ "Other peripheral materials - cover.", /* category */ "pm", /* type */ 9, /* subtype */ 6, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "cp", /* name */ "Publishing Alternate Chapter Number", /* info */ "Published chapter number (this is a chapter marking that would be used in the published text).", /* category */ "cv", /* type */ 0, /* subtype */ 8, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "d", /* name */ "Descriptive Title, Hebrew Subtitle", /* info */ "A Hebrew text heading, to provide description (e.g. Psalms)", /* category */ "t", /* type */ 3, /* subtype */ 0, /* fontsize */ 12 , /* italic */ 1, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 1, /* spacebefore */ 4, /* spaceafter */ 4, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "dc", /* name */ "* Deuterocanonical/LXX Additions", /* info */ "Deuterocanonical/LXX additions or insertions in the Protocanonical text", /* category */ "st", /* type */ 4, /* subtype */ 0, /* fontsize */ 12 , /* italic */ 1, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "em", /* name */ "* Emphasized Text", /* info */ "A character style, use emphasized text style", /* category */ "cs", /* type */ 4, /* subtype */ 0, /* fontsize */ 12 , /* italic */ 1, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "f", /* name */ "* Footnote", /* info */ "A Footnote text item.", /* category */ "f", /* type */ 7, /* subtype */ 0, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 2, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "fdc", /* name */ "* DC text", /* info */ "Footnote text, applies to Apocrypha only.", /* category */ "f", /* type */ 7, /* subtype */ 4, /* fontsize */ 11 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 1, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "fe", /* name */ "* Endnote", /* info */ "An Endnote text item.", /* category */ "f", /* type */ 7, /* subtype */ 1, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "zendnotes", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "fig", /* name */ "* Figure/Illustration/Map", /* info */ "Illustration [Columns to span, height, filename, caption text]", /* category */ "sf", /* type */ 10, /* subtype */ 0, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "fk", /* name */ "Keyword", /* info */ "(*) A footnote keyword.", /* category */ "f", /* type */ 7, /* subtype */ 3, /* fontsize */ 11 , /* italic */ 1, /* bold */ 1, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "fl", /* name */ "Label Text", /* info */ "(*) A footnote label text item, for labelling the type or alternate translation being provided in the note.", /* category */ "f", /* type */ 7, /* subtype */ 3, /* fontsize */ 11 , /* italic */ 1, /* bold */ 1, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "fm", /* name */ "* Footnote Mark", /* info */ "An additional footnote marker location for a previous footnote.", /* category */ "f", /* type */ 4, /* subtype */ 0, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 1, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "fp", /* name */ "Paragraph Mark", /* info */ "(*) A Footnote additional paragraph marker.", /* category */ "f", /* type */ 7, /* subtype */ 5, /* fontsize */ 11 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 3, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 3, /* rightmargin */ 0, /* firstlineindent */ 3, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "fq", /* name */ "Quotation or Alternate Rendering", /* info */ "(*) A footnote scripture quote.", /* category */ "f", /* type */ 7, /* subtype */ 3, /* fontsize */ 11 , /* italic */ 1, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "fqa", /* name */ "Alternate Rendering", /* info */ "(*) A footnote alternate rendering for a portion of scripture text.", /* category */ "f", /* type */ 7, /* subtype */ 3, /* fontsize */ 11 , /* italic */ 1, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "fr", /* name */ "Reference", /* info */ "(*) The origin reference for the footnote.", /* category */ "f", /* type */ 7, /* subtype */ 3, /* fontsize */ 11 , /* italic */ 0, /* bold */ 1, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "ft", /* name */ "Text", /* info */ "(*) Footnote text.", /* category */ "f", /* type */ 7, /* subtype */ 2, /* fontsize */ 11 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "fv", /* name */ "Embedded Verse Number", /* info */ "(*) A verse number within the footnote text.", /* category */ "f", /* type */ 7, /* subtype */ 3, /* fontsize */ 11 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "glo", /* name */ "Glossary", /* info */ "Back matter glossary.", /* category */ "pm", /* type */ 9, /* subtype */ 3, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "h", /* name */ "Running Header", /* info */ "Running header text for a book", /* category */ "id", /* type */ 0, /* subtype */ 3, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 1, /* userbool3 */ 1, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "h1", /* name */ "Running Header", /* info */ "Running header text", /* category */ "id", /* type */ 0, /* subtype */ 3, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 1, /* userbool3 */ 1, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "h2", /* name */ "Left Running Header", /* info */ "Running header text, left side of page", /* category */ "id", /* type */ 0, /* subtype */ 3, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 1, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "h3", /* name */ "Right Running Header", /* info */ "Running header text, right side of page", /* category */ "id", /* type */ 0, /* subtype */ 3, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 1, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "ib", /* name */ "Blank Line", /* info */ "Introduction blank line", /* category */ "ipp", /* type */ 3, /* subtype */ 3, /* fontsize */ 5 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 1,  /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "id", /* name */ "Identification", /* info */ "File identification information (BOOKID, FILENAME, EDITOR, MODIFICATION DATE)", /* category */ "id", /* type */ 0, /* subtype */ 0, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 1, /* userbool2 */ 1, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "ide", /* name */ "Encoding", /* info */ "File encoding information. Bibledit disregards this marker, as all text in Bibledit are in UTF-8 encoding.", /* category */ "id", /* type */ 0, /* subtype */ 1, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "idx", /* name */ "Index", /* info */ "Back matter index.", /* category */ "pm", /* type */ 9, /* subtype */ 4, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "ie", /* name */ "End Marker", /* info */ "Introduction ending marker", /* category */ "ioe", /* type */ 0, /* subtype */ 0, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "iex", /* name */ "Explanatory or Bridge Text", /* info */ "Introduction explanatory or bridge text (e.g. explanation of missing book in Short Old Testament)", /* category */ "ipp", /* type */ 3, /* subtype */ 3, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 4, /* spaceafter */ 4, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 3.2, /* spancolumns */ 1,  /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "im", /* name */ "Paragraph, no first line indent", /* info */ "Introduction prose paragraph, with no first line indent (may occur after poetry)", /* category */ "ipp", /* type */ 3, /* subtype */ 3, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 1,  /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "imi", /* name */ "Indented Paragraph, no first line indent", /* info */ "Introduction prose paragraph text, indented, with no first line indent", /* category */ "ipp", /* type */ 3, /* subtype */ 3, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 6.4, /* rightmargin */ 6.4, /* firstlineindent */ 0, /* spancolumns */ 1,  /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "imq", /* name */ "Paragraph, quote from text, no first line indent", /* info */ "Introduction prose paragraph, quote from the body text, with no first line indent", /* category */ "ipp", /* type */ 3, /* subtype */ 3, /* fontsize */ 12 , /* italic */ 1, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 6.4, /* rightmargin */ 6.4, /* firstlineindent */ 0, /* spancolumns */ 1,  /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "imt", /* name */ "Major Title Level 1", /* info */ "Introduction major title, level 1 (if single level)", /* category */ "ith", /* type */ 3, /* subtype */ 0, /* fontsize */ 14 , /* italic */ 0, /* bold */ 1, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 1, /* spacebefore */ 8, /* spaceafter */ 4, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 1,  /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "imt1", /* name */ "Major Title Level 1", /* info */ "Introduction major title, level 1 (if multiple levels)", /* category */ "ith", /* type */ 3, /* subtype */ 0, /* fontsize */ 14 , /* italic */ 0, /* bold */ 1, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 1, /* spacebefore */ 8, /* spaceafter */ 4, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 1,  /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "imt2", /* name */ "Major Title Level 2", /* info */ "Introduction major title, level 2", /* category */ "ith", /* type */ 3, /* subtype */ 0, /* fontsize */ 13 , /* italic */ 1, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 1, /* spacebefore */ 6, /* spaceafter */ 3, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 1,  /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "imt3", /* name */ "Major Title Level 3", /* info */ "Introduction major title, level 3", /* category */ "ith", /* type */ 3, /* subtype */ 0, /* fontsize */ 12 , /* italic */ 0, /* bold */ 1, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 1, /* spacebefore */ 2, /* spaceafter */ 2, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 1,  /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "imt4", /* name */ "Major Title Level 4", /* info */ "Introduction major title, level 4 (usually within parenthesis)", /* category */ "ith", /* type */ 3, /* subtype */ 0, /* fontsize */ 12 , /* italic */ 1, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 1, /* spacebefore */ 2, /* spaceafter */ 2, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 1,  /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "imte", /* name */ "[Uncommon] Major Title at Introduction End", /* info */ "Introduction major title at introduction end", /* category */ "ith", /* type */ 3, /* subtype */ 0, /* fontsize */ 20 , /* italic */ 0, /* bold */ 1, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 1, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "intro", /* name */ "Introduction", /* info */ "Front matter introduction.", /* category */ "pm", /* type */ 9, /* subtype */ 3, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "io", /* name */ "Outline Level 1", /* info */ "Introduction outline text, level 1 (if single level)", /* category */ "ioe", /* type */ 3, /* subtype */ 1, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 12.7, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 1,  /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "io1", /* name */ "Outline Level 1", /* info */ "Introduction outline text, level 1 (if multiple levels)", /* category */ "ioe", /* type */ 3, /* subtype */ 1, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 12.7, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 1,  /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "io2", /* name */ "Outline Level 2", /* info */ "Introduction outline text, level 2", /* category */ "ioe", /* type */ 3, /* subtype */ 1, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 19.1, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 1,  /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "io3", /* name */ "Outline Level 3", /* info */ "Introduction outline text, level 3", /* category */ "ioe", /* type */ 3, /* subtype */ 1, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 25.4, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 1,  /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "io4", /* name */ "Outline Level 4", /* info */ "Introduction outline text, level 4", /* category */ "ioe", /* type */ 3, /* subtype */ 1, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 31.8, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 1,  /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "ior", /* name */ "Outline References Range", /* info */ "Introduction references range for outline entry; for marking references separately", /* category */ "ioe", /* type */ 4, /* subtype */ 0, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "iot", /* name */ "Outline Title", /* info */ "Introduction outline title", /* category */ "ioe", /* type */ 3, /* subtype */ 1, /* fontsize */ 12 , /* italic */ 0, /* bold */ 1, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 1, /* spacebefore */ 8, /* spaceafter */ 4, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 1,  /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "ip", /* name */ "Paragraph", /* info */ "Introduction prose paragraph", /* category */ "ipp", /* type */ 3, /* subtype */ 3, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 5, /* spancolumns */ 1,  /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "ipi", /* name */ "Indented Paragraph, first line indent", /* info */ "Introduction prose paragraph, indented, with first line indent", /* category */ "ipp", /* type */ 3, /* subtype */ 3, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 6.3, /* rightmargin */ 6.3, /* firstlineindent */ 3.2, /* spancolumns */ 1,  /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "ipq", /* name */ "Paragraph, quote from text", /* info */ "Introduction prose paragraph, quote from the body text", /* category */ "ipp", /* type */ 3, /* subtype */ 3, /* fontsize */ 12 , /* italic */ 1, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 6.4, /* rightmargin */ 6.4, /* firstlineindent */ 3.2, /* spancolumns */ 1,  /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "ipr", /* name */ "Paragraph, right aligned", /* info */ "Introduction prose paragraph, right aligned", /* category */ "ipp", /* type */ 3, /* subtype */ 3, /* fontsize */ 12 , /* italic */ 1, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 2, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 6.4, /* rightmargin */ 6.4, /* firstlineindent */ 0, /* spancolumns */ 1,  /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "iq", /* name */ "Poetry Level 1", /* info */ "Introduction poetry text, level 1 (if single level)", /* category */ "ipp", /* type */ 3, /* subtype */ 3, /* fontsize */ 12 , /* italic */ 1, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 25.4, /* rightmargin */ 0, /* firstlineindent */ -19.1, /* spancolumns */ 1,  /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "iq1", /* name */ "Poetry Level 1", /* info */ "Introduction poetry text, level 1 (if multiple levels)", /* category */ "ipp", /* type */ 3, /* subtype */ 3, /* fontsize */ 12 , /* italic */ 1, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 25.4, /* rightmargin */ 0, /* firstlineindent */ -19.1, /* spancolumns */ 1,  /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "iq2", /* name */ "Poetry Level 2", /* info */ "Introduction poetry text, level 2", /* category */ "ipp", /* type */ 3, /* subtype */ 3, /* fontsize */ 12 , /* italic */ 1, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 25.4, /* rightmargin */ 0, /* firstlineindent */ -12.7, /* spancolumns */ 1,  /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "iq3", /* name */ "Poetry Level 3", /* info */ "Introduction poetry text, level 3", /* category */ "ipp", /* type */ 3, /* subtype */ 3, /* fontsize */ 12 , /* italic */ 1, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 25.4, /* rightmargin */ 0, /* firstlineindent */ -6.4, /* spancolumns */ 1,  /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "is", /* name */ "Section Heading Level 1", /* info */ "Introduction section heading, level 1 (if single level)", /* category */ "ith", /* type */ 3, /* subtype */ 2, /* fontsize */ 12 , /* italic */ 0, /* bold */ 1, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 1, /* spacebefore */ 8, /* spaceafter */ 4, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 1,  /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "is1", /* name */ "Section Heading Level 1", /* info */ "Introduction section heading, level 1 (if multiple levels)", /* category */ "ith", /* type */ 3, /* subtype */ 2, /* fontsize */ 12 , /* italic */ 0, /* bold */ 1, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 1, /* spacebefore */ 8, /* spaceafter */ 4, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 1,  /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "is2", /* name */ "Section Heading Level 2", /* info */ "Introduction section heading, level 2", /* category */ "ith", /* type */ 3, /* subtype */ 2, /* fontsize */ 12 , /* italic */ 0, /* bold */ 1, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 1, /* spacebefore */ 8, /* spaceafter */ 4, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 1,  /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "it", /* name */ "* Italic Text", /* info */ "A character style, use italic text", /* category */ "cs", /* type */ 4, /* subtype */ 0, /* fontsize */ 12 , /* italic */ 1, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "k", /* name */ "* Keyword", /* info */ "For a keyword", /* category */ "st", /* type */ 4, /* subtype */ 0, /* fontsize */ 12 , /* italic */ 1, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "k1", /* name */ "Concordance Keyword Level 1", /* info */ "Concordance main entry text or keyword, level 1", /* category */ "pm", /* type */ 3, /* subtype */ 0, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "k2", /* name */ "Concordance Keyword Level 2", /* info */ "Concordance main entry text or keyword, level 2", /* category */ "pm", /* type */ 3, /* subtype */ 0, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 1,  /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "li", /* name */ "List Entry Level 1", /* info */ "A list entry, level 1 (if single level)", /* category */ "l", /* type */ 3, /* subtype */ 3, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 15.9, /* rightmargin */ -9.3, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "li1", /* name */ "List Entry Level 1", /* info */ "A list entry, level 1 (if multiple levels)", /* category */ "l", /* type */ 3, /* subtype */ 3, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 12.7, /* rightmargin */ 0, /* firstlineindent */ -6.4, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "li2", /* name */ "List Entry Level 2", /* info */ "A list entry, level 2", /* category */ "l", /* type */ 3, /* subtype */ 3, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 19.1, /* rightmargin */ 0, /* firstlineindent */ -6.4, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "li3", /* name */ "List Entry Level 3", /* info */ "A list entry, level 3", /* category */ "l", /* type */ 3, /* subtype */ 3, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 25.4, /* rightmargin */ 0, /* firstlineindent */ -6.4, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "li4", /* name */ "List Entry Level 4", /* info */ "A list entry, level 4", /* category */ "l", /* type */ 3, /* subtype */ 3, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 31.8, /* rightmargin */ 0, /* firstlineindent */ -6.4, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "lit", /* name */ "* Liturgical note", /* info */ "For a comment or note inserted for liturgical use", /* category */ "st", /* type */ 4, /* subtype */ 0, /* fontsize */ 12 , /* italic */ 0, /* bold */ 1, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "m", /* name */ "Margin, No First Line Indent", /* info */ "Paragraph text, with no first line indent (may occur after poetry)", /* category */ "p", /* type */ 3, /* subtype */ 3, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "maps", /* name */ "Map Index", /* info */ "Back matter map index.", /* category */ "pm", /* type */ 9, /* subtype */ 5, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "mi", /* name */ "Indented, No First Line Indent", /* info */ "Paragraph text, indented, with no first line indent; often used for discourse", /* category */ "p", /* type */ 3, /* subtype */ 3, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 6.4, /* rightmargin */ 6.4, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "mr", /* name */ "Major Section Range References", /* info */ "A major section division references range heading", /* category */ "h", /* type */ 3, /* subtype */ 0, /* fontsize */ 12 , /* italic */ 1, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 1, /* spacebefore */ 0, /* spaceafter */ 4, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 1,  /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "ms", /* name */ "Major Section Level 1", /* info */ "A major section division heading, level 1 (if single level)", /* category */ "h", /* type */ 3, /* subtype */ 0, /* fontsize */ 14 , /* italic */ 0, /* bold */ 1, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 1, /* spacebefore */ 16, /* spaceafter */ 4, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 1,  /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "ms1", /* name */ "Major Section Level 1", /* info */ "A major section division heading, level 1 (if multiple levels)", /* category */ "h", /* type */ 3, /* subtype */ 0, /* fontsize */ 14 , /* italic */ 0, /* bold */ 1, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 1, /* spacebefore */ 16, /* spaceafter */ 4, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 1,  /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "ms2", /* name */ "Major Section Level 2", /* info */ "A major section division heading, level 2", /* category */ "h", /* type */ 3, /* subtype */ 0, /* fontsize */ 14 , /* italic */ 0, /* bold */ 1, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 1, /* spacebefore */ 16, /* spaceafter */ 4, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 1,  /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "mt", /* name */ "Major Title Level 1", /* info */ "The main title of the book (if single level)", /* category */ "t", /* type */ 3, /* subtype */ 0, /* fontsize */ 20 , /* italic */ 0, /* bold */ 1, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 1, /* spacebefore */ 8, /* spaceafter */ 4, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 1,  /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "mt1", /* name */ "Major Title Level 1", /* info */ "The main title of the book (if multiple levels)", /* category */ "t", /* type */ 3, /* subtype */ 0, /* fontsize */ 20 , /* italic */ 0, /* bold */ 1, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 1, /* spacebefore */ 2, /* spaceafter */ 4, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 1,  /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "mt2", /* name */ "Major Title Level 2", /* info */ "A secondary title with less important information than the main title.", /* category */ "t", /* type */ 3, /* subtype */ 0, /* fontsize */ 16 , /* italic */ 1, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 1, /* spacebefore */ 0, /* spaceafter */ 2, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 1,  /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "mt3", /* name */ "Major Title Level 3", /* info */ "A secondary title, less important than the main title", /* category */ "t", /* type */ 3, /* subtype */ 0, /* fontsize */ 16 , /* italic */ 0, /* bold */ 1, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 1, /* spacebefore */ 2, /* spaceafter */ 2, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 1,  /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "mt4", /* name */ "Major Title level 4", /* info */ "A small secondary title sometimes occuring within parentheses", /* category */ "t", /* type */ 3, /* subtype */ 0, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 1, /* spacebefore */ 2, /* spaceafter */ 2, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 1,  /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "mte", /* name */ "[Uncommon] Major Title Ending Level 1", /* info */ "The main title of the book repeated at the end of the book (if single level)", /* category */ "t", /* type */ 3, /* subtype */ 0, /* fontsize */ 20 , /* italic */ 0, /* bold */ 1, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 1, /* spacebefore */ 8, /* spaceafter */ 4, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 1,  /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "mte1", /* name */ "[Uncommon] Major Title Ending Level 1", /* info */ "The main title of the book repeated at the end of the book (if multiple levels)", /* category */ "t", /* type */ 3, /* subtype */ 0, /* fontsize */ 20 , /* italic */ 0, /* bold */ 1, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 1, /* spacebefore */ 8, /* spaceafter */ 4, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 1,  /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "mte2", /* name */ "[Uncommon] Major Title Ending Level 2", /* info */ "A secondary title occurring before or after the 'ending' main title", /* category */ "t", /* type */ 3, /* subtype */ 0, /* fontsize */ 16 , /* italic */ 1, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 1, /* spacebefore */ 0, /* spaceafter */ 2, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 1,  /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "nb", /* name */ "No Break with Previous Paragraph", /* info */ "Paragraph text, with no break from previous paragraph text (at chapter boundary)", /* category */ "p", /* type */ 3, /* subtype */ 3, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 3, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "nd", /* name */ "* Name of Deity", /* info */ "For name of diety", /* category */ "st", /* type */ 4, /* subtype */ 0, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 1, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "ndx", /* name */ "* Subject Index Entry", /* info */ "A subject index text item.", /* category */ "sf", /* type */ 13, /* subtype */ 3, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 1, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "no", /* name */ "* Normal Text", /* info */ "A character style, use normal text", /* category */ "cs", /* type */ 4, /* subtype */ 0, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "ord", /* name */ "* Ordinal number text portion", /* info */ "For the text portion of an ordinal number", /* category */ "st", /* type */ 4, /* subtype */ 0, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 1, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "p", /* name */ "Normal, First Line Indent", /* info */ "Paragraph text, with first line indent", /* category */ "p", /* type */ 3, /* subtype */ 3, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 3, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 3.2, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "p1", /* name */ "Front/Back Matter Paragraph Level 1", /* info */ "Front or back matter text paragraph, level 1 (if multiple levels)", /* category */ "pm", /* type */ 3, /* subtype */ 0, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 3.2, /* spancolumns */ 1,  /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "p2", /* name */ "Front/Back Matter Paragraph Level 2", /* info */ "Front or back matter text paragraph, level 2 (if multiple levels)", /* category */ "pm", /* type */ 3, /* subtype */ 0, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 3.2, /* rightmargin */ 0, /* firstlineindent */ 3.2, /* spancolumns */ 1,  /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "pb", /* name */ "Page Break", /* info */ "Page Break used for new reader portions and children's bibles where content is controlled by the page", /* category */ "sb", /* type */ 11, /* subtype */ 0, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "pc", /* name */ "Centered (for Inscription)", /* info */ "Paragraph text, centered (for Inscription)", /* category */ "p", /* type */ 3, /* subtype */ 3, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 1, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "pi", /* name */ "Indented, Level 1, First Line Indent", /* info */ "Paragraph text, level 1 indent (if single level), with first line indent; often used for discourse", /* category */ "p", /* type */ 3, /* subtype */ 3, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 6.4, /* rightmargin */ 6.4, /* firstlineindent */ 3.2, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "pi1", /* name */ "Indented, Level 1, First Line Indent", /* info */ "Paragraph text, level 1 indent (if multiple levels), with first line indent; often used for discourse", /* category */ "p", /* type */ 3, /* subtype */ 3, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 6.4, /* rightmargin */ 6.4, /* firstlineindent */ 3.2, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "pi2", /* name */ "Indented, Level 2, First Line Indent", /* info */ "Paragraph text, level 2 indent, with first line indent; often used for discourse", /* category */ "p", /* type */ 3, /* subtype */ 3, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 12.7, /* rightmargin */ 6.4, /* firstlineindent */ 3.2, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "pi3", /* name */ "Indented, Level 3, First Line Indent", /* info */ "Paragraph text, level 3 indent, with first line indent; often used for discourse", /* category */ "p", /* type */ 3, /* subtype */ 3, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 19.1, /* rightmargin */ 6.4, /* firstlineindent */ 3.2, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "pm", /* name */ "Embedded Text", /* info */ "Embedded text paragraph", /* category */ "p", /* type */ 3, /* subtype */ 3, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 6.4, /* rightmargin */ 6.4, /* firstlineindent */ 3.2, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "pmc", /* name */ "Embedded Text Closing", /* info */ "Embedded text closing", /* category */ "p", /* type */ 3, /* subtype */ 3, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 6.4, /* rightmargin */ 6.4, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "pmo", /* name */ "Embedded Text Opening", /* info */ "Embedded text opening", /* category */ "p", /* type */ 3, /* subtype */ 3, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 3.2, /* rightmargin */ 3.2, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "pmr", /* name */ "Embedded Text Refrain", /* info */ "Embedded text refrain (e.g. Then all the people shall say, Amen!)", /* category */ "p", /* type */ 3, /* subtype */ 3, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 2, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 6.4, /* rightmargin */ 6.4, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "pn", /* name */ "* Proper Name", /* info */ "For a proper name", /* category */ "st", /* type */ 4, /* subtype */ 0, /* fontsize */ 12 , /* italic */ 0, /* bold */ 1, /* underline */ 1, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "pref", /* name */ "Preface", /* info */ "Front matter preface.", /* category */ "pm", /* type */ 9, /* subtype */ 2, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "pro", /* name */ "* CJK Prounciation", /* info */ "For indicating pronunciation in CJK texts", /* category */ "sf", /* type */ 0, /* subtype */ 9, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "pub", /* name */ "Publication Data", /* info */ "Front matter publication data.", /* category */ "pm", /* type */ 9, /* subtype */ 0, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "pubinfo", /* name */ "Publication Information", /* info */ "Publication information - Lang,Credit,Version,Copies,Publisher,Id,Logo. Not yet supported in Bibledit.", /* category */ "pm", /* type */ 0, /* subtype */ 2, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "q", /* name */ "Indent Level 1, Single Level Only", /* info */ "Poetry text, level 1 indent (if single level)", /* category */ "pe", /* type */ 3, /* subtype */ 3, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 25.4, /* rightmargin */ 0, /* firstlineindent */ -19.1, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "q1", /* name */ "Indent Level 1", /* info */ "Poetry text, level 1 indent (if multiple levels)", /* category */ "pe", /* type */ 3, /* subtype */ 3, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 24.4, /* rightmargin */ 0, /* firstlineindent */ -19.1, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "q2", /* name */ "Indent Level 2", /* info */ "Poetry text, level 2 indent", /* category */ "pe", /* type */ 3, /* subtype */ 3, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 25.4, /* rightmargin */ 0, /* firstlineindent */ -12.7, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "q3", /* name */ "Indent Level 3", /* info */ "Poetry text, level 3 indent", /* category */ "pe", /* type */ 3, /* subtype */ 3, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 25.4, /* rightmargin */ 0, /* firstlineindent */ -6.4, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "qa", /* name */ "Acrostic Heading/Marker", /* info */ "Poetry text, Acrostic marker/heading", /* category */ "pe", /* type */ 3, /* subtype */ 3, /* fontsize */ 12 , /* italic */ 1, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "qac", /* name */ "* Acrostic Letter", /* info */ "Poetry text, Acrostic markup of the first character of a line of acrostic poetry", /* category */ "pe", /* type */ 4, /* subtype */ 0, /* fontsize */ 12 , /* italic */ 1, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "qc", /* name */ "Centered", /* info */ "Poetry text, centered", /* category */ "pe", /* type */ 3, /* subtype */ 3, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 1, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "qm", /* name */ "Embedded Text, Indent Level 1, Single Level Only", /* info */ "Poetry text, embedded, level 1 indent (if single level)", /* category */ "pe", /* type */ 3, /* subtype */ 3, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 25.4, /* rightmargin */ 0, /* firstlineindent */ -19.1, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "qm1", /* name */ "Embedded Text, Indent Level 1", /* info */ "Poetry text, embedded, level 1 indent (if multiple levels)", /* category */ "pe", /* type */ 3, /* subtype */ 3, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 25.4, /* rightmargin */ 0, /* firstlineindent */ -19.1, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "qm2", /* name */ "Embedded Text, Indent Level 2", /* info */ "Poetry text, embedded, level 2 indent", /* category */ "pe", /* type */ 3, /* subtype */ 3, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 25.4, /* rightmargin */ 0, /* firstlineindent */ -12.7, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "qm3", /* name */ "Embedded Text, Indent Level 3", /* info */ "Poetry text, embedded, level 3 indent", /* category */ "pe", /* type */ 3, /* subtype */ 3, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 25.4, /* rightmargin */ 0, /* firstlineindent */ -6.4, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "qr", /* name */ "Right Aligned", /* info */ "Poetry text, Right Aligned", /* category */ "pe", /* type */ 3, /* subtype */ 3, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 2, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "qs", /* name */ "* Selah", /* info */ "Poetry text, Selah", /* category */ "pe", /* type */ 4, /* subtype */ 0, /* fontsize */ 12 , /* italic */ 1, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "qt", /* name */ "* Quoted Text, OT in NT", /* info */ "For Old Testament quoted text appearing in the New Testament", /* category */ "st", /* type */ 4, /* subtype */ 0, /* fontsize */ 12 , /* italic */ 1, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "r", /* name */ "Parallel References", /* info */ "Parallel reference(s)", /* category */ "h", /* type */ 3, /* subtype */ 2, /* fontsize */ 12 , /* italic */ 1, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 1, /* spacebefore */ 0, /* spaceafter */ 4, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "rem", /* name */ "Remark", /* info */ "Comments and remarks", /* category */ "id", /* type */ 0, /* subtype */ 2, /* fontsize */ 60 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "rq", /* name */ "Inline Quotation References", /* info */ "A cross-reference indicating the source text for the preceding quotation.", /* category */ "h", /* type */ 3, /* subtype */ 3, /* fontsize */ 12 , /* italic */ 1, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 2, /* spacebefore */ 0, /* spaceafter */ 4, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "s", /* name */ "Section Heading Level 1", /* info */ "A section heading, level 1 (if single level)", /* category */ "h", /* type */ 3, /* subtype */ 2, /* fontsize */ 12 , /* italic */ 0, /* bold */ 1, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 1, /* spacebefore */ 8, /* spaceafter */ 4, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "s1", /* name */ "Section Heading Level 1", /* info */ "A section heading, level 1 (if multiple levels)", /* category */ "h", /* type */ 3, /* subtype */ 2, /* fontsize */ 12 , /* italic */ 0, /* bold */ 1, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 1, /* spacebefore */ 8, /* spaceafter */ 4, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "s2", /* name */ "Section Heading Level 2", /* info */ "A section heading, level 2 (e.g. Proverbs 22-24)", /* category */ "h", /* type */ 3, /* subtype */ 2, /* fontsize */ 12 , /* italic */ 1, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 1, /* spacebefore */ 8, /* spaceafter */ 4, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "s3", /* name */ "Section Heading Level 3", /* info */ "A section heading, level 3 (e.g. Genesis: The First Day)", /* category */ "h", /* type */ 3, /* subtype */ 2, /* fontsize */ 12 , /* italic */ 1, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 6, /* spaceafter */ 3, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "s4", /* name */ "Section Heading Level 4", /* info */ "A section heading, level 4", /* category */ "h", /* type */ 3, /* subtype */ 2, /* fontsize */ 12 , /* italic */ 1, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 6, /* spaceafter */ 3, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "sc", /* name */ "* Small Caps", /* info */ "A character style, for small capitalization text", /* category */ "cs", /* type */ 4, /* subtype */ 0, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 1, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "sig", /* name */ "* Author's Signature (Epistles)", /* info */ "For the signature of the author of an Epistle", /* category */ "st", /* type */ 4, /* subtype */ 0, /* fontsize */ 12 , /* italic */ 1, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "sls", /* name */ "* Secondary Language or Text Source", /* info */ "To represent where the original text is in a secondary language or from an alternate text source", /* category */ "st", /* type */ 4, /* subtype */ 0, /* fontsize */ 12 , /* italic */ 1, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "sp", /* name */ "Speaker", /* info */ "A heading, to identify the speaker (e.g. Job)", /* category */ "h", /* type */ 3, /* subtype */ 2, /* fontsize */ 12 , /* italic */ 1, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 8, /* spaceafter */ 4, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "spine", /* name */ "Spine", /* info */ "Other peripheral materials - spine.", /* category */ "pm", /* type */ 9, /* subtype */ 9, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "sr", /* name */ "Section Heading Range References", /* info */ "A section division references range heading", /* category */ "h", /* type */ 3, /* subtype */ 2, /* fontsize */ 12 , /* italic */ 0, /* bold */ 1, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 1, /* spacebefore */ 0, /* spaceafter */ 4, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "tc1", /* name */ "Column 1 Cell", /* info */ "A table cell item, column 1.", /* category */ "te", /* type */ 12, /* subtype */ 2, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 1, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "tc2", /* name */ "Column 2 Cell", /* info */ "A table cell item, column 2.", /* category */ "te", /* type */ 12, /* subtype */ 2, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 2, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "tc3", /* name */ "Column 3 Cell", /* info */ "A table cell item, column 3.", /* category */ "te", /* type */ 12, /* subtype */ 2, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 3, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "tc4", /* name */ "Column 4 Cell", /* info */ "A table cell item, column 4.", /* category */ "te", /* type */ 12, /* subtype */ 2, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 4, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "tcr1", /* name */ "Column 1 Cell, Right Aligned", /* info */ "A table cell item, column 1, right aligned.", /* category */ "te", /* type */ 12, /* subtype */ 2, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 2, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 1, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "tcr2", /* name */ "Column 2 Cell, Right Aligned", /* info */ "A table cell item, column 2, right aligned.", /* category */ "te", /* type */ 12, /* subtype */ 2, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 2, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 2, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "tcr3", /* name */ "Column 3 Cell, Right Aligned", /* info */ "A table cell item, column 3, right aligned.", /* category */ "te", /* type */ 12, /* subtype */ 2, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 2, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 3, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "tcr4", /* name */ "Column 4 Cell, Right Aligned", /* info */ "A table cell item, column 4, right aligned.", /* category */ "te", /* type */ 12, /* subtype */ 2, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 2, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 4, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "th1", /* name */ "Column 1 Heading", /* info */ "A table heading, column 1.", /* category */ "te", /* type */ 12, /* subtype */ 1, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 1, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "th2", /* name */ "Column 2 Heading", /* info */ "A table heading, column 2.", /* category */ "te", /* type */ 12, /* subtype */ 1, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 2, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "th3", /* name */ "Column 3 Heading", /* info */ "A table heading, column 3.", /* category */ "te", /* type */ 12, /* subtype */ 1, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 3, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "th4", /* name */ "Column 4 Heading", /* info */ "A table heading, column 4.", /* category */ "te", /* type */ 12, /* subtype */ 1, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 4, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "thr1", /* name */ "Column 1 Heading, Right Aligned", /* info */ "A table heading, column 1, right aligned.", /* category */ "te", /* type */ 12, /* subtype */ 1, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 2, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 1, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "thr2", /* name */ "Column 2 Heading, Right Aligned", /* info */ "A table heading, column 2, right aligned.", /* category */ "te", /* type */ 12, /* subtype */ 1, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 2, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 2, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "thr3", /* name */ "Column 3 Heading, Right Aligned", /* info */ "A table heading, column 3, right aligned.", /* category */ "te", /* type */ 12, /* subtype */ 1, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 2, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 3, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "thr4", /* name */ "Column 4 Heading, Right Aligned", /* info */ "A table heading, column 4, right aligned.", /* category */ "te", /* type */ 12, /* subtype */ 1, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 2, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 4, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "tl", /* name */ "* Transliterated Word", /* info */ "For transliterated words", /* category */ "st", /* type */ 4, /* subtype */ 0, /* fontsize */ 12 , /* italic */ 1, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "toc", /* name */ "Table of Contents", /* info */ "Front matter table of contents.", /* category */ "pm", /* type */ 9, /* subtype */ 1, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "toc1", /* name */ "Long Table of Contents Text", /* info */ "Long table of contents text", /* category */ "id", /* type */ 0, /* subtype */ 4, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "toc2", /* name */ "Short Table of Contents Text", /* info */ "Short table of contents text", /* category */ "id", /* type */ 0, /* subtype */ 5, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "toc3", /* name */ "Book Abbreviation", /* info */ "Book Abbreviation (not yet supported by Bibledit).", /* category */ "id", /* type */ 0, /* subtype */ 6, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "tr", /* name */ "Row", /* info */ "A new table row.", /* category */ "te", /* type */ 12, /* subtype */ 0, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 12.7, /* rightmargin */ 0, /* firstlineindent */ -3.2, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "v", /* name */ "Verse Number", /* info */ "A verse number (Necessary for normal Bibledit operation)", /* category */ "cv", /* type */ 6, /* subtype */ 0, /* fontsize */ 60 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 1, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "va", /* name */ "* Alternate verse number", /* info */ "Second (alternate) verse number (for coding dual numeration in Psalms; see also NRSV Exo 22.1-4)", /* category */ "cv", /* type */ 4, /* subtype */ 0, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "vp", /* name */ "* Publishing alternate verse", /* info */ "Published verse marker - this is a verse marking that would be used in the published text.", /* category */ "cv", /* type */ 4, /* subtype */ 0, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "w", /* name */ "* Wordlist Entry", /* info */ "A wordlist text item.", /* category */ "sf", /* type */ 13, /* subtype */ 0, /* fontsize */ 12 , /* italic */ 2, /* bold */ 2, /* underline */ 2, /* smallcaps */ 2, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "wg", /* name */ "* Greek Wordlist Entry", /* info */ "A Greek Wordlist text item.", /* category */ "sf", /* type */ 13, /* subtype */ 2, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 1, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "wh", /* name */ "* Hebrew Wordlist Entry", /* info */ "A Hebrew wordlist text item.", /* category */ "sf", /* type */ 13, /* subtype */ 1, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 1, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "wj", /* name */ "* Words of Jesus", /* info */ "For marking the words of Jesus", /* category */ "st", /* type */ 4, /* subtype */ 0, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#FF0000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "x", /* name */ "* Cross Reference", /* info */ "A list of cross references.", /* category */ "x", /* type */ 8, /* subtype */ 0, /* fontsize */ 12 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 1, /* userint2 */ 2, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "xdc", /* name */ "* DC Target Refs", /* info */ "Cross-reference target reference(s), Apocrypha only.", /* category */ "x", /* type */ 8, /* subtype */ 3, /* fontsize */ 11 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 1, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "xk", /* name */ "Keyword", /* info */ "(*) A cross reference keyword.", /* category */ "x", /* type */ 8, /* subtype */ 2, /* fontsize */ 11 , /* italic */ 1, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "xo", /* name */ "Origin Reference", /* info */ "(*) The cross reference origin reference.", /* category */ "x", /* type */ 8, /* subtype */ 2, /* fontsize */ 11 , /* italic */ 0, /* bold */ 1, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "xq", /* name */ "Quotation", /* info */ "(*) A cross-reference quotation from the scripture text.", /* category */ "x", /* type */ 8, /* subtype */ 2, /* fontsize */ 11 , /* italic */ 1, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" },
  { /* marker */ "xt", /* name */ "Target References", /* info */ "(*) The cross reference target reference(s).", /* category */ "x", /* type */ 8, /* subtype */ 1, /* fontsize */ 11 , /* italic */ 0, /* bold */ 0, /* underline */ 0, /* smallcaps */ 0, /* superscript */ 0, /* justification */ 0, /* spacebefore */ 0, /* spaceafter */ 0, /* leftmargin */ 0, /* rightmargin */ 0, /* firstlineindent */ 0, /* spancolumns */ 0, /* color */ "#000000",  /* print */ 1, /* userbool1 */ 0, /* userbool2 */ 0, /* userbool3 */ 0, /* userint1 */ 0, /* userint2 */ 0, /* userint3 */ 0, /* userstring1 */ "", /* userstring2 */ "", /* userstring3 */ "", /* backgroundcolor */ "#FFFFFF" }
};


sqlite3 * Database_Styles::connect ()
{
  return database_sqlite_connect ("styles");
}


void Database_Styles::create ()
{
  sqlite3 * db = connect ();
  string sql;
  sql = "CREATE TABLE IF NOT EXISTS users ("
        "user text,"
        "sheet text"
        ");";
  database_sqlite_exec (db, sql);
  database_sqlite_disconnect (db);
}


// Creates a stylesheet.
void Database_Styles::createSheet (string sheet)
{
  filter_url_mkdir (sheetfolder (sheet));
  for (unsigned int i = 0; i < data_count(); i++) {
    Database_Styles_Item item = read_item ("", styles_table[i].marker);
    write_item (sheet, item);
  }
}


// Returns an array with the available stylesheets.
vector <string> Database_Styles::getSheets ()
{
  vector <string> sheets = filter_url_scandir (databasefolder ());
  if (find (sheets.begin (), sheets.end (), styles_logic_standard_sheet ()) == sheets.end ()) {
    sheets.push_back (styles_logic_standard_sheet ());
  }
  sort (sheets.begin(), sheets.end());
  return sheets;
}


// Deletes a stylesheet.
void Database_Styles::deleteSheet (string sheet)
{
  if (!sheet.empty ()) filter_url_rmdir (sheetfolder (sheet));
}


// Adds a marker to the stylesheet.
void Database_Styles::addMarker (string sheet, string marker)
{
  Database_Styles_Item item = read_item (sheet, marker);
  write_item (sheet, item);
}


// Deletes a marker from a stylesheet.
void Database_Styles::deleteMarker (string sheet, string marker)
{
  filter_url_unlink (stylefile (sheet, marker));
}


// Returns a map with all the markers and the names of the styles in the stylesheet.
map <string, string> Database_Styles::getMarkersAndNames (string sheet)
{
  map <string, string> markers_names;
  vector <string> markers = getMarkers (sheet);
  for (auto marker : markers) {
    Database_Styles_Item item = read_item (sheet, marker);
    markers_names [marker] = item.name;
  }
  return markers_names;
}


// Returns an array with all the markers of the styles in the stylesheet.
vector <string> Database_Styles::getMarkers (string sheet)
{
  vector <string> markers = filter_url_scandir (sheetfolder (sheet));
  if (markers.empty ()) {
    for (unsigned int i = 0; i < data_count(); i++) {
      markers.push_back (styles_table[i].marker);
    }
  }
  return markers;
}


// Returns an object with all data belonging to a marker.
Database_Styles_Item Database_Styles::getMarkerData (string sheet, string marker)
{
  return read_item (sheet, marker);
}


// Updates a style's name.
void Database_Styles::updateName (string sheet, string marker, string name)
{
  Database_Styles_Item item = read_item (sheet, marker);
  item.name = name;
  write_item (sheet, item);
}


// Updates a style's info.
void Database_Styles::updateInfo (string sheet, string marker, string info)
{
  Database_Styles_Item item = read_item (sheet, marker);
  item.info = info;
  write_item (sheet, item);
}


// Updates a style's category.
void Database_Styles::updateCategory (string sheet, string marker, string category)
{
  Database_Styles_Item item = read_item (sheet, marker);
  item.category = category;
  write_item (sheet, item);
}


// Updates a style's type.
void Database_Styles::updateType (string sheet, string marker, int type)
{
  Database_Styles_Item item = read_item (sheet, marker);
  item.type = type;
  write_item (sheet, item);
}


// Updates a style's subtype.
void Database_Styles::updateSubType (string sheet, string marker, int subtype)
{
  Database_Styles_Item item = read_item (sheet, marker);
  item.subtype = subtype;
  write_item (sheet, item);
}


// Updates a style's font size.
void Database_Styles::updateFontsize (string sheet, string marker, float fontsize)
{
  Database_Styles_Item item = read_item (sheet, marker);
  item.fontsize = fontsize;
  write_item (sheet, item);
}


// Updates a style's italic setting.
void Database_Styles::updateItalic (string sheet, string marker, int italic)
{
  Database_Styles_Item item = read_item (sheet, marker);
  item.italic = italic;
  write_item (sheet, item);
}


// Updates a style's bold setting.
void Database_Styles::updateBold (string sheet, string marker, int bold)
{
  Database_Styles_Item item = read_item (sheet, marker);
  item.bold = bold;
  write_item (sheet, item);
}


// Updates a style's underline setting.
void Database_Styles::updateUnderline (string sheet, string marker, int underline)
{
  Database_Styles_Item item = read_item (sheet, marker);
  item.underline = underline;
  write_item (sheet, item);
}


// Updates a style's small caps setting.
void Database_Styles::updateSmallcaps (string sheet, string marker, int smallcaps)
{
  Database_Styles_Item item = read_item (sheet, marker);
  item.smallcaps = smallcaps;
  write_item (sheet, item);
}


void Database_Styles::updateSuperscript (string sheet, string marker, int superscript)
{
  Database_Styles_Item item = read_item (sheet, marker);
  item.superscript = superscript;
  write_item (sheet, item);
}


void Database_Styles::updateJustification (string sheet, string marker, int justification)
{
  Database_Styles_Item item = read_item (sheet, marker);
  item.justification = justification;
  write_item (sheet, item);
}


void Database_Styles::updateSpaceBefore (string sheet, string marker, float spacebefore)
{
  Database_Styles_Item item = read_item (sheet, marker);
  item.spacebefore = spacebefore;
  write_item (sheet, item);
}


void Database_Styles::updateSpaceAfter (string sheet, string marker, float spaceafter)
{
  Database_Styles_Item item = read_item (sheet, marker);
  item.spaceafter = spaceafter;
  write_item (sheet, item);
}


void Database_Styles::updateLeftMargin (string sheet, string marker, float leftmargin)
{
  Database_Styles_Item item = read_item (sheet, marker);
  item.leftmargin = leftmargin;
  write_item (sheet, item);
}


void Database_Styles::updateRightMargin (string sheet, string marker, float rightmargin)
{
  Database_Styles_Item item = read_item (sheet, marker);
  item.rightmargin = rightmargin;
  write_item (sheet, item);
}


void Database_Styles::updateFirstLineIndent (string sheet, string marker, float firstlineindent)
{
  Database_Styles_Item item = read_item (sheet, marker);
  item.firstlineindent = firstlineindent;
  write_item (sheet, item);
}


void Database_Styles::updateSpanColumns (string sheet, string marker, bool spancolumns)
{
  Database_Styles_Item item = read_item (sheet, marker);
  item.spancolumns = spancolumns;
  write_item (sheet, item);
}


void Database_Styles::updateColor (string sheet, string marker, string color)
{
  Database_Styles_Item item = read_item (sheet, marker);
  item.color = color;
  write_item (sheet, item);
}


void Database_Styles::updatePrint (string sheet, string marker, bool print)
{
  Database_Styles_Item item = read_item (sheet, marker);
  item.print = print;
  write_item (sheet, item);
}


void Database_Styles::updateUserbool1 (string sheet, string marker, bool userbool1)
{
  Database_Styles_Item item = read_item (sheet, marker);
  item.userbool1 = userbool1;
  write_item (sheet, item);
}


void Database_Styles::updateUserbool2 (string sheet, string marker, bool userbool2)
{
  Database_Styles_Item item = read_item (sheet, marker);
  item.userbool2 = userbool2;
  write_item (sheet, item);
}


void Database_Styles::updateUserbool3 (string sheet, string marker, bool userbool3)
{
  Database_Styles_Item item = read_item (sheet, marker);
  item.userbool3 = userbool3;
  write_item (sheet, item);
}


void Database_Styles::updateUserint1 (string sheet, string marker, int userint1)
{
  Database_Styles_Item item = read_item (sheet, marker);
  item.userint1 = userint1;
  write_item (sheet, item);
}


void Database_Styles::updateUserint2 (string sheet, string marker, int userint2)
{
  Database_Styles_Item item = read_item (sheet, marker);
  item.userint2 = userint2;
  write_item (sheet, item);
}


void Database_Styles::updateUserstring1 (string sheet, string marker, string userstring1)
{
  Database_Styles_Item item = read_item (sheet, marker);
  item.userstring1 = userstring1;
  write_item (sheet, item);
}


void Database_Styles::updateUserstring2 (string sheet, string marker, string userstring2)
{
  Database_Styles_Item item = read_item (sheet, marker);
  item.userstring2 = userstring2;
  write_item (sheet, item);
}


void Database_Styles::updateUserstring3 (string sheet, string marker, string userstring3)
{
  Database_Styles_Item item = read_item (sheet, marker);
  item.userstring3 = userstring3;
  write_item (sheet, item);
}


void Database_Styles::updateBackgroundColor (string sheet, string marker, string color)
{
  Database_Styles_Item item = read_item (sheet, marker);
  item.backgroundcolor = color;
  write_item (sheet, item);
}


// Grant $user write access to stylesheet $sheet.
void Database_Styles::grantWriteAccess (string user, string sheet)
{
  SqliteSQL sql;
  sql.add ("INSERT INTO users VALUES (");
  sql.add (user);
  sql.add (",");
  sql.add (sheet);
  sql.add (");");
  sqlite3 * db = connect ();
  database_sqlite_exec (db, sql.sql);
  database_sqlite_disconnect (db);
}


// Revoke a $user's write access to stylesheet $sheet.
// If the $user is empty, then revoke write access of anybody to that $sheet.
void Database_Styles::revokeWriteAccess (string user, string sheet)
{
  SqliteSQL sql;
  sql.add ("DELETE FROM users WHERE");
  if (!user.empty ()) {
    sql.add ("user =");
    sql.add (user);
    sql.add ("AND");
  }
  sql.add ("sheet =");
  sql.add (sheet);
  sql.add (";");
  sqlite3 * db = connect ();
  database_sqlite_exec (db, sql.sql);
  database_sqlite_disconnect (db);
}


// Returns true or false depending on whether $user has write access to $sheet.
bool Database_Styles::hasWriteAccess (string user, string sheet)
{
  SqliteSQL sql;
  sql.add ("SELECT rowid FROM users WHERE user =");
  sql.add (user);
  sql.add ("AND sheet =");
  sql.add (sheet);
  sql.add (";");
  sqlite3 * db = connect ();
  map <string, vector <string> > result = database_sqlite_query (db, sql.sql);
  database_sqlite_disconnect (db);
  return !result["rowid"].empty ();
}


string Database_Styles::databasefolder ()
{
  return filter_url_create_root_path ("databases", "styles");
}


string Database_Styles::sheetfolder (string sheet)
{
  return filter_url_create_path (databasefolder (), sheet);
}


string Database_Styles::stylefile (string sheet, string marker)
{
  return filter_url_create_path (sheetfolder (sheet), marker);
}


unsigned int Database_Styles::data_count ()
{
  return sizeof (styles_table) / sizeof (*styles_table);
}


// Reads a style from file.
// If the file is not there, it takes the default value.
Database_Styles_Item Database_Styles::read_item (string sheet, string marker)
{
  Database_Styles_Item item;
  bool take_default = sheet.empty ();
  string filename;
  if (!take_default) {
    filename = stylefile (sheet, marker);
    if (!file_or_dir_exists (filename)) take_default = true;
  }
  if (take_default) {
    // Take the default style for the marker.
    const char * cmarker = marker.c_str();
    for (unsigned int i = 0; i < data_count(); i++) {
      if (strcmp (cmarker, styles_table[i].marker) == 0) {
        item.marker = styles_table[i].marker;
        item.name = styles_table[i].name;
        item.info = styles_table[i].info;
        item.category = styles_table[i].category;
        item.type = styles_table[i].type;
        item.subtype = styles_table[i].subtype;
        item.fontsize = styles_table[i].fontsize;
        item.italic = styles_table[i].italic;
        item.bold = styles_table[i].bold;
        item.underline = styles_table[i].underline;
        item.smallcaps = styles_table[i].smallcaps;
        item.superscript = styles_table[i].superscript;
        item.justification = styles_table[i].justification;
        item.spacebefore = styles_table[i].spacebefore;
        item.spaceafter = styles_table[i].spaceafter;
        item.leftmargin = styles_table[i].leftmargin;
        item.rightmargin = styles_table[i].rightmargin;
        item.firstlineindent = styles_table[i].firstlineindent;
        item.spancolumns = styles_table[i].spancolumns;
        item.color = styles_table[i].color;
        item.print = styles_table[i].print;
        item.userbool1 = styles_table[i].userbool1;
        item.userbool2 = styles_table[i].userbool2;
        item.userbool3 = styles_table[i].userbool3;
        item.userint1 = styles_table[i].userint1;
        item.userint2 = styles_table[i].userint2;
        item.userint3 = styles_table[i].userint3;
        item.userstring1 = styles_table[i].userstring1;
        item.userstring2 = styles_table[i].userstring2;
        item.userstring3 = styles_table[i].userstring3;
        item.backgroundcolor = styles_table[i].backgroundcolor;
        return item;
      }
    }
    // Non-standard marker.
    item.marker = marker;
    item.name = translate ("Name");
    item.info = translate ("Information");
  } else {
    // Read the style from file.
    string contents = filter_url_file_get_contents (filename);
    vector <string> lines = filter_string_explode (contents, '\n');
    for (unsigned int i = 0; i < lines.size(); i++) {
      if (i == 0)  item.marker = lines [i];
      if (i == 1)  item.name = lines [i];
      if (i == 2)  item.info = lines [i];
      if (i == 3)  item.category = lines [i];
      if (i == 4)  item.type = convert_to_int (lines [i]);
      if (i == 5)  item.subtype = convert_to_int (lines [i]);
      if (i == 6)  item.fontsize = convert_to_float (lines [i]);
      if (i == 7)  item.italic = convert_to_int (lines [i]);
      if (i == 8)  item.bold = convert_to_int (lines [i]);
      if (i == 9)  item.underline = convert_to_int (lines [i]);
      if (i == 10) item.smallcaps = convert_to_int (lines [i]);
      if (i == 11) item.superscript = convert_to_int (lines [i]);
      if (i == 12) item.justification = convert_to_int (lines [i]);
      if (i == 13) item.spacebefore = convert_to_float (lines [i]);
      if (i == 14) item.spaceafter = convert_to_float (lines [i]);
      if (i == 15) item.leftmargin = convert_to_float (lines [i]);
      if (i == 16) item.rightmargin = convert_to_float (lines [i]);
      if (i == 17) item.firstlineindent = convert_to_float (lines [i]);
      if (i == 18) item.spancolumns = convert_to_bool (lines [i]);
      if (i == 19) item.color = lines [i];
      if (i == 20) item.print = convert_to_bool (lines [i]);
      if (i == 21) item.userbool1 = convert_to_bool (lines [i]);
      if (i == 22) item.userbool2 = convert_to_bool (lines [i]);
      if (i == 23) item.userbool3 = convert_to_bool (lines [i]);
      if (i == 24) item.userint1 = convert_to_int (lines [i]);
      if (i == 25) item.userint2 = convert_to_int (lines [i]);
      if (i == 26) item.userint3 = convert_to_int (lines [i]);
      if (i == 27) item.userstring1 = lines [i];
      if (i == 28) item.userstring2 = lines [i];
      if (i == 29) item.userstring3 = lines [i];
      if (i == 30) item.backgroundcolor = lines [i];
    }
  }
  return item;
}


void Database_Styles::write_item (string sheet, Database_Styles_Item & item)
{
  // The style is saved to file here.
  // When the style is loaded again from file, the various parts of the style are loaded by line number.
  // Therefore it cannot handle strings with new lines in them.
  // Remove the new lines where appropriate.
  item.name = filter_string_str_replace ("\n", " ", item.name);
  item.name = filter_string_str_replace ("\r", " ", item.name);
  item.info = filter_string_str_replace ("\n", " ", item.info);
  item.info = filter_string_str_replace ("\r", " ", item.info);
  // Load the lines.
  vector <string> lines;
  lines.push_back (item.marker);
  lines.push_back (item.name);
  lines.push_back (item.info);
  lines.push_back (item.category);
  lines.push_back (convert_to_string (item.type));
  lines.push_back (convert_to_string (item.subtype));
  lines.push_back (convert_to_string (item.fontsize));
  lines.push_back (convert_to_string (item.italic));
  lines.push_back (convert_to_string (item.bold));
  lines.push_back (convert_to_string (item.underline));
  lines.push_back (convert_to_string (item.smallcaps));
  lines.push_back (convert_to_string (item.superscript));
  lines.push_back (convert_to_string (item.justification));
  lines.push_back (convert_to_string (item.spacebefore));
  lines.push_back (convert_to_string (item.spaceafter));
  lines.push_back (convert_to_string (item.leftmargin));
  lines.push_back (convert_to_string (item.rightmargin));
  lines.push_back (convert_to_string (item.firstlineindent));
  lines.push_back (convert_to_string (item.spancolumns));
  lines.push_back (item.color);
  lines.push_back (convert_to_string (item.print));
  lines.push_back (convert_to_string (item.userbool1));
  lines.push_back (convert_to_string (item.userbool2));
  lines.push_back (convert_to_string (item.userbool3));
  lines.push_back (convert_to_string (item.userint1));
  lines.push_back (convert_to_string (item.userint2));
  lines.push_back (convert_to_string (item.userint3));
  lines.push_back (item.userstring1);
  lines.push_back (item.userstring2);
  lines.push_back (item.userstring3);
  lines.push_back (item.backgroundcolor);
  // Save.
  string data = filter_string_implode (lines, "\n");
  filter_url_file_put_contents (stylefile (sheet, item.marker), data);
}


Database_Styles_Item::Database_Styles_Item ()
{
  // Defaults.
  type = 0;
  subtype = 0;
  fontsize = 12;
  italic = 0;
  bold = 0;
  underline = 0;
  smallcaps = 0;
  superscript = 0;
  justification = 0;
  spacebefore = 0;
  spaceafter = 0;
  leftmargin = 0;
  rightmargin = 0;
  firstlineindent = 0;
  spancolumns = false;
  color = "#000000";
  print = false;
  userbool1 = false;
  userbool2 = false;
  userbool3 = false;
  userint1 = 0;
  userint2 = 0;
  userint3 = 0;
  userstring1 = "";
  userstring2 = "";
  userstring3 = "";
  backgroundcolor = "#FFFFFF";
}
