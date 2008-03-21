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


#include "libraries.h"
#include "utilities.h"
#include <glib.h>
#include "constants.h"
#include "xmlfo-utils.h"
#include "date_time_utils.h"
#include "fonts.h"
#include "gwrappers.h"
#include "settings.h"
#include "color.h"
#include "tiny_utilities.h"


void insert_empty_lines (vector<ustring> * lines)
{
  lines->push_back ("");
  lines->push_back ("");
}


XmlFoRoot::XmlFoRoot (vector<ustring> * lines)
/*
Add the root data for the xslfo file.

In XEP, you can specify multiple font-families in font-family property and if 
font-selection-strategy is set to character-by-character value, XEP can 
substitute glyphs missing in the first specified font with characters from other 
fonts in the list.
*/
{
  // Config and font data.
  extern Settings * settings;
  ProjectConfiguration * projectconfig = settings->projectconfig (settings->genconfig.project_get());
  PrintingFonts printingfonts (settings->genconfig.project_get());
  // Depending on the amount of fonts used, get the font and the font selection.
  ustring font;
  ustring font_selection_strategy;
  if (printingfonts.printing_families.size() > 1) {
    font = printingfonts.printing_families_comma_separated ();
    font_selection_strategy = " font-selection-strategy=\"character-by-character\"";
  } else {
    font = printingfonts.printing_families[0]; 
  }
  int lineheight = projectconfig->printing_line_height_get();
  // Assemble the lines to put in the xslfo file.
  mylines = lines;
  mylines->push_back ("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
  ustring line;
  line = "<fo:root xmlns:fo=\"http://www.w3.org/1999/XSL/Format\" font-family=\"" + font + "\"" + font_selection_strategy + " font-size=\"" + convert_to_string (projectconfig->printing_font_size_get()) + "pt\" text-align=\"justify\"";
  // (If the line height is 100%, do not insert anything, because appears to 
  // disturb proper calculation of the line heights with different font sizes.)
  if (lineheight != 100) line.append (" line-height=\"" + convert_to_string (lineheight) + "%\"");
  // Possible right-to-left text flow.
  if (projectconfig->right_to_left_get ()) {
    line.append (" writing-mode=\"rl-tb\"");
  }
  line.append (">");
  mylines->push_back (line);  
  insert_empty_lines (mylines);
}


XmlFoRoot::~XmlFoRoot ()
{
  insert_empty_lines (mylines);
  mylines->push_back ("</fo:root>");
}


XmlFoLayoutMasterSet::XmlFoLayoutMasterSet (vector<ustring> * lines, bool twocolumns)
{
  extern Settings * settings;
  insert_empty_lines (lines);
  ustring line;
  // Ensure a blank page does not print any headers.
  lines->push_back ("  <fo:layout-master-set>");
  line = "    <fo:simple-page-master master-name=\"blank-page\" page-width=\"21cm\" page-height=\"29.7cm\">";
  replace_text (line, "21", convert_to_string (settings->genconfig.paper_width_get ()));
  replace_text (line, "29.7", convert_to_string (settings->genconfig.paper_height_get ()));
  lines->push_back (line);
  // XEP 4 only validates if the region-body is empty, but counts white-space
  // as non-empty space. Strange behaviour, but solved so:
  lines->push_back ("      <fo:region-body></fo:region-body>");
  lines->push_back ("      <fo:region-before region-name=\"header-blank\" extent=\"0cm\"/>");
  lines->push_back ("    </fo:simple-page-master>");
  // Fill in paper size, margins.
  line = "    <fo:simple-page-master master-name=\"odd-page\" page-width=\""
       + convert_to_string (settings->genconfig.paper_width_get ()) + "cm\" page-height=\""
       + convert_to_string (settings->genconfig.paper_height_get ()) + "cm\">";
  lines->push_back (line);
  line = "      <fo:region-body margin-top=\""
       + convert_to_string (settings->genconfig.paper_top_margin_get ()) + "cm\" margin-bottom=\""
       + convert_to_string (settings->genconfig.paper_bottom_margin_get ()) + "cm\" margin-left=\""
       + convert_to_string (settings->genconfig.paper_left_margin_get ()) + "cm\" margin-right=\"" 
       + convert_to_string (settings->genconfig.paper_right_margin_get ()) + "cm\"";
  if (twocolumns)
    line.append (" column-count=\"2\"");
  line.append("></fo:region-body>");
  lines->push_back (line);
  line = "      <fo:region-before region-name=\"header-odd\" extent=\"2cm\"/>";
  replace_text (line, "2", convert_to_string (settings->genconfig.paper_top_margin_get ()));
  lines->push_back (line);
  lines->push_back ("    </fo:simple-page-master>");
  line = "    <fo:simple-page-master master-name=\"even-page\" page-width=\""
       + convert_to_string (settings->genconfig.paper_width_get ()) + "cm\" page-height=\""
       + convert_to_string (settings->genconfig.paper_height_get ()) + "cm\">";
  lines->push_back (line);
  // Mirrored paper : swap left/right margins on even pages.
  line = "      <fo:region-body margin-top=\""
       + convert_to_string (settings->genconfig.paper_top_margin_get ()) + "cm\" margin-bottom=\""
       + convert_to_string (settings->genconfig.paper_bottom_margin_get ()) + "cm\" margin-left=\""
       + convert_to_string (settings->genconfig.paper_right_margin_get ()) + "cm\" margin-right=\""
       + convert_to_string (settings->genconfig.paper_left_margin_get ()) + "cm\"";
  if (twocolumns)
    line.append (" column-count=\"2\"");
  line.append("></fo:region-body>");
  lines->push_back (line);
  line = "      <fo:region-before region-name=\"header-even\" extent=\"2cm\"/>";
  replace_text (line, "2", convert_to_string (settings->genconfig.paper_top_margin_get ()));
  lines->push_back (line);
  lines->push_back ("    </fo:simple-page-master>");
  lines->push_back ("    <fo:page-sequence-master master-name=\"text\">");
  lines->push_back ("      <fo:repeatable-page-master-alternatives>");
  lines->push_back ("        <fo:conditional-page-master-reference blank-or-not-blank=\"blank\" master-reference=\"blank-page\"/>");
  lines->push_back ("        <fo:conditional-page-master-reference odd-or-even=\"odd\" master-reference=\"odd-page\"/>");
  lines->push_back ("        <fo:conditional-page-master-reference odd-or-even=\"even\" master-reference=\"even-page\"/>");
  lines->push_back ("      </fo:repeatable-page-master-alternatives>");
  lines->push_back ("    </fo:page-sequence-master>");
  lines->push_back ("  </fo:layout-master-set>");
  insert_empty_lines (lines);
}


XmlFoLayoutMasterSet::~XmlFoLayoutMasterSet ()
{
}


XmlFoPageSequence::XmlFoPageSequence (vector<ustring> * lines, bool force_even_page_count)
{
  mylines = lines;
  insert_empty_lines (mylines);
  ustring line;
  line = "  <fo:page-sequence master-reference=\"text\"";
  if (force_even_page_count)
    line.append (" force-page-count=\"end-on-even\"");
  line.append (">");
  mylines->push_back (line);
  insert_empty_lines (mylines);
}


XmlFoPageSequence::~XmlFoPageSequence ()
{
  insert_empty_lines (mylines);
  mylines->push_back ("  </fo:page-sequence>");
  insert_empty_lines (mylines);
}


XmlFoStaticContent::XmlFoStaticContent (vector<ustring> * lines, bool chapter_left, bool chapter_right)
{
  extern Settings * settings;
  insert_empty_lines (lines);
  ustring date;
  date = date_time_julian_human_readable (date_time_julian_day_get_current (), false);
  ustring line;
  // Allow for blank pages.
  lines->push_back ("    <fo:static-content flow-name=\"header-blank\">");
  lines->push_back ("    </fo:static-content>");
  lines->push_back ("    <fo:static-content flow-name=\"header-odd\">");
  lines->push_back ("      <fo:table table-layout=\"fixed\" width=\"100%\">");
  line =            "        <fo:table-column column-width=\"2cm\"/>";
  replace_text (line, "2", convert_to_string (settings->genconfig.paper_left_margin_get ()));
  lines->push_back (line);
  lines->push_back ("        <fo:table-column column-width=\"proportional-column-width(1)\"/>");
  lines->push_back ("        <fo:table-column column-width=\"proportional-column-width(1)\"/>");
  line =            "        <fo:table-column column-width=\"1cm\"/>";
  replace_text (line, "1", convert_to_string (settings->genconfig.paper_right_margin_get ()));
  lines->push_back (line);
  lines->push_back ("        <fo:table-body>");
  line =            "          <fo:table-row height=\"1.8cm\">";
  double table_height;
  table_height = settings->genconfig.paper_top_margin_get () - 0.2;
  replace_text (line, "1.8", convert_to_string (table_height));  
  lines->push_back (line);
  lines->push_back ("            <fo:table-cell/>");
  lines->push_back ("            <fo:table-cell display-align=\"after\">");
  lines->push_back ("              <fo:block text-align=\"begin\">");
  lines->push_back ("                <fo:page-number/>");
  lines->push_back ("                <fo:leader leader-length=\"1cm\"/>");
  if (settings->genconfig.printdate_get ()) {
    lines->push_back (date);
  }
  lines->push_back ("              </fo:block>");
  lines->push_back ("            </fo:table-cell>");
  lines->push_back ("            <fo:table-cell display-align=\"after\">");
  lines->push_back ("              <fo:block text-align=\"end\">");
  lines->push_back ("                <fo:retrieve-marker retrieve-class-name=\"rightheader\"/>");
  if (chapter_right)
    lines->push_back ("                <fo:retrieve-marker retrieve-class-name=\"chapter\"/>");
  lines->push_back ("              </fo:block>");
  lines->push_back ("            </fo:table-cell>");
  lines->push_back ("            <fo:table-cell/>");
  lines->push_back ("          </fo:table-row>");
  lines->push_back ("        </fo:table-body>");
  lines->push_back ("      </fo:table>");
  lines->push_back ("    </fo:static-content>");
  lines->push_back ("    <fo:static-content flow-name=\"header-even\">");
  lines->push_back ("      <fo:table table-layout=\"fixed\" width=\"100%\">");
  line =            "        <fo:table-column column-width=\"1cm\"/>";
  replace_text (line, "1", convert_to_string (settings->genconfig.paper_right_margin_get ()));
  lines->push_back (line);
  lines->push_back ("        <fo:table-column column-width=\"proportional-column-width(1)\"/>");
  lines->push_back ("        <fo:table-column column-width=\"proportional-column-width(1)\"/>");
  line =            "        <fo:table-column column-width=\"2cm\"/>";
  replace_text (line, "2", convert_to_string (settings->genconfig.paper_left_margin_get ()));
  lines->push_back (line);
  lines->push_back ("        <fo:table-body>");
  line =            "          <fo:table-row height=\"1.8cm\">";
  replace_text (line, "1.8", convert_to_string (table_height));
  lines->push_back (line);
  lines->push_back ("            <fo:table-cell/>");
  lines->push_back ("            <fo:table-cell display-align=\"after\">");
  lines->push_back ("              <fo:block text-align=\"start\">");
  lines->push_back ("                <fo:retrieve-marker retrieve-class-name=\"leftheader\"/>");
  if (chapter_left)
    lines->push_back ("                <fo:retrieve-marker retrieve-class-name=\"chapter\"/>");
  lines->push_back ("              </fo:block>");
  lines->push_back ("            </fo:table-cell>");
 
  lines->push_back ("            <fo:table-cell display-align=\"after\">");
  lines->push_back ("              <fo:block text-align=\"end\">");
  if (settings->genconfig.printdate_get ()) {
    lines->push_back (date);
  }
  lines->push_back ("                <fo:leader leader-length=\"1cm\"/>");
  lines->push_back ("                <fo:page-number/>");
  lines->push_back ("              </fo:block>");
  lines->push_back ("            </fo:table-cell>");
  lines->push_back ("            <fo:table-cell/>");
  lines->push_back ("          </fo:table-row>");
  lines->push_back ("        </fo:table-body>");
  lines->push_back ("      </fo:table>");
  lines->push_back ("    </fo:static-content>");
  insert_empty_lines (lines);
}


XmlFoStaticContent::~XmlFoStaticContent ()
{
}


XmlFoFlow::XmlFoFlow (vector<ustring> * lines)
{
  mylines = lines;
  insert_empty_lines (mylines);
  mylines->push_back ("    <fo:flow flow-name=\"xsl-region-body\">");
  insert_empty_lines (mylines);
}


XmlFoFlow::~XmlFoFlow ()
{
  insert_empty_lines (mylines);
  mylines->push_back ("    </fo:flow>");
  insert_empty_lines (mylines);
}


XmlFoBlock::XmlFoBlock (vector<ustring> * lines, double fontsize, int lineheight,
                        const ustring& italic_in, const ustring& bold_in, 
                        const ustring& underline_in, const ustring& smallcaps_in,
                        ustring justification,
                        double spacebefore, double spaceafter,
                        double leftmargin, double rightmargin,
                        double firstlineindent, bool spancolumns,
                        bool keepwithnext)
{
  mylines = lines;
  insert_empty_lines (mylines);
  ustring line;
  line = "      <fo:block";
  // Fontsize
  line.append (" font-size=\"");
  line.append (convert_to_string (fontsize));
  line.append ("pt\"");
  // Lineheight
  if (lineheight != 100)
    line.append (" line-height=\"" + convert_to_string (lineheight) + "%\"");
  // Italic
  italic = false;
  if (italic_in == ON) {
    line.append (" font-style=\"italic\"");
    italic = true;
  }
  // Bold
  bold = false;
  if (bold_in == ON) {
    line.append (" font-weight=\"bold\"");
    bold = true;
  }
  // Underline
  underline = false;
  if (underline_in == ON) {
    line.append (" text-decoration=\"underline\"");
    underline = true;
  }
  // Smallcaps
  smallcaps = false;
  if (smallcaps_in == ON) {
    line.append (" font-variant=\"small-caps\"");
    smallcaps = true;
  }
  // Justification
  if (justification == "last-justify")
    line.append (" text-align-last=\"justify\"");
  else {
    if (!justification.empty()) {
      line.append (" text-align=\"");
      line.append (justification);
      line.append ("\"");
    }
  }
  // Space before
  line.append (" space-before=\"");
  line.append (convert_to_string (spacebefore));
  line.append ("mm\"");
  // Space after
  line.append (" space-after=\"");
  line.append (convert_to_string (spaceafter));
  line.append ("mm\"");
  // Left margin
  line.append (" start-indent=\"");
  line.append (convert_to_string (leftmargin));
  line.append ("mm\"");
  // Right margin
  line.append (" end-indent=\"");
  line.append (convert_to_string (rightmargin));
  line.append ("mm\"");
  // First line indent
  line.append (" text-indent=\"");
  line.append (convert_to_string (firstlineindent));
  line.append ("mm\"");
  // Spanning of columns.
  if (spancolumns)
    line.append (" span=\"all\"");
  // Prepare for possible float.
  line.append (" intrusion-displace=\"line\"");
  // Set widows and orphans.
  line.append (" widows=\"2\" orphans=\"2\"");
  // Keeping with next paragraph.
  if (keepwithnext)
    line.append (" keep-with-next.within-column=\"always\"");
  // Disregard line height adjustment as a result of e.g. superscript.
  line.append (" line-height-shift-adjustment=\"disregard-shifts\"");
  
  // Close xml.
  line.append (">");
  mylines->push_back (line);
  // Finally store lines count.
  linescount = mylines->size();
}


XmlFoBlock::~XmlFoBlock ()
{
  // If nothing was inserted, insert a non-breaking space.
  if (linescount == mylines->size()) {
    mylines->push_back (" ");
  }
  // Close the block.
  mylines->push_back ("      </fo:block>");
  insert_empty_lines (mylines);
}
