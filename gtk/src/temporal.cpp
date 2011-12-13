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

#include "temporal.h"
#include "utilities.h"
#include <libxml/xmlwriter.h>
#include "bible.h"
#include "books.h"

void temporal_convert_parallel_passages()
// This was used to convert file NT_order_of_OT_Quotations_in_NT.pps to xml.
// Then for converting NT_Parallel_Passages.pps.
// Then for converting OT_Parallel_Passages.pps.
{
  bool set_opened = false;
  xmlBufferPtr buffer = xmlBufferCreate();
  xmlTextWriterPtr writer = xmlNewTextWriterMemory(buffer, 0);
  xmlTextWriterStartDocument(writer, NULL, "UTF-8", NULL);
  xmlTextWriterSetIndent(writer, 1);
  xmlTextWriterStartElement(writer, BAD_CAST "ot-parallel-passages");

  ReadText rt("/home/joe/parallel-passages/OT_Parallel_Passages.pps", true);
  for (unsigned int i = 0; i < rt.lines.size(); i++) {
    if (rt.lines[i].find("\\key ") == 0) {
      rt.lines[i].erase(0, 5);
      xmlTextWriterStartElement(writer, BAD_CAST "section");
      xmlTextWriterWriteAttribute(writer, BAD_CAST "title", BAD_CAST rt.lines[i].c_str());
    }
    if (rt.lines[i].find("\\ref ") == 0) {
      if (!set_opened) {
        xmlTextWriterStartElement(writer, BAD_CAST "set");
        set_opened = true;
      }
      xmlTextWriterStartElement(writer, BAD_CAST "reference");
      rt.lines[i].erase(0, 5);
      ustring book, chapter, verse;
      decode_reference(rt.lines[i], book, chapter, verse);
      book = books_id_to_english(books_paratext_to_id(book));
      xmlTextWriterWriteAttribute(writer, BAD_CAST "book", BAD_CAST book.c_str());
      xmlTextWriterWriteAttribute(writer, BAD_CAST "chapter", BAD_CAST chapter.c_str());
      xmlTextWriterWriteAttribute(writer, BAD_CAST "verse", BAD_CAST verse.c_str());
      xmlTextWriterEndElement(writer);
    }
    if (rt.lines[i].empty() || (rt.lines[i].find("\\com") == 0)) {
      xmlTextWriterEndElement(writer);
      set_opened = false;
    }
    if (rt.lines[i].empty()) {
      xmlTextWriterEndElement(writer);
    }
  }

  xmlTextWriterEndDocument(writer);
  xmlTextWriterFlush(writer);
  g_file_set_contents("/home/joe/ot-parallel-passages.xml", (const gchar *)buffer->content, -1, NULL);
  if (writer)
    xmlFreeTextWriter(writer);
  if (buffer)
    xmlBufferFree(buffer);
}
