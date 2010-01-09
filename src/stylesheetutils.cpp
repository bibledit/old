/*
 ** Copyright (©) 2003-2009 Teus Benschop.
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
#include "stylesheetutils.h"
#include "directories.h"
#include "constants.h"
#include "xmlutils.h"
#include "gwrappers.h"
#include "gtkwrappers.h"
#include "shell.h"
#include "sqlite3.h"
#include "sqlite_reader.h"
#include "unixwrappers.h"
#include "tiny_utilities.h"
#include "maintenance.h"
#include "settings.h"
#include <libxml/xmlwriter.h>
#include <libxml/xmlreader.h>
#include "styles.h"
#include "paratext.h"


#define STYLESHEET_XML_SUFFIX ".xml2"
const char *RECOGNIZED_SUFFIXES[] = { ".xml1", ".xml2" };


ustring stylesheet_recent_filename()
// This returns the name of the database that contains data for recently used styles.
{
  return gw_build_filename(directories_get_stylesheets(), "recent.sql");
}


ustring stylesheet_xml_filename(const ustring & name)
// This returns the xml file's name for a named stylesheet.
{
  return gw_build_filename(directories_get_stylesheets(), name + STYLESHEET_XML_SUFFIX);
}


ustring stylesheet_xml_template_filename()
// This returns the database's filename of the template.
{
  return gw_build_filename(directories_get_package_data(), "stylesheet.xml");
}


void stylesheet_get_ones_available(vector <ustring>& names)
// Gets the names of the stylesheets that are there.
{
  names.clear();
  ReadFiles rf(directories_get_stylesheets(), "", STYLESHEET_XML_SUFFIX);
  for (unsigned int i = 0; i < rf.files.size(); i++) {
    ustring name = rf.files[i];
    name.erase(name.length() - strlen(STYLESHEET_XML_SUFFIX), strlen(STYLESHEET_XML_SUFFIX));
    if (!name.empty()) {
      names.push_back (name);
    }
  }
}


const char *stylesheet_basic[] = { "id", "h", "mt1", "c", "s1", "r", "p", "v", "q1", "m" };
const char *stylesheet_paragraph[] = { "rem", "mt2", "imt", "ip", "ms", "mr", "nb" };
const char *stylesheet_word_note[] = { "qt", "nd", "f", "fr", "ft", "fq" };

const char *stylesheet_sil_best_practice[] = { "c", "p", "r", "s", "v", "f", "fk", "fq", "fr", "ft", "fv", "h", "id", "mt", "mt2", "mt3", "b", "d", "m", "q", "q2", "q3", "qa", "qc", "qm", "qm2", "qm3", "qr", "qs", "qt",
  "x", "xk", "xo", "xq", "xt", "ie", "im", "imq", "imt", "imt2", "imt3", "io", "io2", "io3", "ior", "ip", "ipr", "iq", "iq2", "is", "is2", "cov", "intro", "pref", "pub", "pubinfo", "spine", "toc", "toc1", "toc2",
  "toc3", "conc", "glo", "idx", "k", "maps", "w", "ca", "cl", "fig", "h1", "h2", "h3", "ide", "lit", "mi", "mr", "ms", "nb", "pc", "pi1", "pi2", "pm", "pmc", "pmo", "pmr", "ps", "rem", "restore", "rq", "s2", "sp",
  "va", "li", "li2", "li3", "li4", "tc1", "tc2", "tc3", "tc4", "tcr1", "tcr2", "tcr3", "tcr4", "th1", "th2", "th3", "th4", "thr1", "thr2", "thr3", "thr4", "tr", "add", "bk", "cls", "nd", "ord", "sig", "tl"
};


void stylesheet_create_new(const ustring & name, StylesheetType stylesheettype)
// Create a new stylesheet, named "name", from the template.
{
  // Copy the template database.
  ustring templatefile = stylesheet_xml_template_filename();
  ustring stylefile = stylesheet_xml_filename (name);
  unix_cp(templatefile, stylefile);
 
  // Take action depending on the type of stylesheet we're making.
  if (stylesheettype == stFull)
    return;
  set <ustring> desired_markers = stylesheet_get_styles_of_type(stylesheettype);
  // Take the new stylesheet, and remove the unwanted markers.
  vector <ustring> all_markers = stylesheet_get_markers(name, NULL);
  vector <ustring> undesired_markers;
  for (unsigned int i = 0; i < all_markers.size(); i++) {
    if (desired_markers.find(all_markers[i]) == desired_markers.end())
      undesired_markers.push_back(all_markers[i]);
  }
  for (unsigned int i = 0; i < undesired_markers.size(); i++) {
    stylesheet_delete_style(name, undesired_markers[i]);
  }
}


void stylesheet_delete(const ustring & name)
// Deletes a stylesheet from disk.
// Note that it continues to live in memory once it's there, till after restart.
{
  unlink(stylesheet_xml_filename(name).c_str());
}


void stylesheet_copy(const ustring & from_name, const ustring & to_name)
// Copies one stylesheet to another.
{
  unix_cp(stylesheet_xml_filename(from_name), stylesheet_xml_filename(to_name));
}


ustring stylesheet_import(const ustring & filename)
// Imports a new stylesheet from "filename".
// It expects a file in the format as it is given in the export function.
{
  // See that the sheet is a recognized one.
  // Derive the name of the new stylesheet from the filename.
  ustring name;
  for (unsigned int i = 0; i < (sizeof(RECOGNIZED_SUFFIXES) / sizeof(*RECOGNIZED_SUFFIXES)); i++) {
    if (g_str_has_suffix(filename.c_str(), RECOGNIZED_SUFFIXES[i])) {
      name = gw_path_get_basename(filename);
      name.erase(name.length() - strlen(RECOGNIZED_SUFFIXES[i]), strlen(RECOGNIZED_SUFFIXES[i]));
    }
  }
  if (name.empty()) {
    gtkw_dialog_error(NULL, filename + ": Unrecognized stylesheet");
    return "";
  }
  // Check whether it already exists.
  if (stylesheet_exists(name)) {
    gtkw_dialog_error(NULL, "Stylesheet " + name + " already exists");
    return "";
  }
  // Get the path of the new stylesheet.
  ustring path = stylesheet_xml_filename(name);
  // Copy the file.
  unix_cp(filename, path);
  // Upgrade the stylesheet.
  stylesheets_upgrade();
  // Return the name of the stylesheet we imported;
  return name;
}


void stylesheet_export_bibledit(const ustring & name, const ustring & filename)
// Exports a stylesheet in standard bibledit format.
{
  ustring originalfile = stylesheet_xml_filename(name);
  ustring destinationfile(filename);
  destinationfile.append(STYLESHEET_XML_SUFFIX);
  unix_cp(originalfile, destinationfile);
}


bool stylesheet_exists(const ustring & name)
// Returns whether this stylesheet exists.
{
  vector < ustring > sheets;
  stylesheet_get_ones_available(sheets);
  set < ustring > existing_sheets(sheets.begin(), sheets.end());
  return (existing_sheets.find(name) != existing_sheets.end());
}


void stylesheet_get_styles(const ustring & stylesheet, vector <Style>& styles)
// Get all data of all stylesheets.
{
  // Get available markers.
  vector < ustring > markers = stylesheet_get_markers(stylesheet, NULL);
  // Read all styles.
  for (unsigned int i = 0; i < markers.size(); i++) {
    Style style(stylesheet, markers[i], false);
    styles.push_back(style);
  }
}


vector <ustring> stylesheet_get_markers(const ustring & stylesheet, vector <ustring> *names)
// This function only gets the markers from the stylesheet.
// Hence it is faster than the similar function that gets the full styles.
// If "names" is non-NULL it gets the names from the stylesheet too.
{
  extern Styles * styles;
  Stylesheet * sheet = styles->stylesheet (stylesheet);
  vector < ustring > markers;
  for (unsigned int i = 0; i < sheet->styles.size(); i++) {
    markers.push_back (sheet->styles[i]->marker);
    if (names) {
      names->push_back (sheet->styles[i]->name);
    }
  }
  return markers;
}


void stylesheet_delete_style(const ustring & stylesheet, const ustring & marker)
// Deletes a style.
{
  extern Styles * styles;
  Stylesheet * sheet = styles->stylesheet (stylesheet);
  sheet->erase (marker);
  sheet->save();
}


void stylesheet_new_style(const ustring & stylesheet, const ustring & marker)
// Adds a new style. Searches template for data.
{
  // Pointer to the styles object.
  extern Styles * styles;
  
  // The stylesheet to which to add data.
  Stylesheet * sheet = styles->stylesheet (stylesheet);
  
  // If the style is in the standard template, copy it over into the stylesheet.
  Stylesheet * standard = styles->stylesheet ("");
  for (unsigned int i = 0; i < standard->styles.size(); i++) {
    StyleV2 * style = standard->styles[i];
    if (style->marker == marker) {
      // Make deep copy and store it into the stylesheet.
      StyleV2 * newstyle = new StyleV2 (*style);
      sheet->insert (newstyle);
      sheet->save();
      return;
    }
  }

  // Create a default new style for the marker.
  StyleV2 * style = new StyleV2 (0);
  style->marker = marker;
  sheet->insert (style);
  sheet->save();  
}


void stylesheet_save_style(const ustring & stylesheet, const ustring & marker, const ustring & name, const ustring & info, StyleType type, int subtype, double fontsize, const ustring & italic, const ustring & bold, const ustring & underline, const ustring & smallcaps, bool superscript, const ustring & justification, double spacebefore, double spaceafter, double leftmargin, double rightmargin, double firstlineindent, bool spancolumns, unsigned int color, bool print, bool userbool1, bool userbool2, bool userbool3, int userint1, int userint2, int userint3, ustring userstring1, ustring userstring2, ustring userstring3)
// Saves style information.
{
  extern Styles * styles;
  Stylesheet * sheet = styles->stylesheet (stylesheet);
  StyleV2 * style = sheet->style(marker);
  style->name = name;
  style->info = info;
  style->type = type;
  style->subtype = subtype;
  style->fontsize = fontsize;
  style->italic = italic;
  style->bold = bold;
  style->underline = underline;
  style->smallcaps = smallcaps;
  style->superscript = superscript;
  style->justification = justification;
  style->spacebefore = spacebefore;
  style->spaceafter = spaceafter;
  style->leftmargin = leftmargin;
  style->rightmargin = rightmargin;
  style->firstlineindent = firstlineindent;
  style->spancolumns = spancolumns;
  style->color = color;
  style->print = print;
  style->userbool1 = userbool1;
  style->userbool2 = userbool2;
  style->userbool3 = userbool3;
  style->userint1 = userint1;
  style->userint2 = userint2;
  style->userint3 = userint3;
  style->userstring1 = userstring1;
  style->userstring2 = userstring2;
  style->userstring3 = userstring3;
  sheet->save();  
}


int stylesheet_style_get_pointer(const vector < Style > &styles, const ustring & marker)
// Returns a pointer to "styles" which describes "marker".
// Or -1 if not found.
{
  // Search marker and return pointer.
  for (unsigned int i = 0; i < styles.size(); i++)
    if (marker == styles[i].marker)
      return i;
  // Ok, marker not found.
  return -1;
}


void stylesheet_upgrade_value (vector <ustring>& sheet, const ustring& marker, const ustring& element, const ustring& value)
// Updates the value of a marker in the stylesheet.
{
  ustring toggle_on_string = "marker=\"" + marker + "\"";
  ustring toggle_off_string = "marker=\"";
  bool within_marker = false;
  ustring element_opener = "<" + element + ">";
  ustring element_closer = "</" + element + ">";
  for (unsigned int i = 0; i < sheet.size(); i++) {
    ustring line = sheet[i];
    if (line.find (toggle_off_string) != string::npos) {
      within_marker = false;
    }
    if (line.find (toggle_on_string) != string::npos) {
      within_marker = true;
    }
    if (within_marker) {
      size_t startpos = line.find (element_opener);
      if (startpos != string::npos) {
        size_t endpos = line.find (element_closer);
        if (endpos != string::npos) {
          line.erase (startpos + element_opener.length(), endpos - startpos - element_opener.length());
          line.insert (startpos + element_opener.length(), value);
          sheet[i] = line;
        }
      }
    }
  }
}


void stylesheets_upgrade()
// Upgrade older stylesheets to the currently used format.
{
  // Make the \r marker a section heading instead of a normal paragraph.
  {
    ReadFiles rf(directories_get_stylesheets(), "", ".xml1");
    for (unsigned int i = 0; i < rf.files.size(); i++) {
      ustring filename = gw_build_filename(directories_get_stylesheets(), rf.files[i]);
      gw_message("Updating stylesheet " + filename);
      ReadText rt (filename, true, false);
      stylesheet_upgrade_value (rt.lines, "r", "subtype", "2");
      unlink (filename.c_str());
      ustring newfilename(filename);
      newfilename.replace(newfilename.length() - 1, 1, "2");
      write_lines (newfilename, rt.lines);
    }
  }

  // Note: The stylesheet.xml template has been updated thus far.
  // Every update listed below still needs to be entered into the template.

  // At the end of everything, check that we have at least one stylesheet.  
  {
    vector < ustring > stylesheets;
    stylesheet_get_ones_available(stylesheets);
    if (stylesheets.size() == 0) {
      stylesheet_create_new(STANDARDSHEET, stFull);
    }
  }
}


void stylesheet_get_recently_used(const ustring & stylesheet, vector <ustring>& markers, vector <unsigned int>& count)
// Read the recently used data: markers and usage count.
{
  // Bail out if the database doesn't exist.
  ustring filename = stylesheet_recent_filename();
  if (!g_file_test (filename.c_str(), G_FILE_TEST_IS_REGULAR))
    return;
  // Connect to the database.
  sqlite3 *db;
  sqlite3_open(stylesheet_recent_filename().c_str(), &db);
  sqlite3_busy_timeout(db, 1000);
  // Read the styles.
  SqliteReader reader(0);
  char *sql;
  sql = g_strdup_printf("select marker, count from '%s';", double_apostrophy (stylesheet).c_str());
  sqlite3_exec(db, sql, reader.callback, &reader, NULL);
  g_free(sql);
  markers.assign(reader.ustring0.begin(), reader.ustring0.end());
  for (unsigned int i = 0; i < reader.ustring1.size(); i++) {
    count.push_back(convert_to_int(reader.ustring1[i]));
  }
}


void stylesheet_set_recently_used(const ustring & stylesheet, vector <ustring>& styles, vector <unsigned int>& counts)
{
  // Connect to db.
  ustring filename = stylesheet_recent_filename();
  sqlite3 *db;
  sqlite3_open(filename.c_str(), &db);
  char *sql;
  // Delete all existing markers.
  sql = g_strdup_printf("delete from '%s';", double_apostrophy (stylesheet).c_str());
  int rc = sqlite3_exec(db, sql, NULL, NULL, NULL);
  g_free(sql);
  // If deleting the markers failed, create the table for the stylesheet.
  if (rc != SQLITE_OK) {
    sql = g_strdup_printf("create table '%s' (marker string, count integer);", double_apostrophy (stylesheet).c_str());
    sqlite3_exec(db, sql, NULL, NULL, NULL);
    g_free(sql);
  }
  // Insert all new markers.
  for (unsigned int i = 0; i < styles.size(); i++) {
    sql = g_strdup_printf("insert into '%s' values ('%s', %d);", double_apostrophy (stylesheet).c_str(), styles[i].c_str(), counts[i]);
    sqlite3_exec(db, sql, NULL, NULL, NULL);
    g_free(sql);
  }
  // Register database write access, for possible maintenance once in a while.
  maintenance_register_database (filename);
}


void stylesheet_save_style(const ustring & stylesheet, const Style & style)
{
  extern Styles * styles;
  Stylesheet * sheet = styles->stylesheet (stylesheet);
  StyleV2 * stylev2 = sheet->style (style.marker);
  if (stylev2) {
    stylev2->marker = style.marker;
    stylev2->name = style.name;
    stylev2->info = style.info;
    stylev2->type = style.type;
    stylev2->subtype = style.subtype;
    stylev2->fontsize = style.fontsize;
    stylev2->italic = style.italic;
    stylev2->bold = style.bold;
    stylev2->underline = style.underline;
    stylev2->smallcaps = style.smallcaps;
    stylev2->superscript = style.superscript;
    stylev2->justification = style.justification;
    stylev2->spacebefore = style.spacebefore;
    stylev2->spaceafter = style.spaceafter;
    stylev2->leftmargin = style.leftmargin;
    stylev2->rightmargin = style.rightmargin;
    stylev2->firstlineindent = style.firstlineindent;
    stylev2->spancolumns = style.spancolumns;
    stylev2->color = style.color;
    stylev2->print = style.print;
    stylev2->userbool1 = style.userbool1;
    stylev2->userbool2 = style.userbool2;
    stylev2->userbool3 = style.userbool3;
    stylev2->userint1 = style.userint1;
    stylev2->userint2 = style.userint2;
    stylev2->userint3 = style.userint3;
    stylev2->userstring1 = style.userstring1;
    stylev2->userstring2 = style.userstring2;
    stylev2->userstring3 = style.userstring3;
  }
  sheet->save();
}


void stylesheet_load_style(const ustring & stylesheet, Style& style)
/*
Reads one Style.
stylesheet: which stylesheet to read from. If no stylesheet is given, it reads from the template.
style: the Style object to read. The marker is already given in the object.
*/
{
  extern Styles * styles;
  Stylesheet * sheet = styles->stylesheet (stylesheet);
  StyleV2 * stylev2 = sheet->style (style.marker);
  if (stylev2) {
    style.marker = stylev2->marker;
    style.name = stylev2->name;
    style.info = stylev2->info;
    style.type = stylev2->type;
    style.subtype = stylev2->subtype;
    style.fontsize = stylev2->fontsize;
    style.italic = stylev2->italic;
    style.bold = stylev2->bold;
    style.underline = stylev2->underline;
    style.smallcaps = stylev2->smallcaps;
    style.superscript = stylev2->superscript;
    style.justification = stylev2->justification;
    style.spacebefore = stylev2->spacebefore;
    style.spaceafter = stylev2->spaceafter;
    style.leftmargin = stylev2->leftmargin;
    style.rightmargin = stylev2->rightmargin;
    style.firstlineindent = stylev2->firstlineindent;
    style.spancolumns = stylev2->spancolumns;
    style.color = stylev2->color;
    style.print = stylev2->print;
    style.userbool1 = stylev2->userbool1;
    style.userbool2 = stylev2->userbool2;
    style.userbool3 = stylev2->userbool3;
    style.userint1 = stylev2->userint1;
    style.userint2 = stylev2->userint2;
    style.userint3 = stylev2->userint3;
    style.userstring1 = stylev2->userstring1;
    style.userstring2 = stylev2->userstring2;
    style.userstring3 = stylev2->userstring3;
  }
}


set <ustring> stylesheet_get_styles_of_type(StylesheetType stylesheettype)
{
  set <ustring> markers;
  switch (stylesheettype) {
  case stBasicParagraphWordNote:
    {
      // Store the markes that are in, and fall through to the next type.
      for (unsigned int i = 0; i < (sizeof(stylesheet_word_note) / sizeof(*stylesheet_word_note)); i++) {
        markers.insert(stylesheet_word_note[i]);
      }
    }
  case stBasicParagraph:
    {
      // Store the markes that are in, and fall through to the next type.
      for (unsigned int i = 0; i < (sizeof(stylesheet_paragraph) / sizeof(*stylesheet_paragraph)); i++) {
        markers.insert(stylesheet_paragraph[i]);
      }
    }
  case stBasic:
    {
      // Store the markers that are in.
      for (unsigned int i = 0; i < (sizeof(stylesheet_basic) / sizeof(*stylesheet_basic)); i++) {
        markers.insert(stylesheet_basic[i]);
      }
      break;
    }
  case stSilBestPractice:
    {
      // Store the markers that are in.
      for (unsigned int i = 0; i < (sizeof(stylesheet_sil_best_practice) / sizeof(*stylesheet_sil_best_practice)); i++) {
        markers.insert(stylesheet_sil_best_practice[i]);
      }
      break;
    }
  case stFull:
    {
      // Full stylesheet: take all styles from the stylesheet template.
      extern Styles * styles;
      Stylesheet * sheet = styles->stylesheet("");
      for (unsigned int i = 0; i < sheet->styles.size(); i++) {
        markers.insert(sheet->styles[i]->marker);
      }
    }
  }
  return markers;
}


ustring stylesheet_get_actual ()
// Gets the actual stylesheet.
{
  // Get the stylesheet from the configuration, with a fallback.
  extern Settings * settings;
  ustring sheet = settings->genconfig.stylesheet_get();
  if (sheet.empty()) {
    sheet = STANDARDSHEET;
  }
  
  // See whether it exists.
  vector < ustring > stylesheets;
  stylesheet_get_ones_available(stylesheets);
  set < ustring > sheets(stylesheets.begin(), stylesheets.end());

  // Sheet is there? Fine.
  if (sheets.find(sheet) != sheets.end()) {
  }

  // Sheets is not there - take Standard, if it's around.
  else if (sheets.find(STANDARDSHEET) != sheets.end()) {
    sheet = STANDARDSHEET;
  }

  // Else take first sheet in the list.
  else {
    sheet = stylesheets[0];
  }

  // Return sheet.
  return sheet;
}


void stylesheet_export_paratext(const ustring & name, ustring filename)
// Exports a stylesheet in Paratext format.
{
  ExportParatextStylesheet export_paratext_sheet (0);
  export_paratext_sheet.convert (name);
  export_paratext_sheet.save (filename);
}


