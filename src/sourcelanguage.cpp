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


#include "sourcelanguage.h"
#include "directories.h"
#include "gwrappers.h"
#include "utilities.h"
#include "dialoglistview.h"
#include "settings.h"
#include "gtkwrappers.h"
#include "sqlite_reader.h"
#include "tiny_utilities.h"
#include "progresswindow.h"
#include "morphology.h"


ustring source_language_database_file_name (const ustring& name)
// Give a "name" for a source language, it gives the filename of its database.
{
  return gw_build_filename(directories_get_databases(), "source_language_" + name + ".sql");
}


void source_language_database_create (const ustring& name)
// Creates an empty database for a source language given by "name".
{
  ustring filename = source_language_database_file_name (name);
  unlink (filename.c_str());
  sqlite3 *db;
  sqlite3_open(filename.c_str(), &db);
  sqlite3_exec(db, "create table text (book integer, chapter integer, verse integer, text text);", NULL, NULL, NULL);
  sqlite3_exec(db, "create table lemmata (book integer, chapter integer, verse integer, item integer, value text);", NULL, NULL, NULL);
  sqlite3_exec(db, "create table morphology (book integer, chapter integer, verse integer, item integer, value text);", NULL, NULL, NULL);
  sqlite3_close(db);
}


vector <ustring> source_language_get_names ()
// Gets a list of available source languages.
{
  ReadFiles rf(directories_get_databases(), "source_language_", ".sql");
  vector <ustring> names = rf.files;
  sort(names.begin(), names.end());
  for (unsigned int i = 0; i < names.size(); i++) {
    // Remove the prefix.
    names[i].erase (0, 16);
    // Remove the suffix.
    names[i].erase (names[i].length() - 4, 4);
  }
  return names;
}


void source_language_add_to_display ()
// Add a source language to the ones being displayed.
{
  extern Settings * settings;
  vector <ustring> names;
  {
    vector <ustring> current_names = settings->genconfig.source_language_names_get ();
    set <ustring> current_names_set (current_names.begin(), current_names.end());
    vector <ustring> available_names = source_language_get_names ();
    for (unsigned int i = 0; i < available_names.size(); i++) {
      if (current_names_set.find (available_names[i]) == current_names_set.end()) {
        names.push_back (available_names[i]);
      }
    }
  }
  if (names.empty()) {
    gtkw_dialog_info (NULL, "No more source languages available");
    return;
  }
  ListviewDialog dialog ("Select a source language to add", names, "", false, NULL);
  if (dialog.run() == GTK_RESPONSE_OK) {
    vector <ustring> names = settings->genconfig.source_language_names_get ();
    names.push_back (dialog.focus);
    settings->genconfig.source_language_names_set (names);
  }
}


void source_language_remove_from_display ()
// Remove a source language from the ones being displayed.
{
  extern Settings * settings;
  vector <ustring> names = settings->genconfig.source_language_names_get ();
  if (names.empty()) {
    gtkw_dialog_info (NULL, "No source languages displaying");
    return;
  }
  ListviewDialog dialog ("Select a source language to remove", names, "", false, NULL);
  if (dialog.run() == GTK_RESPONSE_OK) {
    vector <ustring> newnames;
    for (unsigned int i = 0; i < names.size(); i++) {
      if (names[i] != dialog.focus) {
        newnames.push_back (names[i]);
      }
      settings->genconfig.source_language_names_set (newnames);
    }
  }
}


void source_language_get_lemmata_and_morphology (const ustring& name, const Reference& reference, vector <ustring>& words,
                                                 vector <unsigned int>& lemmata_positions, vector <ustring>& lemmata_values,
                                                 vector <unsigned int>& morphology_positions, vector <ustring>& morphology_values)
// Based on a "reference", it provides lemmata and morphology for a verse.
{
  ustring filename = source_language_database_file_name(name);
  if (!g_file_test (filename.c_str(), G_FILE_TEST_IS_REGULAR)) {
    return;
  }
  sqlite3 *db;
  int rc;
  char *error = NULL;
  try {

    // Open the database.
    rc = sqlite3_open(filename.c_str(), &db);
    if (rc)
      throw runtime_error(sqlite3_errmsg(db));
    sqlite3_busy_timeout(db, 1000);

    // Retrieve the text and sort it out.
    {
      SqliteReader reader(0);
      char *sql;
      sql = g_strdup_printf("select text from text where book = %d and chapter = %d and verse = %d;", reference.book, reference.chapter, convert_to_int (reference.verse));
      rc = sqlite3_exec(db, sql, reader.callback, &reader, &error);
      g_free(sql);
      if (rc) {
        throw runtime_error(sqlite3_errmsg(db));
      }
      if (!reader.ustring0.empty()) {
        Parse parse (reader.ustring0[0], false);
        words = parse.words;
      }
    }

    // Retrieve the Strong's lemmata.
    {
      SqliteReader reader(0);
      char *sql;
      sql = g_strdup_printf("select item, value from lemmata where book = %d and chapter = %d and verse = %d order by item asc;", reference.book, reference.chapter, convert_to_int (reference.verse));
      rc = sqlite3_exec(db, sql, reader.callback, &reader, &error);
      g_free(sql);
      if (rc) {
        throw runtime_error(sqlite3_errmsg(db));
      }
      for (unsigned int i = 0; i < reader.ustring0.size(); i++) {
        lemmata_positions.push_back (convert_to_int (reader.ustring0[i]));
        lemmata_values.push_back (reader.ustring1[i]);
      }
    }

    // Retrieve the morphology.
    {
      SqliteReader reader(0);
      char *sql;
      sql = g_strdup_printf("select item, value from morphology where book = %d and chapter = %d and verse = %d order by item asc;", reference.book, reference.chapter, convert_to_int (reference.verse));
      rc = sqlite3_exec(db, sql, reader.callback, &reader, &error);
      g_free(sql);
      if (rc) {
        throw runtime_error(sqlite3_errmsg(db));
      }
      for (unsigned int i = 0; i < reader.ustring0.size(); i++) {
        morphology_positions.push_back (convert_to_int (reader.ustring0[i]));
        morphology_values.push_back (reader.ustring1[i]);
      }
    }

  }
  catch(exception & ex) {
    gw_critical(ex.what());
  }
  sqlite3_close(db);
}


void source_language_test_lemmata_and_morphology () // Todo
{
  vector <ustring> names = source_language_get_names ();
  for (unsigned int i = 0; i < names.size(); i++) {
    ustring name = names[i];
    sqlite3 *db;
    sqlite3_open(source_language_database_file_name(name).c_str(), &db);
    sqlite3_busy_timeout(db, 1000);
    {
      /* Todo
      ustring title = name + " - lemmata";
      ProgressWindow progresswindow (title, false);
      SqliteReader reader(0);
      sqlite3_exec(db, "select distinct value from lemmata;", reader.callback, &reader, NULL);
      progresswindow.set_iterate (0, 1, reader.ustring0.size());
      vector <ustring> output;
      for (unsigned int i = 0; i < reader.ustring0.size(); i++) {
        progresswindow.iterate();
        output.push_back ("Lemma: " + reader.ustring0[i]);
      }
      ustring outputfile = gw_build_filename (g_get_home_dir(), title);
      write_lines (outputfile, output);
      */
    }
    {
      ustring title = name + " - morphology";
      ProgressWindow progresswindow (title, false);
      SqliteReader reader(0);
      sqlite3_exec(db, "select distinct value from morphology;", reader.callback, &reader, NULL);
      progresswindow.set_iterate (0, 1, reader.ustring0.size());
      vector <ustring> output;
      for (unsigned int i = 0; i < reader.ustring0.size(); i++) {
        progresswindow.iterate();
        ustring parsing = reader.ustring0[i];
        output.push_back ("Morphology: " + parsing);
        ustring definition;
        if (morphology_define_parsing (parsing, definition)) {
          output.push_back ("Definition: " + definition);
        } else {
          output.push_back ("ERROR: No definition found");
        }

      }
      ustring outputfile = gw_build_filename (g_get_home_dir(), title);
      write_lines (outputfile, output);
    }
    sqlite3_close(db);
  }
}


