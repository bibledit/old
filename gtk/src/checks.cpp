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


#include "checks.h"
#include "directories.h"
#include "gwrappers.h"
#include "xmlutils.h"
#include "settings.h"
#include "bible.h"
#include "style.h"
#include "tiny_utilities.h"
#include "utilities.h"
#include "gtkwrappers.h"
#include <glib/gi18n.h>

void checks_output_references_comments(const vector < ustring > &references, const vector < ustring > &comments)
// Outputs the results of check to stdout.
{
  for (unsigned int i = 0; i < references.size(); i++) {
    ustring line = references[i] + " " + comments[i] + "\n";
    if (write(1, line.c_str(), strlen(line.c_str()))) ;
  }
}


void checks_display_references_comments(vector < ustring > &references, vector < ustring > &comments, WindowReferences * references_window)
// Displays the results of the checks to the user.
{
  vector < Reference > refs;
  for (unsigned int i = 0; i < references.size(); i++) {
    Reference ref(0);
    reference_discover(0, 0, "", references[i], ref.book, ref.chapter, ref.verse);
    refs.push_back(ref);
  }
  extern Settings *settings;
  references_window->set (refs, settings->genconfig.project_get(), &comments);
}


DisplayCheckingResults::DisplayCheckingResults(const ustring & title)
{
  // Opening html code.
  start(title);
}


void DisplayCheckingResults::word_inventory(const vector < ustring > &words, const vector < unsigned int >&count, int total, int unique, int filteredtotal, int filteredunique, int excludelimit)
{
  ustring line;
  // Heading and information.
  heading(2, _("Word Inventory"));
  paragraph(_("Total word count: ") + convert_to_string(total));
  paragraph(_("Unique word count: ") + convert_to_string(unique));
  if (filteredtotal != total) {
    paragraph(_("Words with a count of ") + convert_to_string(excludelimit) + _(" and up were filtered out."));
    paragraph(_("Filtered total word count: ") + convert_to_string(filteredtotal));
    paragraph(_("Filtered unique word count: ") + convert_to_string(filteredunique));
  }
  paragraph(_("Click on a word to see the places where it occurs."));

  // Open the columns.
  open_table();
  open_table_column();
  add_table_cell(_("Word"), true, false);
  add_table_cell(_("Count"), true, true);
  close_table_column();

  // Add the data.
  for (unsigned int i = 0; i < words.size(); i++) {
    open_table_column();
    line = search_whole_words(words[i], words[i]);
    add_table_cell(line, false, false, false);
    add_table_cell(convert_to_string(count[i]), false, true);
    close_table_column();
  }

  // Close everything.  
  close_table();
  finalize();

  // Write data and display it.
  ustring outputfilename;
  outputfilename = gw_build_filename(Directories->get_temp(), "wordinventory.html");
  write_lines(outputfilename, html);
  gtkw_show_uri (outputfilename);
}

void DisplayCheckingResults::usfm_count(const vector < ustring > &usfms, const vector < unsigned int >&count, const ustring & stylesheet)
{
  ustring line;
  // Heading and information.
  heading(2, _("Marker Count"));

  // Open the columns.
  open_table();
  open_table_column();
  add_table_cell(_("Marker"), true, false);
  add_table_cell(_("Count"), true, true);
  add_table_cell(_("Description"), true, false);
  close_table_column();

  // Add the data.
  for (unsigned int i = 0; i < usfms.size(); i++) {
    open_table_column();
    add_table_cell(usfms[i], false, false);
    add_table_cell(convert_to_string(count[i]), false, true);
    Style style(stylesheet, usfms[i], false);
    add_table_cell(style.name, false, false);
    close_table_column();
  }

  // Close everything.  
  close_table();
  finalize();

  // Write data and display it.
  ustring outputfilename;
  outputfilename = gw_build_filename(Directories->get_temp(), "usfmcount.html");
  write_lines(outputfilename, html);
  gtkw_show_uri (outputfilename);
}

void DisplayCheckingResults::character_count(const vector < ustring > &chars, const vector < unsigned int >&count)
{
  ustring line;
  // Heading and information.
  heading(2, _("Character Count"));

  // Open the columns.
  open_table();
  open_table_column();
  add_table_cell(_("Character"), true, false);
  add_table_cell(_("Decimal"), true, false);
  add_table_cell(_("Hexadecimal"), true, false);
  add_table_cell(_("Count"), true, true);
  close_table_column();

  // Add the data.
  for (unsigned int i = 0; i < chars.size(); i++) {
    open_table_column();
    add_table_cell(chars[i], false, false);
    ustring dec = character_to_decimal_entity(chars[i]);
    xml_handle_entities(dec, NULL);
    add_table_cell(dec, false, false);
    add_table_cell(character_to_hexadecimal_entity(chars[i]), false, false);
    add_table_cell(convert_to_string(count[i]), false, true);
    close_table_column();
  }

  // Close everything.  
  close_table();
  finalize();

  // Write data and display it.
  ustring outputfilename;
  outputfilename = gw_build_filename(Directories->get_temp(), "charactercount.html");
  write_lines(outputfilename, html);
  gtkw_show_uri (outputfilename);
}

void DisplayCheckingResults::references_inventory(const vector < ustring > &verses, const vector < ustring > &references)
{
  ustring line;
  // Heading and information.
  heading(2, _("References Inventory"));

  // Open the columns.
  open_table();
  open_table_column();
  add_table_cell(_("Verse"), true, false);
  add_table_cell(_("References"), true, false);
  close_table_column();

  // Add the data.
  for (unsigned int i = 0; i < verses.size(); i++) {
    open_table_column();
    add_table_cell(verses[i], false, false);
    add_table_cell(references[i], false, false);
    close_table_column();
  }

  // Close everything.  
  close_table();
  finalize();

  // Write data and display it.
  ustring outputfilename;
  outputfilename = gw_build_filename(Directories->get_temp(), "referenceinventory.html");
  write_lines(outputfilename, html);
  gtkw_show_uri (outputfilename);
}

void DisplayCheckingResults::ot_quotations_in_nt(const vector <ustring> nt_refs, const vector <ustring> nt_texts, const vector <VectorUstring>& ot_refs, const vector <VectorUstring>& ot_texts, const gchar * mainheading)
{
  heading(2, mainheading);
  open_table();

  ustring column_references = _("New Testament");
  ustring column_referents = _("Old Testament");
  open_table_column();
  extern Settings * settings;
  if (settings->session.check_output_in_ot_order) {
    add_table_cell(column_referents, true, true);
    add_table_cell("", true, false);
    add_table_cell(column_references, true, true);
    add_table_cell("", true, false);
  } else {
    add_table_cell(column_references, true, true);
    add_table_cell("", true, false);
    add_table_cell(column_referents, true, true);
    add_table_cell("", true, false);
  }
  close_table_column();

  for (unsigned int i = 0; i < nt_refs.size(); i++) {
    open_table_column();
    add_table_cell(nt_refs[i], false, true);
    add_table_cell(nt_texts[i], false, false);
    for (unsigned int i2 = 0; i2 < ot_refs[i].size(); i2++) {
      if (i2) {
        close_table_column();
        open_table_column();
        add_table_cell("", false, false);
        add_table_cell("", false, false);
      }
      add_table_cell(ot_refs[i][i2], false, true);
      add_table_cell(ot_texts[i][i2], false, false);
    }
    close_table_column();
  }

  close_table();
  finalize();

  ustring outputfilename;
  outputfilename = gw_build_filename(Directories->get_temp(), "nt-ot-quotations.html");
  write_lines(outputfilename, html);
  gtkw_show_uri (outputfilename);
}

void DisplayCheckingResults::parallel_passages(const vector < OtNtParallelDataSection > &data, const gchar * mainheading)
{
  heading(2, mainheading);

  for (unsigned int i = 0; i < data.size(); i++) {
    heading(3, data[i].title);
    open_table();
    for (unsigned int i2 = 0; i2 < data[i].sets.size(); i2++) {
      open_table_column();
      for (unsigned int i3 = 0; i3 < data[i].sets[i2].data.size(); i3++) {
        add_table_cell(data[i].sets[i2].data[i3], false, false);
      }
      close_table_column();
    }
    close_table();
  }

  finalize();

  ustring outputfilename;
  outputfilename = gw_build_filename(Directories->get_temp(), "parallelpassages.html");
  write_lines(outputfilename, html);
  gtkw_show_uri (outputfilename);
}

void DisplayCheckingResults::start(const ustring & title)
{
  html.push_back("<html>");
  html.push_back("<head>");
  html.push_back("<meta content=\"text/html; charset=UTF-8\" http-equiv=\"content-type\">");
  html.push_back("<title>" + title + "</title>");
  html.push_back("<link href=\"http://localhost:51516/bibledit.css\" rel=\"stylesheet\" type=\"text/css\" />");
  html.push_back("<SCRIPT TYPE=\"text/javascript\">");
  html.push_back("<!--");
  html.push_back("function popup(mylink, windowname)");
  html.push_back("{");
  html.push_back("  if (! window.focus)");
  html.push_back("    return true;");
  html.push_back("  var href;");
  html.push_back("  if (typeof(mylink) == 'string')");
  html.push_back("    href=mylink;");
  html.push_back("  else");
  html.push_back("    href=mylink.href;");
  html.push_back("  window.open(href, windowname, 'width=300,height=80,scrollbars=no');");
  html.push_back("  return false;");
  html.push_back("}");
  html.push_back("//-->");
  html.push_back("</SCRIPT>");
  html.push_back("</head>");
  html.push_back("<body>");
}

void DisplayCheckingResults::heading(unsigned int level, const ustring & text)
{
  ustring line;
  line = "<h";
  line.append(convert_to_string(level));
  line.append(">");
  line.append(text);
  line.append("</h");
  line.append(convert_to_string(level));
  line.append(">");
  html.push_back(line);
}

void DisplayCheckingResults::paragraph(const ustring & text)
{
  ustring line;
  line = "<p>";
  line.append(text);
  line.append("</p>");
  html.push_back(line);
}

void DisplayCheckingResults::open_table()
{
  html.push_back("<table border=\"0\" cellpadding=\"2\" cellspacing=\"15\">");
  html.push_back("<tbody>");
  html.push_back("</tr>");
}

void DisplayCheckingResults::close_table()
{
  html.push_back("</tr>");
  html.push_back("</tbody>");
  html.push_back("</table>");
}

void DisplayCheckingResults::open_table_column()
{
  html.push_back("<tr>");
}

void DisplayCheckingResults::close_table_column()
{
  html.push_back("</tr>");
}

void DisplayCheckingResults::add_table_cell(const ustring & text, bool bold, bool right_aligned, bool translate_entities)
{
  ustring line;
  line = "<td";
  if (right_aligned)
    line.append(" style=\"text-align: right;\" valign=\"undefined\"");
  line.append(">");
  if (bold)
    line.append("<b>");
  ustring text2(text);
  if (translate_entities)
    xml_handle_entities(text2, NULL);
  line.append(text2);
  if (bold)
    line.append("</b>");
  line.append("</td>");
  html.push_back(line);
}

void DisplayCheckingResults::finalize()
{
  html.push_back("<br>");
  html.push_back("</body>");
  html.push_back("</html>");
}

ustring DisplayCheckingResults::search_whole_words(const ustring & text, const ustring & word)
{
  ustring s;
  s = "<A HREF=\"http://localhost:51516/bibledit_loads_references.html?search-whole-word=";
  s.append(word);
  s.append("\" onClick=\"return popup(this,'bibledit')\">");
  s.append(text);
  s.append("</A>");
  return s;
}

ustring get_non_alphanumericals(const ustring & s)
{
  ustring result;
  for (unsigned int i = 0; i < s.length(); i++) {
    ustring ch = s.substr(i, 1);
    gunichar gu = g_utf8_get_char(ch.c_str());
    if (!g_unichar_isalnum(gu))
      result.append(ch);
  }
  return result;
}

CollectCheckingResults::CollectCheckingResults(int dummy)
{
}

void CollectCheckingResults::add(const vector < ustring > &refs, const vector < ustring > &cmts)
{
  for (unsigned int i = 0; i < refs.size(); i++) {
    references.push_back(refs[i]);
    comments.push_back(cmts[i]);
  }
}
