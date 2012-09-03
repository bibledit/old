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


#include "htmlwriter.h"
#include "tiny_utilities.h"
#include "directories.h"
#include "gwrappers.h"
#include "gtkwrappers.h"


HtmlWriter::HtmlWriter(const ustring & title, bool include_java_scripts, bool include_bibledit_css, bool include_bar_graph_css)
{
  buffer = xmlBufferCreate();
  writer = xmlNewTextWriterMemory(buffer, 0);
  xmlTextWriterSetIndent(writer, 1);
  xmlTextWriterStartDocument(writer, NULL, "UTF-8", NULL);

  xmlTextWriterStartElement(writer, BAD_CAST "html");

  xmlTextWriterStartElement(writer, BAD_CAST "head");

  xmlTextWriterStartElement(writer, BAD_CAST "meta");
  xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "content", "text/html; charset=UTF-8");
  xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "http-equiv", "content-type");
  xmlTextWriterEndElement(writer);

  xmlTextWriterStartElement(writer, BAD_CAST "title");
  xmlTextWriterWriteFormatString(writer, "%s", title.c_str());
  xmlTextWriterEndElement(writer);

  if (include_bibledit_css) {
    xmlTextWriterStartElement(writer, BAD_CAST "link");
    xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "href", "http://localhost:51516/bibledit.css");
    xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "rel", "stylesheet");
    xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "type", "text/css");
    xmlTextWriterEndElement(writer);
  }

  if (include_java_scripts) {
    xmlTextWriterStartElement(writer, BAD_CAST "SCRIPT");
    xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "TYPE", "text/javascript");
    xmlTextWriterStartComment(writer);
    xmlTextWriterWriteFormatString(writer, "\n");
    xmlTextWriterWriteFormatString(writer, "function popup(mylink, windowname)\n");
    xmlTextWriterWriteFormatString(writer, "{\n");
    xmlTextWriterWriteFormatString(writer, "  if (! window.focus)\n");
    xmlTextWriterWriteFormatString(writer, "    return true;\n");
    xmlTextWriterWriteFormatString(writer, "  var href;\n");
    xmlTextWriterWriteFormatString(writer, "  if (typeof(mylink) == 'string')\n");
    xmlTextWriterWriteFormatString(writer, "    href=mylink;\n");
    xmlTextWriterWriteFormatString(writer, "  else\n");
    xmlTextWriterWriteFormatString(writer, "    href=mylink.href;\n");
    xmlTextWriterWriteFormatString(writer, "  window.open(href, windowname, 'width=300,height=80,scrollbars=no');\n");
    xmlTextWriterWriteFormatString(writer, "  return false;\n");
    xmlTextWriterWriteFormatString(writer, "}\n");
    xmlTextWriterEndComment(writer);
    xmlTextWriterEndElement(writer);
  }

  if (include_bar_graph_css) {
    xmlTextWriterStartElement(writer, BAD_CAST "style");
    xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "type", "text/css");
    xmlTextWriterWriteFormatString(writer, "\n");
    xmlTextWriterWriteFormatString(writer, ".graph {\n");
    xmlTextWriterWriteFormatString(writer, "  border: solid 1px black;\n");
    xmlTextWriterWriteFormatString(writer, "}\n");
    xmlTextWriterWriteFormatString(writer, "\n");
    xmlTextWriterWriteFormatString(writer, ".graph thead th {\n");
    xmlTextWriterWriteFormatString(writer, "  border-bottom: double 3px black;\n");
    xmlTextWriterWriteFormatString(writer, "  padding: 1em;\n");
    xmlTextWriterWriteFormatString(writer, "}\n");
    xmlTextWriterWriteFormatString(writer, "\n");
    xmlTextWriterWriteFormatString(writer, ".graph tfoot td {\n");
    xmlTextWriterWriteFormatString(writer, "  border-top: solid 1px #999999;\n");
    xmlTextWriterWriteFormatString(writer, "  font-size: x-small;\n");
    xmlTextWriterWriteFormatString(writer, "  text-align: center;\n");
    xmlTextWriterWriteFormatString(writer, "  padding: 0.5em;\n");
    xmlTextWriterWriteFormatString(writer, "  color: #666666;\n");
    xmlTextWriterWriteFormatString(writer, "}\n");
    xmlTextWriterWriteFormatString(writer, "\n");
    xmlTextWriterWriteFormatString(writer, ".bar div { \n");
    xmlTextWriterWriteFormatString(writer, "  border-top: solid 15px #0077DD;\n");
    xmlTextWriterWriteFormatString(writer, "  background-color: #004080;\n");
    xmlTextWriterWriteFormatString(writer, "  text-align: right;\n");
    xmlTextWriterWriteFormatString(writer, "  color: white;\n");
    xmlTextWriterWriteFormatString(writer, "  float: left;\n");
    xmlTextWriterWriteFormatString(writer, "  padding-top: 0;\n");
    xmlTextWriterWriteFormatString(writer, "}\n");
    xmlTextWriterWriteFormatString(writer, "\n");
    xmlTextWriterEndElement(writer);
  }

  xmlTextWriterEndElement(writer);

  xmlTextWriterStartElement(writer, BAD_CAST "body");
}


HtmlWriter::~HtmlWriter()
{
  xmlTextWriterEndDocument(writer);
  xmlTextWriterFlush(writer);
  ustring filename = gw_build_filename(directories_get_temp(), "document.html");
  g_file_set_contents(filename.c_str(), (const gchar *)buffer->content, -1, NULL);
  gtkw_show_uri (filename, false);
  if (writer)
    xmlFreeTextWriter(writer);
  if (buffer)
    xmlBufferFree(buffer);
}


void HtmlWriter::heading(unsigned int level, const ustring & text)
{
  ustring element("h");
  element.append(convert_to_string(level));
  xmlTextWriterStartElement(writer, BAD_CAST element.c_str());
  xmlTextWriterWriteFormatString(writer, "%s", text.c_str());
  xmlTextWriterEndElement(writer);
}


void HtmlWriter::paragraph(const ustring & text)
{
  xmlTextWriterStartElement(writer, BAD_CAST "p");
  xmlTextWriterWriteFormatString(writer, "%s", text.c_str());
  xmlTextWriterEndElement(writer);
}


void HtmlWriter::bargraph(const ustring & header, const vector < ustring > &texts, const vector < unsigned int >&percentages, const ustring & footer)
{
  xmlTextWriterStartElement(writer, BAD_CAST "table");
  xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "class", "graph");

  xmlTextWriterStartElement(writer, BAD_CAST "thead");
  xmlTextWriterStartElement(writer, BAD_CAST "tr");
  xmlTextWriterStartElement(writer, BAD_CAST "th");
  xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "colspan", "3");
  xmlTextWriterWriteFormatString(writer, "%s", header.c_str());
  xmlTextWriterEndElement(writer);
  xmlTextWriterEndElement(writer);
  xmlTextWriterEndElement(writer);

  xmlTextWriterStartElement(writer, BAD_CAST "tbody");
  for (unsigned int i = 0; i < texts.size(); i++) {
    xmlTextWriterStartElement(writer, BAD_CAST "tr");
    xmlTextWriterStartElement(writer, BAD_CAST "td");
    xmlTextWriterWriteFormatString(writer, "%s", texts[i].c_str());
    xmlTextWriterEndElement(writer);
    xmlTextWriterStartElement(writer, BAD_CAST "td");
    xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "width", "400px");
    xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "class", "bar");
    xmlTextWriterStartElement(writer, BAD_CAST "div");
    xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "style", "width: %i%%", percentages[i]);
    xmlTextWriterEndElement(writer);
    xmlTextWriterEndElement(writer);
    xmlTextWriterStartElement(writer, BAD_CAST "td");
    xmlTextWriterWriteFormatString(writer, "%i%%", percentages[i]);
    xmlTextWriterEndElement(writer);
    xmlTextWriterEndElement(writer);
  }
  xmlTextWriterEndElement(writer);

  xmlTextWriterEndElement(writer);
}


void HtmlWriter::table(const ustring & main_header, const vector < ustring > &column_headers, const vector < VectorUstring > &cell_texts, const ustring & footer, vector < bool > *centers, int header_font_size)
/*
Writes a table.
main_header: The main header on the table.
column_headers: The headers for each column.
cell_texts: A double vector of texts for in the cells.
centers: A list of booleans whether the text in the cells needs to be centered.  
header_font_size: 0: Normal, negative: smaller, positive: bigger.
*/
{
  xmlTextWriterStartElement(writer, BAD_CAST "table");
  xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "border", "1");

  if (!main_header.empty() || !column_headers.empty()) {
    xmlTextWriterStartElement(writer, BAD_CAST "thead");

    xmlTextWriterStartElement(writer, BAD_CAST "tr");
    xmlTextWriterStartElement(writer, BAD_CAST "th");
    xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "colspan", "100");
    xmlTextWriterWriteString(writer, BAD_CAST main_header.c_str());
    xmlTextWriterEndElement(writer);
    xmlTextWriterEndElement(writer);

    xmlTextWriterStartElement(writer, BAD_CAST "tr");
    for (unsigned int i = 0; i < column_headers.size(); i++) {
      xmlTextWriterStartElement(writer, BAD_CAST "th");
      if (header_font_size > 0) {
        xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "style", "font-size: larger;");
      }
      if (header_font_size < 0) {
        xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "style", "font-size: smaller;");
      }
      xmlTextWriterWriteString(writer, BAD_CAST column_headers[i].c_str());
      xmlTextWriterEndElement(writer);
    }
    xmlTextWriterEndElement(writer);

    xmlTextWriterEndElement(writer);
  }

  xmlTextWriterStartElement(writer, BAD_CAST "tbody");
  for (unsigned int i = 0; i < cell_texts.size(); i++) {
    xmlTextWriterStartElement(writer, BAD_CAST "tr");
    for (unsigned int i2 = 0; i2 < cell_texts[i].size(); i2++) {
      xmlTextWriterStartElement(writer, BAD_CAST "td");
      if (centers) {
        if (i2 < centers->size()) {
          if (centers->at(i2)) {
            xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "style", "text-align: center;");
          }
        }
      }
      xmlTextWriterWriteString(writer, BAD_CAST cell_texts[i][i2].c_str());
      xmlTextWriterEndElement(writer);
    }
    xmlTextWriterEndElement(writer);
  }
  xmlTextWriterEndElement(writer);

  xmlTextWriterEndElement(writer);
}


void HtmlWriter::hyperlinkedparagraph(const ustring & text, const ustring & hyperlink)
// <p><a href="filename.html">Text</a></p>
{
  xmlTextWriterStartElement(writer, BAD_CAST "p");
  xmlTextWriterStartElement(writer, BAD_CAST "a");
  xmlTextWriterWriteAttribute(writer, BAD_CAST "href", BAD_CAST hyperlink.c_str());
  xmlTextWriterWriteFormatString(writer, "%s", text.c_str());
  xmlTextWriterEndElement(writer);
  xmlTextWriterEndElement(writer);
}

