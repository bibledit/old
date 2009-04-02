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
#include "shutdown.h"
#include "settings.h"
#include <libxml/xmlwriter.h>
#include <libxml/xmlreader.h>
#include "styles.h"


#define STYLESHEET_SQL_SUFFIX ".sql18" // Todo can go out later.
#define STYLESHEET_XML_SUFFIX ".xml1"
const char *RECOGNIZED_SUFFIXES[] = { ".sql17", ".sql18", ".xml1" };

ustring stylesheet_sql_filename(const ustring & name) // Todo verify wherever it is used.
// This returns the database's filename for a named stylesheet.
{
  return gw_build_filename(directories_get_stylesheets(), name + STYLESHEET_SQL_SUFFIX);
}

ustring stylesheet_xml_filename(const ustring & name)
// This returns the xml file's name for a named stylesheet.
{
  return gw_build_filename(directories_get_stylesheets(), name + STYLESHEET_XML_SUFFIX);
}

ustring stylesheet_template_filename() // Todo to go out later.
// This returns the database's filename of the template.
{
  return gw_build_filename(directories_get_package_data(), "stylesheet.sql");
}

ustring stylesheet_xml_template_filename()
// This returns the database's filename of the template.
{
  return gw_build_filename(directories_get_package_data(), "stylesheet.xmll");
}

void stylesheet_get_ones_available(vector < ustring > &names)
// Gets the names of the stylesheets that are there.
{
  ReadFiles rf(directories_get_stylesheets(), "", STYLESHEET_SQL_SUFFIX);
  for (unsigned int i = 0; i < rf.files.size(); i++)
    rf.files[i].erase(rf.files[i].length() - strlen(STYLESHEET_SQL_SUFFIX), strlen(STYLESHEET_SQL_SUFFIX));
  names.assign(rf.files.begin(), rf.files.end());
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
  ustring templatefile = stylesheet_template_filename();
  ustring stylefile = gw_build_filename(directories_get_stylesheets(), name + STYLESHEET_SQL_SUFFIX);
  unix_cp(templatefile, stylefile);
  // Take action depending on the type of stylesheet we're making.
  if (stylesheettype == stFull)
    return;
  set < ustring > desired_markers = stylesheet_get_styles_of_type(stylesheettype);
  // Take the new stylesheet, and remove the unwanted markers.
  vector < ustring > all_markers = stylesheet_get_markers(name, NULL);
  vector < ustring > undesired_markers;
  for (unsigned int i = 0; i < all_markers.size(); i++) {
    if (desired_markers.find(all_markers[i]) == desired_markers.end())
      undesired_markers.push_back(all_markers[i]);
  }
  for (unsigned int i = 0; i < undesired_markers.size(); i++) {
    stylesheet_delete_style(name, undesired_markers[i]);
  }
}

void stylesheet_delete(const ustring & name)
// Deletes a stylesheet.
{
  unlink(stylesheet_sql_filename(name).c_str()); // Todo delete the other one too.
}

void stylesheet_copy(const ustring & from_name, const ustring & to_name)
// Copies one stylesheet to another.
{
  unix_cp(stylesheet_sql_filename(from_name), stylesheet_sql_filename(to_name)); // Todo use both files.
}

ustring stylesheet_import(const ustring & filename)
// Imports a new stylesheet from "filename".
// It expects a file in the formst as it is given in the export function.
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
  ustring path = stylesheet_sql_filename(name);
  // Copy the database.
  unix_cp(filename, path);
  // Upgrade the stylesheet.
  stylesheets_upgrade();
  // Return the name of the stylesheet we imported;
  return name;
}

void stylesheet_export(const ustring & name, const ustring & filename)
// Exports a stylesheet.
{
  ustring originalfile = stylesheet_sql_filename(name);
  ustring destinationfile(filename);
  destinationfile.append(STYLESHEET_SQL_SUFFIX);
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


void stylesheet_get_styles(const ustring & stylesheet, vector < Style > &styles)
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
  Style style(stylesheet, marker, true);
  style.read_template(); // Todo this may need update.
}


void stylesheet_save_style(const ustring & stylesheet, const ustring & marker, const ustring & name, const ustring & info, StyleType type, int subtype, double fontsize, const ustring & italic, const ustring & bold, const ustring & underline, const ustring & smallcaps, bool superscript, const ustring & justification, double spacebefore, double spaceafter, double leftmargin, double rightmargin, double firstlineindent, bool spancolumns, unsigned int color, bool print, bool userbool1,
                           bool userbool2, bool userbool3, int userint1, int userint2, int userint3, ustring userstring1, ustring userstring2, ustring userstring3)
// Saves style information.
{
  Style style(stylesheet, marker, true);
  style.name = name;
  style.info = info;
  style.type = type;
  style.subtype = subtype;
  style.fontsize = fontsize;
  style.italic = italic;
  style.bold = bold;
  style.underline = underline;
  style.smallcaps = smallcaps;
  style.superscript = superscript;
  style.justification = justification;
  style.spacebefore = spacebefore;
  style.spaceafter = spaceafter;
  style.leftmargin = leftmargin;
  style.rightmargin = rightmargin;
  style.firstlineindent = firstlineindent;
  style.spancolumns = spancolumns;
  style.color = color;
  style.print = print;
  style.userbool1 = userbool1;
  style.userbool2 = userbool2;
  style.userbool3 = userbool3;
  style.userint1 = userint1;
  style.userint2 = userint2;
  style.userint3 = userint3;
  style.userstring1 = userstring1;
  style.userstring2 = userstring2;
  style.userstring3 = userstring3;
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


void stylesheets_upgrade()
// Upgrade older stylesheets to the currently used format.
{
  // Upgrade from *.sql17 -> *.sql18: Footnote and crossreferences no longer renumber per page.
  {
    ReadFiles rf(directories_get_stylesheets(), "", ".sql17");
    for (unsigned int i = 0; i < rf.files.size(); i++) {
      ustring filename = gw_build_filename(directories_get_stylesheets(), rf.files[i]);
      gw_message("Updating stylesheet " + filename);
      sqlite3 *db;
      char *error = NULL;
      sqlite3_open(filename.c_str(), &db);
      sqlite3_busy_timeout(db, 1000);
      sqlite3_exec(db, "update styles set userint2 = 2 where marker = 'f';", NULL, NULL, &error);
      sqlite3_exec(db, "update styles set userint2 = 2 where marker = 'x';", NULL, NULL, &error);
      sqlite3_close(db);
      ustring newfilename(filename);
      newfilename.replace(newfilename.length() - 2, 2, "18");
      unix_mv(filename, newfilename);
    }
  }

  // Convert the stylesheet from the sqlite database to xml format.
  {
    ReadFiles rf(directories_get_stylesheets(), "", ".sql18");
    for (unsigned int i = 0; i < rf.files.size(); i++) {

      gw_message("Converting stylesheet " + rf.files[i] + " to xml format");

      // Open the sqlite3 database.
      ustring sql_filename = gw_build_filename(directories_get_stylesheets(), rf.files[i]);
      sqlite3 *db;
      sqlite3_open(sql_filename.c_str(), &db);
      sqlite3_busy_timeout(db, 1000);

      // The output xml stylesheet.
      xmlBufferPtr buffer = xmlBufferCreate();
      xmlTextWriterPtr writer = xmlNewTextWriterMemory(buffer, 0);
      xmlTextWriterStartDocument(writer, NULL, "UTF-8", NULL);
      xmlTextWriterSetIndent(writer, 1);
      xmlTextWriterStartElement(writer, BAD_CAST "stylesheet");

      // Read data from database.
      SqliteReader reader(0);
      char *sql;
      sql = g_strdup_printf("select name, info, fontsize, marker, italic, bold, underline, smallcaps, superscript, justification, spacebefore, spaceafter, leftmargin, rightmargin, firstlineindent, spancolumns, type, subtype, userbool1, userbool2, userbool3, userint1, userint2, userint3, userstring1, userstring2, userstring3, color, print from styles;");
      sqlite3_exec(db, sql, reader.callback, &reader, NULL);
      g_free(sql);
      
      // Convert data.
      for (unsigned int i = 0; i < reader.ustring0.size(); i++) {

        // Open a style for the marker
        xmlTextWriterStartElement(writer, BAD_CAST "style");
				xmlTextWriterWriteFormatAttribute (writer, BAD_CAST "marker", "%s", reader.ustring3[i].c_str());

        // Write values.
        
        xmlTextWriterStartElement(writer, BAD_CAST "name");
        xmlTextWriterWriteFormatString(writer, "%s", reader.ustring0[i].c_str());
        xmlTextWriterEndElement(writer);

        xmlTextWriterStartElement(writer, BAD_CAST "info");
        xmlTextWriterWriteFormatString(writer, "%s", reader.ustring1[i].c_str());
        xmlTextWriterEndElement(writer);

        xmlTextWriterStartElement(writer, BAD_CAST "type");
        xmlTextWriterWriteFormatString(writer, "%s", reader.ustring16[i].c_str());
        xmlTextWriterEndElement(writer);

        xmlTextWriterStartElement(writer, BAD_CAST "subtype");
        xmlTextWriterWriteFormatString(writer, "%s", reader.ustring17[i].c_str());
        xmlTextWriterEndElement(writer);

        xmlTextWriterStartElement(writer, BAD_CAST "fontsize");
        xmlTextWriterWriteFormatString(writer, "%s", reader.ustring2[i].c_str());
        xmlTextWriterEndElement(writer);

        xmlTextWriterStartElement(writer, BAD_CAST "italic");
        xmlTextWriterWriteFormatString(writer, "%s", reader.ustring4[i].c_str());
        xmlTextWriterEndElement(writer);

        xmlTextWriterStartElement(writer, BAD_CAST "bold");
        xmlTextWriterWriteFormatString(writer, "%s", reader.ustring5[i].c_str());
        xmlTextWriterEndElement(writer);

        xmlTextWriterStartElement(writer, BAD_CAST "underline");
        xmlTextWriterWriteFormatString(writer, "%s", reader.ustring6[i].c_str());
        xmlTextWriterEndElement(writer);

        xmlTextWriterStartElement(writer, BAD_CAST "smallcaps");
        xmlTextWriterWriteFormatString(writer, "%s", reader.ustring7[i].c_str());
        xmlTextWriterEndElement(writer);

        xmlTextWriterStartElement(writer, BAD_CAST "superscript");
        xmlTextWriterWriteFormatString(writer, "%s", reader.ustring8[i].c_str());
        xmlTextWriterEndElement(writer);

        xmlTextWriterStartElement(writer, BAD_CAST "justification");
        xmlTextWriterWriteFormatString(writer, "%s", reader.ustring9[i].c_str());
        xmlTextWriterEndElement(writer);

        xmlTextWriterStartElement(writer, BAD_CAST "spacebefore");
        xmlTextWriterWriteFormatString(writer, "%s", reader.ustring10[i].c_str());
        xmlTextWriterEndElement(writer);

        xmlTextWriterStartElement(writer, BAD_CAST "spaceafter");
        xmlTextWriterWriteFormatString(writer, "%s", reader.ustring11[i].c_str());
        xmlTextWriterEndElement(writer);

        xmlTextWriterStartElement(writer, BAD_CAST "leftmargin");
        xmlTextWriterWriteFormatString(writer, "%s", reader.ustring12[i].c_str());
        xmlTextWriterEndElement(writer);

        xmlTextWriterStartElement(writer, BAD_CAST "rightmargin");
        xmlTextWriterWriteFormatString(writer, "%s", reader.ustring13[i].c_str());
        xmlTextWriterEndElement(writer);

        xmlTextWriterStartElement(writer, BAD_CAST "firstlineindent");
        xmlTextWriterWriteFormatString(writer, "%s", reader.ustring14[i].c_str());
        xmlTextWriterEndElement(writer);

        xmlTextWriterStartElement(writer, BAD_CAST "spancolumns");
        xmlTextWriterWriteFormatString(writer, "%s", reader.ustring15[i].c_str());
        xmlTextWriterEndElement(writer);

        xmlTextWriterStartElement(writer, BAD_CAST "color");
        xmlTextWriterWriteFormatString(writer, "%s", reader.ustring27[i].c_str());
        xmlTextWriterEndElement(writer);

        xmlTextWriterStartElement(writer, BAD_CAST "print");
        xmlTextWriterWriteFormatString(writer, "%s", reader.ustring28[i].c_str());
        xmlTextWriterEndElement(writer);

        xmlTextWriterStartElement(writer, BAD_CAST "userbool1");
        xmlTextWriterWriteFormatString(writer, "%s", reader.ustring18[i].c_str());
        xmlTextWriterEndElement(writer);

        xmlTextWriterStartElement(writer, BAD_CAST "userbool2");
        xmlTextWriterWriteFormatString(writer, "%s", reader.ustring19[i].c_str());
        xmlTextWriterEndElement(writer);

        xmlTextWriterStartElement(writer, BAD_CAST "userbool3");
        xmlTextWriterWriteFormatString(writer, "%s", reader.ustring20[i].c_str());
        xmlTextWriterEndElement(writer);

        xmlTextWriterStartElement(writer, BAD_CAST "userint1");
        xmlTextWriterWriteFormatString(writer, "%s", reader.ustring21[i].c_str());
        xmlTextWriterEndElement(writer);

        xmlTextWriterStartElement(writer, BAD_CAST "userint2");
        xmlTextWriterWriteFormatString(writer, "%s", reader.ustring22[i].c_str());
        xmlTextWriterEndElement(writer);

        xmlTextWriterStartElement(writer, BAD_CAST "userint3");
        xmlTextWriterWriteFormatString(writer, "%s", reader.ustring23[i].c_str());
        xmlTextWriterEndElement(writer);

        xmlTextWriterStartElement(writer, BAD_CAST "userstring1");
        xmlTextWriterWriteFormatString(writer, "%s", reader.ustring24[i].c_str());
        xmlTextWriterEndElement(writer);

        xmlTextWriterStartElement(writer, BAD_CAST "userstring2");
        xmlTextWriterWriteFormatString(writer, "%s", reader.ustring25[i].c_str());
        xmlTextWriterEndElement(writer);

        xmlTextWriterStartElement(writer, BAD_CAST "userstring3");
        xmlTextWriterWriteFormatString(writer, "%s", reader.ustring26[i].c_str());
        xmlTextWriterEndElement(writer);

        // Close the style.
        xmlTextWriterEndElement(writer);
      }

      // Close input.
      sqlite3_close(db);
      
      // Close document and write it to disk.
      xmlTextWriterEndDocument(writer);
      xmlTextWriterFlush(writer);
      ustring xml_filename (sql_filename);
      replace_text (xml_filename, ".sql18", ".xml1");
      g_file_set_contents(xml_filename.c_str(), (const gchar *)buffer->content, -1, NULL);

      // Free memory.
      if (writer)
        xmlFreeTextWriter(writer);
      if (buffer)
        xmlBufferFree(buffer);
		
      // Remove the database from disk.
	  //unlink (sql_filename.c_str()); // Todo temporally switched off - we also need to remove stylesheet.sql from the templates.
    }
  }

  // Note: The stylesheet.xml template has been updated thus far. Everything below this still needs to be done.

  // At the end of everything, check that we have at least one stylesheet.  
  {
    vector < ustring > stylesheets;
    stylesheet_get_ones_available(stylesheets);
    if (stylesheets.size() == 0) {
      stylesheet_create_new(STANDARDSHEET, stFull);
    }
  }
}


void stylesheet_get_recently_used(const ustring & stylesheet, vector < ustring > &markers, vector < unsigned int >&count) // Todo think of another mechanism for this.
// Read the recently used data: markers and usage count.
{
  // Some variables.  
  sqlite3 *db;
  int rc;
  char *error = NULL;
  try {
    // Connect to the database.
    rc = sqlite3_open(stylesheet_sql_filename(stylesheet).c_str(), &db);
    if (rc)
      throw runtime_error(sqlite3_errmsg(db));
    sqlite3_busy_timeout(db, 1000);
    // Read the styles.
    SqliteReader reader(0);
    char *sql;
    sql = g_strdup_printf("select marker, count from recent;");
    rc = sqlite3_exec(db, sql, reader.callback, &reader, &error);
    g_free(sql);
    if (rc != SQLITE_OK) {
      throw runtime_error(sqlite3_errmsg(db));
    }
    markers.assign(reader.ustring0.begin(), reader.ustring0.end());
    for (unsigned int i = 0; i < reader.ustring1.size(); i++) {
      count.push_back(convert_to_int(reader.ustring1[i]));
    }
  }
  catch(exception & ex) {
    gw_critical(ex.what());
  }
  // Close connection.
  sqlite3_close(db);
}


void stylesheet_set_recently_used(const ustring & stylesheet, vector < ustring > &styles, vector < unsigned int >&counts) // Todo use another mechanism for this.
{
  // Some variables.
  sqlite3 *db;
  int rc;
  char *error = NULL;
  try {
    // Connect to the database.
    rc = sqlite3_open(stylesheet_sql_filename(stylesheet).c_str(), &db);
    if (rc)
      throw runtime_error(sqlite3_errmsg(db));
    sqlite3_busy_timeout(db, 1000);
    // Variable.
    char *sql;
    // Delete all existing markers.
    sql = g_strdup_printf("delete from recent;");
    rc = sqlite3_exec(db, sql, NULL, NULL, &error);
    if (rc) {
      throw runtime_error(sqlite3_errmsg(db));
    }
    // Insert all new markers.
    for (unsigned int i = 0; i < styles.size(); i++) {
      sql = g_strdup_printf("insert into recent values ('%s', %d);", styles[i].c_str(), counts[i]);
      rc = sqlite3_exec(db, sql, NULL, NULL, &error);
      if (rc) {
        throw runtime_error(sqlite3_errmsg(db));
      }
    }
    // Free memory.
    g_free(sql);
  }
  catch(exception & ex) {
    gw_critical(ex.what());
  }
  // Close connection.
  sqlite3_close(db);
}


void stylesheet_save_style(const ustring & stylesheet, const Style & style) // Todo update for .xml files.
{
  // Some variables.  
  sqlite3 *db;
  int rc;
  char *error = NULL;
  try {
    // Connect to the database.
    rc = sqlite3_open(stylesheet_sql_filename(stylesheet).c_str(), &db);
    if (rc)
      throw runtime_error(sqlite3_errmsg(db));
    sqlite3_busy_timeout(db, 1000);
    // Variable.
    char *sql;
    // Delete possible existing style.
    sql = g_strdup_printf("delete from styles where marker = '%s';", style.marker.c_str());
    rc = sqlite3_exec(db, sql, NULL, NULL, &error);
    if (rc) {
      throw runtime_error(sqlite3_errmsg(db));
    }
    // Save the style.
    sql = g_strdup_printf("insert into styles values ('%s', '%s', '%s', %f, %i, '%s', '%s', '%s', '%s', %i, '%s', %f, %f, %f, %f, %f, %i, %i, %i, %i, %i, %i, %i, %i, %i, '%s', '%s', '%s', %d, %d);", style.marker.c_str(), double_apostrophy(style.name).c_str(), double_apostrophy(style.info).c_str(), style.fontsize, 100, // Font percentage no longer in use but left here for historical reasons.
                          style.italic.c_str(), style.bold.c_str(), style.underline.c_str(), style.smallcaps.c_str(), style.superscript, style.justification.c_str(),
                          style.spacebefore, style.spaceafter, style.leftmargin, style.rightmargin, style.firstlineindent, style.spancolumns, style.type, style.subtype, style.userbool1, style.userbool2, style.userbool3, style.userint1, style.userint2, style.userint3, style.userstring1.c_str(), style.userstring2.c_str(), style.userstring3.c_str(), style.color, style.print);
    rc = sqlite3_exec(db, sql, NULL, NULL, &error);
    if (rc) {
      throw runtime_error(sqlite3_errmsg(db));
    }
    // Free memory.
    g_free(sql);
  }
  catch(exception & ex) {
    gw_critical(ex.what());
  }
  // Close connection.
  sqlite3_close(db);
}


void stylesheet_load_style(const ustring & stylesheet, Style & style)
/*
Reads one Style.
stylesheet: which stylesheet to read from. If no stylesheet is given, it reads from the template.
style: the Style object to read. The marker is already given in the object.
*/
{
  // Some variables.  
  sqlite3 *db;
  int rc;
  char *error = NULL;
  try {
    // If the stylesheet is not given, we read from the template.
    ustring filename = stylesheet_sql_filename(stylesheet);
    if (stylesheet.empty())
      filename = stylesheet_template_filename();
    // Connect to the database.
    rc = sqlite3_open(filename.c_str(), &db);
    if (rc)
      throw runtime_error(sqlite3_errmsg(db));
    sqlite3_busy_timeout(db, 1000);
    // Read the style.
    SqliteReader reader(0);
    char *sql;
    sql = g_strdup_printf("select name, info, fontsize, fontpercentage, " "italic, bold, underline, smallcaps, superscript, justification, " "spacebefore, spaceafter, leftmargin, rightmargin, firstlineindent, spancolumns, " "type, subtype, userbool1, userbool2, userbool3, " "userint1, userint2, userint3, userstring1, userstring2, userstring3, " "color, print " "from styles where marker = '%s';", style.marker.c_str());
    rc = sqlite3_exec(db, sql, reader.callback, &reader, &error);
    g_free(sql);
    if (rc != SQLITE_OK) {
      throw runtime_error(sqlite3_errmsg(db));
    }
    if (reader.ustring0.size() > 0) {
      style.name = reader.ustring0[0];
      style.info = reader.ustring1[0];
      style.fontsize = convert_to_double(reader.ustring2[0]);
      // Font percentage no longer used.
      style.italic = reader.ustring4[0];
      style.bold = reader.ustring5[0];
      style.underline = reader.ustring6[0];
      style.smallcaps = reader.ustring7[0];
      style.superscript = convert_to_bool(reader.ustring8[0]);
      style.justification = reader.ustring9[0];
      style.spacebefore = convert_to_double(reader.ustring10[0]);
      style.spaceafter = convert_to_double(reader.ustring11[0]);
      style.leftmargin = convert_to_double(reader.ustring12[0]);
      style.rightmargin = convert_to_double(reader.ustring13[0]);
      style.firstlineindent = convert_to_double(reader.ustring14[0]);
      style.spancolumns = convert_to_bool(reader.ustring15[0]);
      style.type = (StyleType) convert_to_int(reader.ustring16[0]);
      style.subtype = convert_to_int(reader.ustring17[0]);
      style.userbool1 = convert_to_bool(reader.ustring18[0]);
      style.userbool2 = convert_to_bool(reader.ustring19[0]);
      style.userbool3 = convert_to_bool(reader.ustring20[0]);
      style.userint1 = convert_to_int(reader.ustring21[0]);
      style.userint2 = convert_to_int(reader.ustring22[0]);
      style.userint3 = convert_to_int(reader.ustring23[0]);
      style.userstring1 = reader.ustring24[0];
      style.userstring2 = reader.ustring25[0];
      style.userstring3 = reader.ustring26[0];
      style.color = convert_to_int(reader.ustring27[0]);
      style.print = convert_to_int(reader.ustring28[0]);
    }
  }
  catch(exception & ex) {
    gw_critical(ex.what());
  }
  // Close connection.
  sqlite3_close(db);
}


void stylesheet_vacuum(const ustring & stylesheet)
{
  vacuum_database(stylesheet_sql_filename(stylesheet));
}


set < ustring > stylesheet_get_styles_of_type(StylesheetType stylesheettype)
{
  set < ustring > markers;
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
      // Full stylesheet: take all styles from the stylesheet template. Todo needs update to read from the template.
      sqlite3 *db;
      int rc;
      char *error = NULL;
      try {
        rc = sqlite3_open(stylesheet_template_filename().c_str(), &db);
        if (rc)
          throw runtime_error(sqlite3_errmsg(db));
        sqlite3_busy_timeout(db, 1000);
        SqliteReader reader(0);
        char *sql;
        sql = g_strdup_printf("select marker from styles;");
        rc = sqlite3_exec(db, sql, reader.callback, &reader, &error);
        g_free(sql);
        if (rc != SQLITE_OK) {
          throw runtime_error(sqlite3_errmsg(db));
        }
        for (unsigned int i = 0; i < reader.ustring0.size(); i++) {
          markers.insert(reader.ustring0[i]);
        }
      }
      catch(exception & ex) {
        gw_critical(ex.what());
      }
      sqlite3_close(db);
      break;
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
  if (sheets.find(sheet) != sheets.end()) ;

  // Sheets is not there - take Standard, if it's around.
  else if (sheets.find(STANDARDSHEET) != sheets.end())
    sheet = STANDARDSHEET;

  // Else take first sheet in the list.
  else
    sheet = stylesheets[0];

  // Return sheet.
  return sheet;
}


// Todo to convert this in a certain order: First all reading of styles, then editing of them, then creation.
// Todo the new mechanism for the recently used ones is that on saving them, a new db may be created and then saved - reading remains as now.
// Todo reading the stylesheet is very slow. Make a new system where the whole sheet is read at once, and where the individual values 
// are read from a map<ustring, value>. This should speed it up greatly. Thus all the stylesheet are kept in memory and accessed from there.

// Todo at the end of all, generate a proper stylesheet.xml template.
