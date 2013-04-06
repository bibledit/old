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
#include "utilities.h"
#include <glib.h>
#include "constants.h"
#include "osis.h"
#include "gwrappers.h"
#include "bible.h"
#include "books.h"
#include "tiny_utilities.h"

void osis_insert_empty_lines(WriteText * wt)
{
  wt->text("\n");
  wt->text("\n");
}

OsisRoot::OsisRoot(WriteText * wt, const ustring & name, const ustring & description)
// Add the root data for the osis file.
{
  mywt = wt;
  mywt->text("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
  mywt->text("\n");
  mywt->text("<osis xmlns=\"http://www.bibletechnologies.net/2003/OSIS/namespace\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://www.bibletechnologies.net/2003/OSIS/namespace osisCore.2.1.xsd\">\n");
  mywt->text("\n");
  mywt->text("<osisText osisIDWork=\"" + name + "\" osisRefWork=\"defaultReferenceScheme\" xml:lang=\"en\">\n");
  mywt->text("\n");
  mywt->text(" <header>\n");
  mywt->text("  <work osisWork=\"" + name + "\">\n");
  mywt->text("   <title>" + description + "</title>\n");
  mywt->text("   <identifier type=\"OSIS\">Bible." + name + "</identifier>\n");
  mywt->text("   <refSystem>Bible." + name + "</refSystem>\n");
  mywt->text("  </work>\n");
  mywt->text("  <work osisWork=\"defaultReferenceScheme\">\n");
  mywt->text("   <refSystem>Bible." + name + "</refSystem>\n");
  mywt->text("  </work>\n");
  mywt->text(" </header>\n");
  osis_insert_empty_lines(mywt);
}

OsisRoot::~OsisRoot()
{
  osis_insert_empty_lines(mywt);
  mywt->text("</osisText>\n");
  mywt->text("</osis>\n");
}

OsisBook::OsisBook(WriteText * wt, const ustring & english_book)
// Add the book data for the osis file.
{
  // Save variables.
  mywt = wt;
  // Get the abbreviation to be used.
  unsigned int index = books_english_to_id(english_book);
  book = books_id_to_osis(index);
  // Insert xml.  
  osis_insert_empty_lines(mywt);
  mywt->text("  <div type=\"book\" osisID=\"" + book + "\">\n");
  osis_insert_empty_lines(mywt);
}

OsisBook::~OsisBook()
{
  osis_insert_empty_lines(mywt);
  mywt->text("  </div>\n");
  osis_insert_empty_lines(mywt);
}

OsisChapter::OsisChapter(WriteText * wt, const ustring & book, unsigned int chapter_in)
// Add the chapter data for the osis file.
{
  // Save variables.
  mywt = wt;
  chapter = chapter_in;
  osis_insert_empty_lines(mywt);
  mywt->text("   <chapter osisID=\"" + book + "." + convert_to_string(chapter) + "\">\n");
  osis_insert_empty_lines(mywt);
}

OsisChapter::~OsisChapter()
{
  osis_insert_empty_lines(mywt);
  mywt->text("   </chapter>");
  osis_insert_empty_lines(mywt);
}

OsisVerse::OsisVerse(WriteText * wt, const ustring & book, unsigned int chapter, const ustring & verse)
// Add the verse data for the osis file.
// Segmented verses like "6a", e.g. in verse "2-6a" are supposed to be given like:
// <verse osisID="Matt.1.2 Matt.1.3 Matt.1.4 Matt.1.5 Matt.1.6!a">
{
  // Save variables.
  mywt = wt;

  // Get the separate verses in cases of sequences and the like.
  vector < ustring > verses = verse_range_sequence_v2(verse);

  // Go through all separate verses to assemble the OSIS verse.
  ustring osisverse;
  for (unsigned int i = 0; i < verses.size(); i++) {
    // Change the "a" into the "!a" as required. Same for the "b".
    replace_text(verses[i], "a", "!a");
    replace_text(verses[i], "b", "!b");
    // Assemble the verse.
    if (i)
      osisverse.append(" ");
    osisverse.append(book + "." + convert_to_string(chapter) + "." + verses[i]);
  }

  // Insert xml code. 
  wt->text("    <verse osisID=\"");
  wt->text(osisverse);
  wt->text("\">");
}

OsisVerse::~OsisVerse()
{
  mywt->text("</verse>\n");
  mywt->text("\n");
}

OsisParagraph::OsisParagraph(WriteText * wt)
// Open a paragraph.
{
  // Save variables.
  mywt = wt;
//  mywt->text ("<div type=\"section\">"); 
// It seems that the newer osis2mod compiler does not support these sections anymore.
//  mywt->text ("<p>"); 
}

OsisParagraph::~OsisParagraph()
// Close a paragraph.
{
//  mywt->text ("</div>");
//  mywt->text ("</p>");
}
