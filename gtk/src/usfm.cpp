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

#include "libraries.h"
#include "usfm.h"
#include "utilities.h"
#include "directories.h"
#include "style.h"
#include "stylesheetutils.h"

Usfm::Usfm(const ustring & stylesheet_in)
{
  stylesheet = stylesheet_in;
  reload();
}

Usfm::~Usfm()
{
}

void Usfm::reload()
{
  styles.clear();
  stylesheet_get_styles(stylesheet, styles);
}

bool Usfm::is_identifier(const ustring & usfm)
{
  get_style_pointer(usfm);
  if (last_style_pointer < 0)
    return false;
  return (styles[last_style_pointer].type == stIdentifier);
}

IdentifierType Usfm::identifier_get_subtype(const ustring & usfm)
{
  get_style_pointer(usfm);
  if (last_style_pointer < 0)
    return itComment;
  return (IdentifierType) styles[last_style_pointer].subtype;
}

bool Usfm::is_starting_paragraph(const ustring & usfm)
{
  get_style_pointer(usfm);
  if (last_style_pointer < 0)
    return false;
  return (styles[last_style_pointer].type == stStartsParagraph);
}

ParagraphType Usfm::paragraph_get_subtype(const ustring & usfm)
{
  get_style_pointer(usfm);
  if (last_style_pointer < 0)
    return ptNormalParagraph;
  return (ParagraphType) styles[last_style_pointer].subtype;
}

bool Usfm::is_inline_text(const ustring & usfm)
{
  get_style_pointer(usfm);
  if (last_style_pointer < 0)
    return false;
  return (styles[last_style_pointer].type == stInlineText);
}

bool Usfm::is_chapter_number(const ustring & usfm)
{
  get_style_pointer(usfm);
  if (last_style_pointer < 0)
    return false;
  return (styles[last_style_pointer].type == stChapterNumber);
}

bool Usfm::is_verse_number(const ustring & usfm)
{
  get_style_pointer(usfm);
  if (last_style_pointer < 0)
    return false;
  return (styles[last_style_pointer].type == stVerseNumber);
}

bool Usfm::is_foot_endnote(const ustring & usfm)
{
  get_style_pointer(usfm);
  if (last_style_pointer < 0)
    return false;
  return (styles[last_style_pointer].type == stFootEndNote);
}

FootEndNoteType Usfm::foot_endnote_get_subtype(const ustring & usfm)
{
  get_style_pointer(usfm);
  if (last_style_pointer < 0)
    return fentContent;
  return (FootEndNoteType) styles[last_style_pointer].subtype;
}

bool Usfm::is_xref(const ustring & usfm)
{
  get_style_pointer(usfm);
  if (last_style_pointer < 0)
    return false;
  return (styles[last_style_pointer].type == stCrossreference);
}

CrossreferenceType Usfm::xref_get_subtype(const ustring & usfm)
{
  get_style_pointer(usfm);
  if (last_style_pointer < 0)
    return ctContent;
  return (CrossreferenceType) styles[last_style_pointer].subtype;
}

bool Usfm::is_peripheral(const ustring & usfm)
{
  get_style_pointer(usfm);
  if (last_style_pointer < 0)
    return false;
  return (styles[last_style_pointer].type == stPeripheral);
}

PeripheralType Usfm::peripheral_get_subtype(const ustring & usfm)
{
  get_style_pointer(usfm);
  if (last_style_pointer < 0)
    return ptSpine;
  return (PeripheralType) styles[last_style_pointer].subtype;
}

bool Usfm::is_picture(const ustring & usfm)
{
  get_style_pointer(usfm);
  if (last_style_pointer < 0)
    return false;
  return (styles[last_style_pointer].type == stPicture);
}

bool Usfm::is_pagebreak(const ustring & usfm)
{
  get_style_pointer(usfm);
  if (last_style_pointer < 0)
    return false;
  return (styles[last_style_pointer].type == stPageBreak);
}

bool Usfm::is_table_element(const ustring & usfm)
{
  get_style_pointer(usfm);
  if (last_style_pointer < 0)
    return false;
  return (styles[last_style_pointer].type == stTableElement);
}

TableElementType Usfm::table_element_get_subtype(const ustring & usfm)
{
  get_style_pointer(usfm);
  if (last_style_pointer < 0)
    return tetCell;
  return (TableElementType) styles[last_style_pointer].subtype;
}

ustring Usfm::name()
{
  return styles[last_style_pointer].name;
}

double Usfm::fontsize()
{
  return styles[last_style_pointer].fontsize;
}

ustring Usfm::italic()
{
  return styles[last_style_pointer].italic;
}

ustring Usfm::bold()
{
  return styles[last_style_pointer].bold;
}

ustring Usfm::underline()
{
  return styles[last_style_pointer].underline;
}

ustring Usfm::smallcaps()
{
  return styles[last_style_pointer].smallcaps;
}

bool Usfm::superscript()
{
  return styles[last_style_pointer].superscript;
}

ustring Usfm::justification()
{
  return styles[last_style_pointer].justification;
}

double Usfm::spacebefore()
{
  return styles[last_style_pointer].spacebefore;
}

double Usfm::spaceafter()
{
  return styles[last_style_pointer].spaceafter;
}

double Usfm::leftmargin()
{
  return styles[last_style_pointer].leftmargin;
}

double Usfm::rightmargin()
{
  return styles[last_style_pointer].rightmargin;
}

double Usfm::firstlineindent()
{
  return styles[last_style_pointer].firstlineindent;
}

bool Usfm::spancolumns()
{
  return styles[last_style_pointer].spancolumns;
}

unsigned int Usfm::color()
{
  return styles[last_style_pointer].color;
}

bool Usfm::userbool1()
{
  return styles[last_style_pointer].userbool1;
}

bool Usfm::userbool2()
{
  return styles[last_style_pointer].userbool2;
}

bool Usfm::userbool3()
{
  return styles[last_style_pointer].userbool3;
}

int Usfm::userint1()
{
  return styles[last_style_pointer].userint1;
}

int Usfm::get_style_pointer(const ustring & usfm)
// Returns a pointer to the style relevant for "usfm", or -1 is the style is not found.
{
  // Optimization to speed up finding the pointer: if found before, reuse the result.
  if (usfm != last_marker) {
    last_style_pointer = stylesheet_style_get_pointer(styles, usfm);
    last_marker = usfm;
  }
  return last_style_pointer;
}

bool usfm_is_id(const ustring & marker)
{
  return (marker == "id");
}

bool usfm_is_chapter(const ustring & marker)
{
  return (marker == "c");
}

bool usfm_is_verse(const ustring & marker)
{
  return (marker == "v");
}
