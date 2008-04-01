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
#include "stylesheetutils.h"
#include "directories.h"
#include "constants.h"
#include "xmlutils.h"
#include "gwrappers.h"
#include "gtkwrappers.h"
#include "shell.h"
#include "sqlite3.h"
#include "sqlite_reader.h"
#include "vacuum.h"
#include "unixwrappers.h"
#include "tiny_utilities.h"


#define STYLESHEET_SUFFIX ".sql11"
char *RECOGNIZED_SUFFIXES [] = { ".sql6", ".sql7", ".sql8", ".sql9", ".sql10", ".sql11" };


ustring stylesheet_filename (const ustring& name)
// This returns the database's filename for a named stylesheet.
{
  return gw_build_filename (directories_get_stylesheets (), name + STYLESHEET_SUFFIX);
}


ustring stylesheet_template_filename ()
// This returns the database's filename of the template.
{
  return gw_build_filename (directories_get_package_data (), "stylesheet.sql");
}


void stylesheet_get_ones_available (vector<ustring>& names)
// Gets the names of the stylesheets that are there.
{
  ReadFiles rf (directories_get_stylesheets (), "", STYLESHEET_SUFFIX);
  for (unsigned int i = 0; i < rf.files.size(); i++)
    rf.files[i].erase (rf.files[i].length() - strlen (STYLESHEET_SUFFIX), strlen (STYLESHEET_SUFFIX));
  names.assign (rf.files.begin(), rf.files.end());
}


char * stylesheet_basic [] = { "id", "h", "mt1", "c", "s1", "r", "p", "v", "q1", "m" };
char * stylesheet_paragraph [] = { "rem", "mt2", "imt", "ip", "ms", "mr", "nb" };
char * stylesheet_word_note [] = { "qt", "nd", "f", "fr", "ft", "fq" };
char * stylesheet_sil_best_practice [] = { 
  "c", "p", "r", "s", "v", "f", "fk", "fq", "fr", "ft", "fv", "h", "id", "mt", 
  "mt2", "mt3", "b", "d", "m", "q", "q2", "q3", "qa", "qc", "qm", "qm2", "qm3", 
  "qr", "qs", "qt", "x", "xk", "xo", "xq", "xt", "ie", "im", "imq", "imt", 
  "imt2", "imt3", "io", "io2", "io3", "ior", "ip", "ipr", "iq", "iq2", "is", 
  "is2", "cov", "intro", "pref", "pub", "pubinfo", "spine", "toc", "toc1", 
  "toc2", "toc3", "conc", "glo", "idx", "k", "maps", "w", "ca", "cl", "fig", 
  "h1", "h2", "h3", "ide", "lit", "mi", "mr", "ms", "nb", "pc", "pi1", "pi2", 
  "pm", "pmc", "pmo", "pmr", "ps", "rem", "restore", "rq", "s2", "sp", "va", 
  "li", "li2", "li3", "li4", "tc1", "tc2", "tc3", "tc4", "tcr1", "tcr2", "tcr3", 
  "tcr4", "th1", "th2", "th3", "th4", "thr1", "thr2", "thr3", "thr4", "tr", 
  "add", "bk", "cls", "nd", "ord", "sig", "tl" 
};
  

void stylesheet_create_new (const ustring& name, StylesheetType stylesheettype)
// Create a new stylesheet, named "name", from the template.
{
  // Copy the template database.
  ustring templatefile = stylesheet_template_filename ();
  ustring stylefile = gw_build_filename (directories_get_stylesheets (), name + STYLESHEET_SUFFIX);
  unix_cp (templatefile, stylefile);
  // Take action depending on the type of stylesheet we're making.
  if (stylesheettype == stFull) return;
  set<ustring> desired_markers = stylesheet_get_styles_of_type (stylesheettype);
  // Take the new stylesheet, and remove the unwanted markers.
  vector<ustring> all_markers = stylesheet_get_markers (name, NULL);
  vector<ustring> undesired_markers;
  for (unsigned int i = 0; i < all_markers.size(); i++) {
    if (desired_markers.find (all_markers[i]) == desired_markers.end())
      undesired_markers.push_back (all_markers[i]);
  }
  for (unsigned int i = 0; i < undesired_markers.size(); i++) {
    stylesheet_delete_style (name, undesired_markers[i]);
  }  
}


void stylesheet_delete (const ustring& name)
// Deletes a stylesheet.
{
  unlink (stylesheet_filename (name).c_str());
}


void stylesheet_copy (const ustring& from_name, const ustring& to_name)
// Copies one stylesheet to another.
{
  unix_cp (stylesheet_filename (from_name), stylesheet_filename (to_name));
}


ustring stylesheet_import (const ustring& filename)
// Imports a new stylesheet from "filename".
// It expects a file in the formst as it is given in the export function.
{
  // See that the sheet is a recognized one.
  // Derive the name of the new stylesheet from the filename.
  ustring name;
  for (unsigned int i = 0; i < (sizeof (RECOGNIZED_SUFFIXES) / sizeof (*RECOGNIZED_SUFFIXES)); i++) {
    if (g_str_has_suffix (filename.c_str(), RECOGNIZED_SUFFIXES[i])) {
      name = gw_path_get_basename (filename);
      name.erase (name.length() - strlen (RECOGNIZED_SUFFIXES[i]), strlen (RECOGNIZED_SUFFIXES[i]));
    }
  }
  if (name.empty()) {
    gtkw_dialog_error (NULL, filename + ": Unrecognized stylesheet");
    return "";
  }
  // Check whether it already exists.
  if (stylesheet_exists (name)) {
    gtkw_dialog_error (NULL, "Stylesheet " + name +  " already exists");
    return "";
  }
  // Get the path of the new stylesheet.
  ustring path = stylesheet_filename (name);
  // Copy the database.
  unix_cp (filename, path);
  // Upgrade the stylesheet.
  stylesheets_upgrade ();
  // Return the name of the stylesheet we imported;
  return name;
}


void stylesheet_export (const ustring& name, const ustring& filename)
// Exports a stylesheet.
{
  ustring originalfile = stylesheet_filename (name);
  ustring destinationfile (filename);
  destinationfile.append (STYLESHEET_SUFFIX);
  unix_cp (originalfile, destinationfile);
}


bool stylesheet_exists (const ustring& name)
// Returns whether this stylesheet exists.
{
  vector<ustring> sheets;
  stylesheet_get_ones_available (sheets);
  set<ustring> existing_sheets (sheets.begin(), sheets.end());
  return (existing_sheets.find (name) != existing_sheets.end());
}


void stylesheet_get_styles (const ustring& stylesheet, vector<Style>& styles)
// Get all data of all stylesheets.
{
  // Get available markers.
  vector<ustring> markers = stylesheet_get_markers (stylesheet, NULL);
  // Read all styles.
  for (unsigned int i = 0; i < markers.size(); i++) {
    Style style (stylesheet, markers[i], false);
    styles.push_back (style);
  }
}


vector<ustring> stylesheet_get_markers (const ustring& stylesheet, vector<ustring> * names)
/*
This function only gets the markers and the names of the styles of the stylesheet,
and is therefore faster than the similar function that gets the whole style.
It is intended to be used in such situation that only the markers and/or the
name of a style is needed, such as in the styletree.
*/
{
  // Store styles
  vector<ustring> markers;
  // Some variables.  
  sqlite3 *db;
  int rc;
  char *error = NULL;
  try
  {
    // Connect to the database.
    rc = sqlite3_open(stylesheet_filename (stylesheet).c_str (), &db);
    if (rc) throw runtime_error (sqlite3_errmsg(db));
    sqlite3_busy_timeout (db, 1000);
    // Read the available markers.
    SqliteReader reader (0);
    char * sql;
    sql = g_strdup_printf ("select marker, name from styles;");
    rc = sqlite3_exec(db, sql, reader.callback, &reader, &error);
    g_free (sql);
    if (rc != SQLITE_OK) {
      throw runtime_error (sqlite3_errmsg(db));
    }
    markers.assign (reader.ustring0.begin(), reader.ustring0.end());
    if (names)
      names->assign (reader.ustring1.begin(), reader.ustring1.end());
  }
  catch (exception & ex)
  {
    gw_critical (ex.what ());
  }
  // Close connection.
  sqlite3_close (db);
  // Return result.
  return markers;
}


void stylesheet_delete_style (const ustring& stylesheet, const ustring& marker)
// Deletes a style.
{
  // Some variables.  
  sqlite3 *db;
  int rc;
  char *error = NULL;
  try
  {
    // Connect to the database.
    rc = sqlite3_open(stylesheet_filename (stylesheet).c_str (), &db);
    if (rc) throw runtime_error (sqlite3_errmsg(db));
    sqlite3_busy_timeout (db, 1000);
    // Delete style.
    char * sql;
    sql = g_strdup_printf ("delete from styles where marker = '%s';", marker.c_str());
    rc = sqlite3_exec (db, sql, NULL, NULL, &error);
    if (rc) {
      throw runtime_error (sqlite3_errmsg(db));
    }
    g_free (sql);
  }
  catch (exception & ex)
  {
    gw_critical (ex.what ());
  }
  // Close connection.
  sqlite3_close (db);
}


void stylesheet_new_style (const ustring& stylesheet, const ustring& marker)
// Adds a new style. Searches template for data.
{
  Style style (stylesheet, marker, true);
  style.read_template ();
}


void stylesheet_save_style (const ustring& stylesheet, const ustring& marker,
                            const ustring& name, const ustring& info,
                            StyleType type, int subtype,
                            double fontsize, int fontpercentage,
                            const ustring& italic, const ustring& bold, 
                            const ustring& underline, const ustring& smallcaps,
                            bool superscript, const ustring& justification,
                            double spacebefore, double spaceafter,
                            double leftmargin, double rightmargin,
                            double firstlineindent, bool spancolumns,
                            unsigned int color, bool print,
                            bool userbool1, bool userbool2, bool userbool3,
                            int userint1, int userint2, int userint3,
                            ustring userstring1, ustring userstring2, ustring userstring3)
// Saves style information.
{
  Style style (stylesheet, marker, true);
  style.name = name;
  style.info = info;
  style.type = type;
  style.subtype = subtype;
  style.fontsize = fontsize;
  style.fontpercentage = fontpercentage;
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


int stylesheet_style_get_pointer (const vector<Style>& styles, const ustring& marker)
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


void stylesheets_upgrade ()
// Upgrade older stylesheets to the currently used format.
{
  // At the end of everything, check that we have at least one stylesheet.  
  {
    vector<ustring> stylesheets;
    stylesheet_get_ones_available (stylesheets);
    if (stylesheets.size() == 0) {
      stylesheet_create_new (STANDARDSHEET, stFull);
    }
  }
}


void stylesheet_get_recently_used (const ustring& stylesheet, vector<ustring>& markers, vector<unsigned int>& count)
// Read the recently used data: markers and usage count.
{
  // Some variables.  
  sqlite3 *db;
  int rc;
  char *error = NULL;
  try
  {
    // Connect to the database.
    rc = sqlite3_open(stylesheet_filename (stylesheet).c_str (), &db);
    if (rc) throw runtime_error (sqlite3_errmsg(db));
    sqlite3_busy_timeout (db, 1000);
    // Read the styles.
    SqliteReader reader (0);
    char * sql;
    sql = g_strdup_printf ("select marker, count from recent;");
    rc = sqlite3_exec(db, sql, reader.callback, &reader, &error);
    g_free (sql);
    if (rc != SQLITE_OK) {
      throw runtime_error (sqlite3_errmsg(db));
    }
    markers.assign (reader.ustring0.begin(), reader.ustring0.end());
    for (unsigned int i = 0; i < reader.ustring1.size(); i++) {
      count.push_back (convert_to_int (reader.ustring1[i]));
    }
  }
  catch (exception & ex)
  {
    gw_critical (ex.what ());
  }
  // Close connection.
  sqlite3_close (db);
}


void stylesheet_set_recently_used (const ustring& stylesheet, vector<ustring>& styles, vector<unsigned int>& counts)
{
  // Some variables.
  sqlite3 *db;
  int rc;
  char *error = NULL;
  try
  {
    // Connect to the database.
    rc = sqlite3_open(stylesheet_filename (stylesheet).c_str (), &db);
    if (rc) throw runtime_error (sqlite3_errmsg(db));
    sqlite3_busy_timeout (db, 1000);
    // Variable.
    char * sql;
    // Delete all existing markers.
    sql = g_strdup_printf ("delete from recent;");
    rc = sqlite3_exec (db, sql, NULL, NULL, &error);
    if (rc) {
      throw runtime_error (sqlite3_errmsg(db));
    }
    // Insert all new markers.
    for (unsigned int i = 0; i < styles.size(); i++) {
      sql = g_strdup_printf("insert into recent values ('%s', %d);", styles[i].c_str(), counts[i]);
      rc = sqlite3_exec (db, sql, NULL, NULL, &error);
      if (rc) {
        throw runtime_error (sqlite3_errmsg(db));
      }
    }
    // Free memory.
    g_free (sql);
  }
  catch (exception & ex)
  {
    gw_critical (ex.what ());
  }
  // Close connection.
  sqlite3_close (db);
}


bool stylesheet_style_has_endmarker (const Style& style)
{
  bool endmarker = false;
  if (style.type == stInlineText)
    endmarker = true;
  if (style.type == stFootEndNote) {
    if (style.subtype == fentFootnote)
      endmarker = true;
    if (style.subtype == fentEndnote)
      endmarker = true;
    if (style.subtype == fentContentWithEndmarker)
      endmarker = true;
  }
  if (style.type == stCrossreference) {
    if (style.subtype == ctCrossreference)
      endmarker = true;
    if (style.subtype == ctContentWithEndmarker)
      endmarker = true;
  }
  if (style.type == stPicture)
    endmarker = true;
  return endmarker;
}


void stylesheet_save_style (const ustring& stylesheet, const Style& style)
{
  // Some variables.  
  sqlite3 *db;
  int rc;
  char *error = NULL;
  try
  {
    // Connect to the database.
    rc = sqlite3_open(stylesheet_filename (stylesheet).c_str (), &db);
    if (rc) throw runtime_error (sqlite3_errmsg(db));
    sqlite3_busy_timeout (db, 1000);
    // Variable.
    char * sql;
    // Delete possible existing style.
    sql = g_strdup_printf ("delete from styles where marker = '%s';", style.marker.c_str());
    rc = sqlite3_exec (db, sql, NULL, NULL, &error);
    if (rc) {
      throw runtime_error (sqlite3_errmsg(db));
    }
    // Save the style.
    sql = g_strdup_printf("insert into styles values ('%s', '%s', '%s', %f, %i, '%s', '%s', '%s', '%s', %i, '%s', %f, %f, %f, %f, %f, %i, %i, %i, %i, %i, %i, %i, %i, %i, '%s', '%s', '%s', %d, %d);",
      style.marker.c_str(), double_apostrophy (style.name).c_str(), double_apostrophy (style.info).c_str(),
      style.fontsize, style.fontpercentage,
      style.italic.c_str(), style.bold.c_str(), style.underline.c_str(), style.smallcaps.c_str(), style.superscript, style.justification.c_str(),
      style.spacebefore, style.spaceafter, style.leftmargin, style.rightmargin,
      style.firstlineindent, style.spancolumns, 
      style.type, style.subtype, 
      style.userbool1, style.userbool2, style.userbool3,
      style.userint1, style.userint2, style.userint3,
      style.userstring1.c_str(), style.userstring2.c_str(), style.userstring3.c_str(),
      style.color, style.print);
    rc = sqlite3_exec (db, sql, NULL, NULL, &error);
    if (rc) {
      throw runtime_error (sqlite3_errmsg(db));
    }
    // Free memory.
    g_free (sql);
  }
  catch (exception & ex)
  {
    gw_critical (ex.what ());
  }
  // Close connection.
  sqlite3_close (db);
}


void stylesheet_load_style (const ustring& stylesheet, Style& style)
{
  // Some variables.  
  sqlite3 *db;
  int rc;
  char *error = NULL;
  try
  {
    // If the stylesheet is not given, we read from the template.
    ustring filename = stylesheet_filename (stylesheet);
    if (stylesheet.empty()) filename = stylesheet_template_filename ();
    // Connect to the database.
    rc = sqlite3_open(filename.c_str (), &db);
    if (rc) throw runtime_error (sqlite3_errmsg(db));
    sqlite3_busy_timeout (db, 1000);
    // Read the style.
    SqliteReader reader (0);
    char * sql;
    sql = g_strdup_printf ("select name, info, fontsize, fontpercentage, "
      "italic, bold, underline, smallcaps, superscript, justification, "
      "spacebefore, spaceafter, leftmargin, rightmargin, firstlineindent, spancolumns, "
      "type, subtype, userbool1, userbool2, userbool3, "
      "userint1, userint2, userint3, userstring1, userstring2, userstring3, "
      "color, print "
      "from styles where marker = '%s';", style.marker.c_str());
    rc = sqlite3_exec(db, sql, reader.callback, &reader, &error);
    g_free (sql);
    if (rc != SQLITE_OK) {
      throw runtime_error (sqlite3_errmsg(db));
    }
    if (reader.ustring0.size() > 0) {
      style.name = reader.ustring0[0];
      style.info = reader.ustring1[0];
      style.fontsize = convert_to_double (reader.ustring2[0]);
      style.fontpercentage = convert_to_int (reader.ustring3[0]);
      style.italic = reader.ustring4[0];
      style.bold = reader.ustring5[0];
      style.underline = reader.ustring6[0];
      style.smallcaps = reader.ustring7[0];
      style.superscript = convert_to_bool (reader.ustring8[0]);
      style.justification = reader.ustring9[0];
      style.spacebefore = convert_to_double (reader.ustring10[0]);
      style.spaceafter = convert_to_double (reader.ustring11[0]);
      style.leftmargin = convert_to_double (reader.ustring12[0]);
      style.rightmargin = convert_to_double (reader.ustring13[0]);
      style.firstlineindent = convert_to_double (reader.ustring14[0]);
      style.spancolumns = convert_to_bool (reader.ustring15[0]);
      style.type = (StyleType) convert_to_int (reader.ustring16[0]);
      style.subtype = convert_to_int (reader.ustring17[0]);
      style.userbool1 = convert_to_bool (reader.ustring18[0]);
      style.userbool2 = convert_to_bool (reader.ustring19[0]);
      style.userbool3 = convert_to_bool (reader.ustring20[0]);
      style.userint1 = convert_to_int (reader.ustring21[0]);
      style.userint2 = convert_to_int (reader.ustring22[0]);
      style.userint3 = convert_to_int (reader.ustring23[0]);
      style.userstring1 = reader.ustring24[0];
      style.userstring2 = reader.ustring25[0];
      style.userstring3 = reader.ustring26[0];
      style.color = convert_to_int (reader.ustring27[0]);
      style.print = convert_to_int (reader.ustring28[0]);
    }
  }
  catch (exception & ex)
  {
    gw_critical (ex.what ());
  }
  // Close connection.
  sqlite3_close (db);
}


void stylesheet_vacuum (const ustring& stylesheet, unsigned int starttime)
{
  vacuum_database (stylesheet_filename (stylesheet), starttime);
}


set <ustring> stylesheet_get_styles_of_type (StylesheetType stylesheettype)
{
  set<ustring> markers;
  switch (stylesheettype) {
    case stBasicParagraphWordNote:
    {
      // Store the markes that are in, and fall through to the next type.
      for (unsigned int i = 0; i < (sizeof (stylesheet_word_note) / sizeof (*stylesheet_word_note)); i++) {
        markers.insert (stylesheet_word_note[i]);
      }
    }
    case stBasicParagraph:
    {
      // Store the markes that are in, and fall through to the next type.
      for (unsigned int i = 0; i < (sizeof (stylesheet_paragraph) / sizeof (*stylesheet_paragraph)); i++) {
        markers.insert (stylesheet_paragraph[i]);
      }
    }
    case stBasic:
    {
      // Store the markers that are in.
      for (unsigned int i = 0; i < (sizeof (stylesheet_basic) / sizeof (*stylesheet_basic)); i++) {
        markers.insert (stylesheet_basic[i]);
      }
      break;
    }
    case stSilBestPractice:
    {
      // Store the markers that are in.
      for (unsigned int i = 0; i < (sizeof (stylesheet_sil_best_practice) / sizeof (*stylesheet_sil_best_practice)); i++) {
        markers.insert (stylesheet_sil_best_practice[i]);
      }
      break;
    }
    case stFull:
    {
      // Full stylesheet: take all styles from the stylesheet template.
      sqlite3 *db;
      int rc;
      char *error = NULL;
      try
      {
        rc = sqlite3_open(stylesheet_template_filename ().c_str (), &db);
        if (rc) throw runtime_error (sqlite3_errmsg(db));
        sqlite3_busy_timeout (db, 1000);
        SqliteReader reader (0);
        char * sql;
        sql = g_strdup_printf ("select marker from styles;");
        rc = sqlite3_exec(db, sql, reader.callback, &reader, &error);
        g_free (sql);
        if (rc != SQLITE_OK) {
          throw runtime_error (sqlite3_errmsg(db));
        }
        for (unsigned int i = 0; i < reader.ustring0.size(); i++) {
          markers.insert (reader.ustring0[i]);
        }
      }
      catch (exception & ex)
      {
        gw_critical (ex.what ());
      }
      sqlite3_close (db);
      break;
    }
  }
  return markers;  
}
