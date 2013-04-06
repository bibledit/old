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
#include "odtutils.h"
#include "date_time_utils.h"
#include "gwrappers.h"
#include "settings.h"
#include "color.h"

OdtTextParagraph::OdtTextParagraph(vector < ustring > *lines, const ustring & stylename)
// OpenDocument Text Text Paragraph.
// If no stylename is given, it takes the "Standard" style.
{
  // Save pointer, init variables.
  mylines = lines;
  mynewline = false;

  // Build the line.
  myline = "<text:p text:style-name=\"";
  if (stylename.empty())
    myline.append("Standard");
  else
    myline.append(stylename);
  myline.append("\">");

  // Line length.
  linelength = myline.length();
}

OdtTextParagraph::~OdtTextParagraph()
{
  // Close the paragraph.
  myline.append("</text:p>");

  // Store the line.
  mylines->push_back(myline);
}

void OdtTextParagraph::newline()
{
  mynewline = true;
}

void OdtTextParagraph::plaintext(const ustring & line)
{
  if (mynewline)
    myline.append(" ");
  mynewline = false;
  if (linelength != myline.length())
    myline.append(" ");
  myline.append(line);
}

void OdtTextParagraph::spannedtext(const ustring & line, const ustring & stylename)
{
  if (mynewline)
    myline.append(" ");
  mynewline = false;
  myline.append("<text:span text:style-name=\"");
  myline.append(stylename);
  myline.append("\">");
  myline.append(line);
  myline.append("</text:span>");
}

ustring odt_content_xml_filename(const ustring & directory)
{
  return gw_build_filename(directory, "content.xml");
}

void odt_set_font(const ustring & directory, const ustring & fontname)
// Writes the font to the right files in the directory given.
{
  // Save the font in the content file.
  ReadText rt2(odt_content_xml_filename(directory), true, false);
  for (unsigned int i = 0; i < rt2.lines.size(); i++) {
    replace_text(rt2.lines[i], "Bitstream", fontname);
  }
  write_lines(odt_content_xml_filename(directory), rt2.lines);
}

void odt_insert_content(const ustring & directory, const vector < ustring > &text)
{
  vector < ustring > odtlines;
  ReadText rt(odt_content_xml_filename(directory), true, false);
  for (unsigned int i = 0; i < rt.lines.size(); i++) {
    if (rt.lines[i].find("</office:text>") == 0) {
      for (unsigned int i2 = 0; i2 < text.size(); i2++) {
        odtlines.push_back(text[i2]);
      }
    }
    odtlines.push_back(rt.lines[i]);
  }
  write_lines(odt_content_xml_filename(directory), odtlines);
}
