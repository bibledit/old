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

#include "libraries.h"
#include "utilities.h"
#include <glib.h>
#include "constants.h"
#include "date_time_utils.h"
#include "gwrappers.h"
#include "settings.h"
#include "color.h"
#include "tiny_utilities.h"
#include "xmlfo-utils.h"

XmlFoBlock::XmlFoBlock(vector < ustring > *lines, double fontsize, int lineheight, const ustring & italic_in, const ustring & bold_in, const ustring & underline_in, const ustring & smallcaps_in, ustring justification, double spacebefore, double spaceafter, double leftmargin, double rightmargin, double firstlineindent, bool spancolumns, bool keepwithnext)
{
  mylines = lines;
  ustring line;
  line = "      <fo:block";
  // Fontsize
  line.append(" font-size=\"");
  line.append(convert_to_string(fontsize));
  line.append("pt\"");
  // Lineheight
  if (lineheight != 100)
    line.append(" line-height=\"" + convert_to_string(lineheight) + "%\"");
  // Italic
  italic = false;
  if (italic_in == ON) {
    line.append(" font-style=\"italic\"");
    italic = true;
  }
  // Bold
  bold = false;
  if (bold_in == ON) {
    line.append(" font-weight=\"bold\"");
    bold = true;
  }
  // Underline
  underline = false;
  if (underline_in == ON) {
    line.append(" text-decoration=\"underline\"");
    underline = true;
  }
  // Smallcaps
  smallcaps = false;
  if (smallcaps_in == ON) {
    line.append(" font-variant=\"small-caps\"");
    smallcaps = true;
  }
  // Justification
  if (justification == "last-justify")
    line.append(" text-align-last=\"justify\"");
  else {
    if (!justification.empty()) {
      line.append(" text-align=\"");
      line.append(justification);
      line.append("\"");
    }
  }
  // Space before
  line.append(" space-before=\"");
  line.append(convert_to_string(spacebefore));
  line.append("mm\"");
  // Space after
  line.append(" space-after=\"");
  line.append(convert_to_string(spaceafter));
  line.append("mm\"");
  // Left margin
  line.append(" start-indent=\"");
  line.append(convert_to_string(leftmargin));
  line.append("mm\"");
  // Right margin
  line.append(" end-indent=\"");
  line.append(convert_to_string(rightmargin));
  line.append("mm\"");
  // First line indent
  line.append(" text-indent=\"");
  line.append(convert_to_string(firstlineindent));
  line.append("mm\"");
  // Spanning of columns.
  if (spancolumns)
    line.append(" span=\"all\"");
  // Prepare for possible float.
  line.append(" intrusion-displace=\"line\"");
  // Set widows and orphans.
  line.append(" widows=\"2\" orphans=\"2\"");
  // Keeping with next paragraph.
  if (keepwithnext)
    line.append(" keep-with-next.within-column=\"always\"");
  // Disregard line height adjustment as a result of e.g. superscript.
  line.append(" line-height-shift-adjustment=\"disregard-shifts\"");

  // Close xml.
  line.append(">");
  mylines->push_back(line);
  // Finally store lines count.
  linescount = mylines->size();
}

XmlFoBlock::~XmlFoBlock()
{
  // If nothing was inserted, insert a non-breaking space.
  if (linescount == mylines->size()) {
    mylines->push_back(" ");
  }
  // Close the block.
  mylines->push_back("      </fo:block>");
}
