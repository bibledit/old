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
#include <glib.h>
#include "notes_utils.h"
#include "directories.h"
#include <sqlite3.h>
#include "sqlite_reader.h"
#include "bible.h"
#include "date_time_utils.h"
#include "gwrappers.h"
#include "shell.h"
#include "progresswindow.h"
#include "settings.h"
#include "projectutils.h"
#include "books.h"
#include "settings.h"
#include "versification.h"
#include "unixwrappers.h"
#include "tiny_utilities.h"
#include "maintenance.h"


void notes_database_verify(const ustring& alternate_directory)
/*
 This verifies that the notes database is fine.
 If it's not there, it creates one.
 At times it upgrades older databases.
 */
{
  // Filename of database.
  ustring oldfilename;
  ustring filename = notes_database_filename(alternate_directory);

  // If the newest database is already there, bail out.
  if (g_file_test(filename.c_str(), G_FILE_TEST_IS_REGULAR))
    return;

  // Last thing: make new database if there is no db.
  filename = notes_database_filename();
  if (!g_file_test(filename.c_str(), G_FILE_TEST_IS_REGULAR)) {
    sqlite3 *db;
    int rc;
    char *error = NULL;
    try {
      // Connect to the database.
      rc = sqlite3_open(filename.c_str(), &db);
      if (rc)
        throw runtime_error(sqlite3_errmsg(db));
      sqlite3_busy_timeout(db, 1000);
      // Create the notes table.
      char *sql;
      sql = g_strdup_printf("create table '%s' (id integer, reference text, ref_osis text, project text, category text, note text, casefolded text, created integer, modified integer, user text, logbook text);", TABLE_NOTES);
      rc = sqlite3_exec(db, sql, NULL, NULL, &error);
      g_free(sql);
    }
    catch(exception & ex) {
      gw_critical(ex.what());
    }
    // Close connection.  
    sqlite3_close(db);
  }
}


const gchar * notes_database_basename ()
{
  return "notes.sql2";
}


ustring notes_database_filename(const ustring& alternate_directory)
// Returns the filename of the notes database.
{ 
  ustring filename;
  filename = gw_build_filename(directories_get_notes(), notes_database_basename ());
  if (!alternate_directory.empty()) {
    filename = gw_build_filename(alternate_directory, notes_database_basename ());
  }
  return filename;
}


gint notes_database_get_unique_id()
// This generates a unique id, one that is not yet used in the notes database.
{
  gint32 result = 0;
  sqlite3 *db;
  int rc;
  char *error = NULL;
  try {
    rc = sqlite3_open(notes_database_filename().c_str(), &db);
    if (rc)
      throw runtime_error(sqlite3_errmsg(db));
    sqlite3_busy_timeout(db, 1000);
    bool found = false;
    while (!found) {
      result = g_random_int_range(1, 100000000);
      SqliteReader reader(0);
      char *sql;
      sql = g_strdup_printf("select count(*) from '%s' where id=%i;", TABLE_NOTES, result);
      rc = sqlite3_exec(db, sql, reader.callback, &reader, &error);
      g_free(sql);
      if (rc != SQLITE_OK) {
        throw runtime_error(error);
      }
      gint count = convert_to_int(reader.ustring0[0]);
      if (count == 0) {
        found = true;
      }
    }
  }
  catch(exception & ex) {
    gw_critical(ex.what());
  }
  sqlite3_close(db);
  return result;
}


void notes_delete_one(int id)
// Deletes the note with id.
{
  sqlite3 *db;
  int rc;
  char *error = NULL;
  try {
    // Connect to database and start transaction
    rc = sqlite3_open(notes_database_filename().c_str(), &db);
    if (rc)
      throw runtime_error(sqlite3_errmsg(db));
    sqlite3_busy_timeout(db, 1000);
    rc = sqlite3_exec(db, "begin;", NULL, NULL, &error);
    // Delete data with "id".
    char *sql;
    sql = g_strdup_printf("delete from %s where id = %d;", TABLE_NOTES, id);
    rc = sqlite3_exec(db, sql, NULL, NULL, &error);
    g_free(sql);
    // Commit the transaction and close connection.
    rc = sqlite3_exec(db, "commit;", NULL, NULL, &error);
  }
  catch(exception & ex) {
    gw_critical(ex.what());
  }
  sqlite3_close(db);
}


void notes_sort(vector < unsigned int >&ids, const vector < ustring > &refs, const vector < ustring > &allrefs, const vector < int >&dates)
/*
 This sorts notes.
 ids         - ID's of the notes.
 The ids will be modified and sorted, so that these can be used 
 to fetch data from the database in the right order.
 refs        - References to sort on first.
 allrefs     - All the references to sort on next.
 dates       - Modification dates to sort on third.
 */
{
  // No sorting when less than two entries.
  if (ids.size() < 2)
    return;

  // The sorting is done by putting everything in one string, and then
  // sorting the vector of strings, and then taking out the ids again.

  // Storage for strings to sort
  vector < ustring > strings_to_sort;

  // Go through all data to sort.
  for (unsigned int i = 0; i < ids.size(); i++) {

    // Storage for string to build.
    ustring string_to_build;

    // First part of the string is the reference.
    // It does not always has the same length, so make it 9.
    string_to_build = "0000" + refs[i];
    string_to_build = string_to_build.substr(string_to_build.length() - 9, 9);

    // Next sort on all references.
    string_to_build.append(allrefs[i]);

    // Third part to sort on is the date. This Julian day has a length of 6
    // characters at the time of programming, and will continue so until
    // Deo Volente the year 2739 A.D. Therefore there is no need to modify the
    // length of it.
    {
      ustring date;
      date = convert_to_string(dates[i]);
      string_to_build.append(date);
    }

    // Store built string
    strings_to_sort.push_back(string_to_build);
  }

  // Sort the data.
  quick_sort(strings_to_sort, ids, 0, strings_to_sort.size());
}


void notes_select(vector < unsigned int >&ids, unsigned int &id_cursor, const ustring & currentreference)
/*
 This selects notes for display.
 It does this based on the current reference.
 The resulting selection will be stored in ids.
 It gives "id_cursor" which contains the id the cursor has to be put at.
 */
{
  // Configuration.
  extern Settings *settings;
  // Clear ids.
  ids.clear();
  // The average numerical equivalent of current reference.
  int numerical_currentreference = 0;
  {
    ustring book, chapter, verse;
    decode_reference(currentreference, book, chapter, verse);
    vector <int>verses = verses_encode(verse);
    for (unsigned int i = 0; i < verses.size(); i++) {
      numerical_currentreference += verses[i];
    }
    if (!verses.empty())
      numerical_currentreference /= verses.size();
    numerical_currentreference += reference_to_numerical_equivalent(book, chapter, "0");
  }
  // Date selection.
  int currentdate = date_time_julian_day_get_current();
  // Category selection.
  ustring category = settings->genconfig.notes_selection_category_get();
  // Cursor focus calculation.
  int minimum_cursor_distance = G_MAXINT;
  // Database variables.
  sqlite3 *db;
  int rc;
  char *error = NULL;
  try {
    // Connect to database.
    rc = sqlite3_open(notes_database_filename().c_str(), &db);
    if (rc)
      throw runtime_error(sqlite3_errmsg(db));
    sqlite3_busy_timeout(db, 1000);
    SqliteReader sqlitereader(0);
    // See which notes to select.
    switch ((NotesSelectionReferenceType) settings->genconfig.notes_selection_reference_get()) {
    case nsrtCurrentVerse:
      {
        // This selects any notes which refer to the current verse.
        ustring book, chapter, verse;
        decode_reference(currentreference, book, chapter, verse);
        unsigned int verse_zero;
        verse_zero = reference_to_numerical_equivalent(book, chapter, "0");
        vector < int >verses = verses_encode(verse);
        for (unsigned int i = 0; i < verses.size(); i++) {
          ustring this_verse = convert_to_string(int (verse_zero + verses[i]));
          char *sql;
          sql = g_strdup_printf("select id, reference, modified, project, category from '%s' where reference glob ('* %s *');", TABLE_NOTES, this_verse.c_str());
          rc = sqlite3_exec(db, sql, sqlitereader.callback, &sqlitereader, &error);
          g_free(sql);
          if (rc != SQLITE_OK) {
            throw runtime_error(error);
          }
        }
        break;
      }
    case nsrtCurrentChapter:
      {
        // This selects any notes which refer to the current chapter.
        ustring book, chapter, verse;
        decode_reference(currentreference, book, chapter, verse);
        unsigned int verse_zero;
        verse_zero = reference_to_numerical_equivalent(book, chapter, "0");
        ustring this_chapter = convert_to_string(verse_zero);
        this_chapter.erase(this_chapter.length() - 3, 3);
        char *sql;
        sql = g_strdup_printf("select id, reference, modified, project, category from '%s' where reference glob ('* %s??? *');", TABLE_NOTES, this_chapter.c_str());
        rc = sqlite3_exec(db, sql, sqlitereader.callback, &sqlitereader, &error);
        g_free(sql);
        if (rc != SQLITE_OK) {
          throw runtime_error(error);
        }
        break;
      }
    case nsrtCurrentBook:
      {
        // This selects any notes which refer to the current book.
        ustring book, chapter, verse;
        decode_reference(currentreference, book, chapter, verse);
        unsigned int verse_zero;
        verse_zero = reference_to_numerical_equivalent(book, chapter, "0");
        ustring this_book = convert_to_string(verse_zero);
        this_book.erase(this_book.length() - 6);
        char *sql;
        sql = g_strdup_printf("select id, reference, modified, project, category from '%s' where reference glob ('* %s?????? *');", TABLE_NOTES, this_book.c_str());
        rc = sqlite3_exec(db, sql, sqlitereader.callback, &sqlitereader, &error);
        g_free(sql);
        if (rc != SQLITE_OK) {
          throw runtime_error(error);
        }
        break;
      }
    case nsrtAny:
      {
        char *sql;
        sql = g_strdup_printf("select id, reference, modified, project, category from '%s';", TABLE_NOTES);
        rc = sqlite3_exec(db, sql, sqlitereader.callback, &sqlitereader, &error);
        g_free(sql);
        if (rc != SQLITE_OK) {
          throw runtime_error(error);
        }
        break;
      }
    }
    // Storage for sorting purposes.
    vector <ustring> references;
    vector <int> distances;
    set <gint32> already_stored_ids;
    // Read all resulting data from the database. Make further selections.
    for (unsigned int rc = 0; rc < sqlitereader.ustring0.size(); rc++) {
      // Selection based on the date.
      int modified_date = convert_to_int(sqlitereader.ustring2[rc]);
      switch ((NotesSelectionEditedType) settings->genconfig.notes_selection_edited_get()) {
      case nsetToday:
        {
          if (modified_date != currentdate)
            continue;
          break;
        }
      case nsetDateRange:
        {
          if (modified_date < settings->genconfig.notes_selection_date_from_get())
            continue;
          if (modified_date > settings->genconfig.notes_selection_date_to_get())
            continue;
          break;
        }
      case nsetAny:
        {
          break;
        }
      }
      // Selection based on the category of the note.
      if (!category.empty()) {
        if (category != sqlitereader.ustring4[rc])
          continue;
      }
      // Selection based on project.
      if (settings->genconfig.notes_selection_current_project_get()) {
        bool project_ok = false;
        ustring project_in_db = sqlitereader.ustring3[rc];
        if (project_in_db == settings->genconfig.project_get())
          project_ok = true;
        // Current notes can use "All".
        if (project_in_db == "All")
          project_ok = true;
        // Only show notes when the project is fine.
        if (!project_ok)
          continue;
      }
      // Get note id.
      gint32 id = convert_to_int(sqlitereader.ustring0[rc]);
      // Sorting on the references that the note refers to.
      unsigned int average_note_reference = 0;
      {
        Parse parse(sqlitereader.ustring1[rc]);
        if (!parse.words.empty()) {
          for (unsigned int i = 0; i < parse.words.size(); i++) {
            average_note_reference += convert_to_int (parse.words[i]);
          }
          average_note_reference /= parse.words.size();
        }
      }
      // Calculate the distance between note and reference.
      // Also see if the cursor must be positioned here.
      int distance = average_note_reference - numerical_currentreference;
      int absolute_distance = ABS (distance);
      if (absolute_distance < minimum_cursor_distance) {
        minimum_cursor_distance = absolute_distance;
        id_cursor = id;
      }
      // Store data. 
      // As we now work with half-verses (10a, 10b), because of the way we select 
      // notes we might have repeating ids. Filter these out.
      if (already_stored_ids.find(id) == already_stored_ids.end()) {
        ids.push_back(id);
        already_stored_ids.insert(id);
        distances.push_back(distance);
      }
    }
    // Sort the notes, based on the distances of the notes to the active reference.
    quick_sort(distances, ids, 0, distances.size());
  }
  catch(exception & ex) {
    gw_critical(ex.what());
  }
  // Close database
  sqlite3_close(db);
}


void notes_display(ustring & note_buffer, vector < unsigned int >ids, unsigned int cursor_id, unsigned int &cursor_offset, bool & stop)
/*
 This collect html data for displaying the notes.
 It collects data for all the notes that have an id given in ids.
 It inserts a html anchor at the start of the note whose id is "cursor_id".
 */
{
  extern Settings *settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(settings->genconfig.project_get());
  ustring language = projectconfig->language_get();
  sqlite3 *db;
  int rc;
  char *error = NULL;
  try {
    // Connect to database.
    rc = sqlite3_open(notes_database_filename().c_str(), &db);
    if (rc)
      throw runtime_error(sqlite3_errmsg(db));
    sqlite3_busy_timeout(db, 1000);

    // Whether to show the text of the reference(s).
    bool show_reference_text = settings->genconfig.notes_display_reference_text_get();

    // Whether to show the summary only.
    bool show_summary = settings->genconfig.notes_display_summary_get();

    // Go through all the notes.
    for (unsigned int c = 0; c < ids.size(); c++) {

      // Handle possible stop command.
      if (stop)
        continue;

      // Get from the database.
      SqliteReader reader(0);
      char *sql;
      sql = g_strdup_printf("select id, ref_osis, note, project, category, created, user from '%s' where id = %d;", TABLE_NOTES, ids[c]);
      rc = sqlite3_exec(db, sql, reader.callback, &reader, &error);
      g_free(sql);
      if (rc != SQLITE_OK) {
        throw runtime_error(error);
      }
      // Go through the results (there should be only one anyway).
      for (unsigned int r = 0; r < reader.ustring0.size(); r++) {

        // Get the reference(s).
        ustring reference = reader.ustring1[r];

        // Parse the string into its possible several references.
        Parse parse(reference, false);
        reference.clear();

        // Keep list of references.
        vector < Reference > references;

        // Go through each reference.
        for (unsigned int i2 = 0; i2 < parse.words.size(); i2++) {
          // Make it human readable.
          Reference ref(0);
          reference_discover(0, 0, "", parse.words[i2], ref.book, ref.chapter, ref.verse);
          if (!reference.empty())
            reference.append(", ");
          reference.append(ref.human_readable(language));
          references.push_back(ref);
        }

        // Get project.
        ustring project = reader.ustring3[r];

        // Start creating the heading with links.
        ustring linkheading;
        // If this note is to be focused, then insert a special anchor for that: <a name="cursoranchor"></a>
        if (ids[c] == cursor_id) {
          linkheading.append("<a name=\"");
          linkheading.append(notes_cursor_anchor());
          linkheading.append("\"></a>");
        }
        if (settings->session.project_notes_show_title) {
          // Insert a link with this heading, e.g.: <a href="10">Genesis 1.1</a>
          linkheading.append("<a href=\"" + convert_to_string(ids[c]) + "\">");
          linkheading.append(reference);
          if (settings->genconfig.notes_display_project_get())
            linkheading.append(" " + project);
          if (settings->genconfig.notes_display_category_get())
            linkheading.append(" " + reader.ustring4[r]);
          if (settings->genconfig.notes_display_date_created_get())
            linkheading.append(" " + date_time_julian_human_readable(convert_to_int(reader.ustring5[r]), true));
          if (settings->genconfig.notes_display_created_by_get())
            linkheading.append(" " + reader.ustring6[r]);
          linkheading.append("</a>");
          // Append a [delete] link too, e.g.: <a href="d10">[delete]</a>
          linkheading.append(" <a href=\"d" + convert_to_string(ids[c]) + "\">");
          linkheading.append("[delete]");
          linkheading.append("</a>");
          // Append a [references] link too, e.g.: <a href="r10">[references]</a>
          linkheading.append(" <a href=\"r" + convert_to_string(ids[c]) + "\">");
          linkheading.append("[references]");
          linkheading.append("</a>");
        }
        // Add the heading to the note data.
        note_buffer.append(linkheading);

        // Handle summary. Show only the first few words.
        if (show_summary) {
          ustring summary = reader.ustring2[r];
          replace_text(summary, "\n", " ");
          replace_text(summary, "<BR>", " ");
          Parse parse(summary, false);
          unsigned int maximum = 5;
          maximum = CLAMP(maximum, 0, parse.words.size());
          summary.clear();
          for (unsigned int w = 0; w < maximum; w++) {
            summary.append(" ");
            summary.append(parse.words[w]);
          }
          if (!summary.empty())
            summary.append(" ...");
          note_buffer.append(summary);
        }
        // Append a new line.
        note_buffer.append("<BR>\n");

        // Insert text of the references, if requested.
        if (show_reference_text) {
          for (unsigned int r = 0; r < references.size(); r++) {
            vector < unsigned int >simple_verses = verse_range_sequence(references[r].verse);
            for (unsigned int sv = 0; sv < simple_verses.size(); sv++) {
              Reference ref(references[r]);
              ref.verse = convert_to_string(simple_verses[sv]);
              note_buffer.append(ref.human_readable(language));
              note_buffer.append(" ");
              ustring text = project_retrieve_verse(project, ref.book, ref.chapter, ref.verse);
              if (!text.empty()) {
                CategorizeLine cl(text);
                text = cl.verse;
              }
              note_buffer.append(text);
              note_buffer.append("<BR>\n");
            }
          }
        }
        // Get the text of the note.
        if (!show_summary) {
          ustring note = reader.ustring2[r];
          notes_update_old_one(note);
          note_buffer.append(note);
          note_buffer.append("<BR>\n");
        }

      }
    }
  }
  catch(exception & ex) {
    gw_critical(ex.what());
  }
  // Close connection.  
  sqlite3_close(db);
}


void notes_get_references_from_editor(GtkTextBuffer * textbuffer, vector < Reference > &references, vector < ustring > &messages)
/*
 Gets all references from the notes editor.
 Normalizes them.
 Produces messages on trouble.
 Handles notes that span more than one chapter.
 */
{
  // Get all lines from the textbuffer.
  vector < ustring > lines;
  textbuffer_get_lines(textbuffer, lines);
  // When discovering a reference from a user's entry, use previous values,
  // so that it becomes quicker for a user to enter new references.
  // If Leviticus 10:11 is already there, and the user wishes to add verse
  // 12 also, he just enters 12 on a line, and that' it.
  Reference previousreference(0);
  for (unsigned int i = 0; i < lines.size(); i++) {
    if (!lines[i].empty()) {
      // Normalize reference.
      Reference reference(0);
      if (reference_discover(previousreference.book, previousreference.chapter, previousreference.verse, lines[i], reference.book, reference.chapter, reference.verse)) {
        ustring ch1, vs1, ch2, vs2;
        if (chapter_span_discover(lines[i], ch1, vs1, ch2, vs2)) {
          // We cross the chapter boundaries. 
          // Store as two or more references, 
          // the first one going up to the end of the chapter, 
          // and the second one starting at the next chapter verse 1,
          // and any chapter in-between.
          extern Settings *settings;
          ProjectConfiguration *projectconfig = settings->projectconfig(settings->genconfig.project_get());
          Reference ref(reference.book, convert_to_int(ch1), vs1);
          ustring lastverse = versification_get_last_verse(projectconfig->versification_get(), reference.book, convert_to_int(ch1));
          ref.verse.append("-" + lastverse);
          references.push_back(ref);
          for (unsigned int ch = convert_to_int(ch1) + 1; ch < convert_to_int(ch2); ch++) {
            Reference ref(reference.book, ch, "1");
            ustring lastverse = versification_get_last_verse(projectconfig->versification_get(), reference.book, ch);
            ref.verse.append("-" + lastverse);
            references.push_back(ref);
          }
          ref.chapter = convert_to_int(ch2);
          ref.verse = "1-" + vs2;
          references.push_back(ref);
          // Store values to discover next reference.
          previousreference.book = reference.book;
          previousreference.chapter = convert_to_int(ch2);
          previousreference.verse = vs2;
        } else {
          // We've a normal reference.
          // Store reference.
          references.push_back(reference);
          // Store values to discover next reference.
          previousreference.book = reference.book;
          previousreference.chapter = reference.chapter;
          previousreference.verse = reference.verse;
        }
      } else {
        messages.push_back("Reference " + lines[i] + " is not valid and has been removed.");
      }
    }
  }
}


ustring notes_categories_filename()
// Returns the filename of the notes database.
{
  return gw_build_filename(directories_get_notes(), "categories");
}


void notes_categories_check()
// Check categories are there - if not, create default set.
{
  if (!g_file_test(notes_categories_filename().c_str(), G_FILE_TEST_IS_REGULAR)) {
    vector < ustring > categories;
    categories.push_back("No issue");
    categories.push_back("For myself");
    categories.push_back("For subteam");
    categories.push_back("For team");
    categories.push_back("For scholar");
    categories.push_back("For panel");
    categories.push_back("For church");
    categories.push_back("For consultant");
    categories.push_back("For Bible society");
    write_lines(notes_categories_filename(), categories);
  }
}


void notes_categories_add_from_database(vector < ustring > &categories)
// Takes the existing notes categories, if there are any, and adds any
// extra categories found in the database.
{
  sqlite3 *db;
  int rc;
  char *error = NULL;
  try {
    // Get the unique categories.
    set < ustring > database_categories;
    rc = sqlite3_open(notes_database_filename().c_str(), &db);
    if (rc)
      throw runtime_error(sqlite3_errmsg(db));
    sqlite3_busy_timeout(db, 1000);
    SqliteReader reader(0);
    char *sql;
    sql = g_strdup_printf("select category from '%s';", TABLE_NOTES);
    rc = sqlite3_exec(db, sql, reader.callback, &reader, &error);
    g_free(sql);
    if (rc != SQLITE_OK) {
      throw runtime_error(error);
    }
    for (unsigned int i = 0; i < reader.ustring0.size(); i++) {
      database_categories.insert(reader.ustring0[i]);
    }
    // Add any new categories to the container.
    set < ustring > localcategories(categories.begin(), categories.end());
    vector < ustring > db_categories(database_categories.begin(), database_categories.end());
    for (unsigned int i = 0; i < db_categories.size(); i++) {
      if (localcategories.find(db_categories[i]) == localcategories.end()) {
        categories.push_back(db_categories[i]);
      }
    }
  }
  catch(exception & ex) {
    gw_critical(ex.what());
  }
  sqlite3_close(db);
}


void notes_projects_add_from_database(vector < ustring > &projects)
// Takes the existing projects, if there are any, and adds any
// extra projects found in the database.
{
  sqlite3 *db;
  int rc;
  char *error = NULL;
  try {
    // Get the unique categories.
    set < ustring > database_projects;
    rc = sqlite3_open(notes_database_filename().c_str(), &db);
    if (rc)
      throw runtime_error(sqlite3_errmsg(db));
    sqlite3_busy_timeout(db, 1000);
    SqliteReader reader(0);
    char *sql;
    sql = g_strdup_printf("select project from '%s';", TABLE_NOTES);
    rc = sqlite3_exec(db, sql, reader.callback, &reader, &error);
    g_free(sql);
    if (rc != SQLITE_OK) {
      throw runtime_error(error);
    }
    for (unsigned int i = 0; i < reader.ustring0.size(); i++) {
      database_projects.insert(reader.ustring0[i]);
    }
    // Add any new categories to the container.
    set < ustring > localprojects(projects.begin(), projects.end());
    vector < ustring > db_projects(database_projects.begin(), database_projects.end());
    for (unsigned int i = 0; i < db_projects.size(); i++) {
      if (localprojects.find(db_projects[i]) == localprojects.end()) {
        projects.push_back(db_projects[i]);
      }
    }
  }
  catch(exception & ex) {
    gw_critical(ex.what());
  }
  sqlite3_close(db);
}


void notes_vacuum()
// Vacuum the database.
{
  vacuum_database(notes_database_filename());
}


void notes_store_one(int id, ustring & note, const ustring & project, vector < Reference > &references, const ustring & category, int date_created, const ustring & user_created, ustring & logbook)
{
  sqlite3 *db;
  int rc;
  char *error = NULL;
  try {
    /*
       Validate and normalize the references.
       Bad ones are removed and a message will be given.
       If no valid references remain, stop the whole transaction and give a message.
     */
    ustring encoded_references;
    ustring osis_references;
    // Get and validate all references from the textview.
    {
      // Sort the references so they appear tidy in the editor.
      sort_references(references);
      // Encode all references.
      for (unsigned int i = 0; i < references.size(); i++) {
        // Encode the reference.
        ustring book = books_id_to_english(references[i].book);
        ustring chapter = convert_to_string(references[i].chapter);
        vector < int >verses = verses_encode(references[i].verse);
        int book_chapter = reference_to_numerical_equivalent(book, chapter, "0");
        for (unsigned int i2 = 0; i2 < verses.size(); i2++) {
          encoded_references.append(" ");
          encoded_references.append(convert_to_string(int (book_chapter + verses[i2])));
        }
        // Store the references in OSIS format too.
        ustring osis_book = books_id_to_osis(references[i].book);
        ustring osis_reference = osis_book + "." + chapter + "." + references[i].verse;
        if (!osis_references.empty())
          osis_references.append(" ");
        osis_references.append(osis_reference);
      }
      encoded_references.append(" ");
    }
    // Connect to database and start transaction.
    rc = sqlite3_open(notes_database_filename().c_str(), &db);
    if (rc)
      throw runtime_error(sqlite3_errmsg(db));
    sqlite3_busy_timeout(db, 1000);
    rc = sqlite3_exec(db, "begin;", NULL, NULL, &error);
    if (rc != SQLITE_OK) {
      throw runtime_error(error);
    }
    // Delete previous data with "id".
    gchar *sql;
    sql = g_strdup_printf("delete from %s where id = %d;", TABLE_NOTES, id);
    rc = sqlite3_exec(db, sql, NULL, NULL, &error);
    g_free(sql);
    if (rc != SQLITE_OK) {
      throw runtime_error(error);
    }
    // Put new data in the database.
    // ID (integer), we take variable "id".
    // References (text), we take variable "encoded_references".
    // Project (text), we take variable "project".
    // Status (integer) This field is not used, and could be reused.
    // Category (text), we take variable "category".
    // Note (text), we take variable "note".
    // Trim off extra newlines at the end, and ensure it always has one.
    note = trim(note);
    note.append("\n");
    // Apostrophies need to be doubled before storing them.
    note = double_apostrophy(note);
    // Casefolded (text)
    ustring casefolded = note.casefold();
    // Date created. Variabele "date_created".
    // Date modified.
    int date_modified = date_time_julian_day_get_current();
    // Username. Use: user_created
    // Logbook (text), take variable logbook.
    // Apostrophies need to be doubled before storing them.
    logbook = double_apostrophy(logbook);
    // Insert data in database.
    sql = g_strdup_printf("insert into %s values (%d, '%s', '%s', '%s', '%s', '%s', '%s', %d, %d, '%s', '%s');", TABLE_NOTES, id, encoded_references.c_str(), osis_references.c_str(), project.c_str(), category.c_str(), note.c_str(), casefolded.c_str(), date_created, date_modified, user_created.c_str(), logbook.c_str());
    rc = sqlite3_exec(db, sql, NULL, NULL, &error);
    g_free(sql);
    if (rc != SQLITE_OK) {
      throw runtime_error(error);
    }
    // Commit the transaction.
    rc = sqlite3_exec(db, "commit;", NULL, NULL, &error);
    if (rc != SQLITE_OK) {
      throw runtime_error(error);
    }
  }
  catch(exception & ex) {
    gw_critical(ex.what());
  }
  // Close connection.  
  sqlite3_close(db);
}


void notes_change_category(const ustring & from, const ustring & to)
// Changes all notes in category "from" to category "to".
{
  sqlite3 *db;
  int rc;
  char *error = NULL;
  try {
    rc = sqlite3_open(notes_database_filename().c_str(), &db);
    if (rc)
      throw runtime_error(sqlite3_errmsg(db));
    sqlite3_busy_timeout(db, 1000);
    char *sql;
    sql = g_strdup_printf("update '%s' set category = '%s' where category = '%s';", TABLE_NOTES, to.c_str(), from.c_str());
    rc = sqlite3_exec(db, sql, NULL, NULL, &error);
    g_free(sql);
    if (rc)
      throw runtime_error(error);
  }
  catch(exception & ex) {
    gw_critical(ex.what());
  }
  sqlite3_close(db);
}


void notes_change_project(const ustring & from, const ustring & to)
// Changes all notes in project "from" to project "to".
{
  sqlite3 *db;
  int rc;
  char *error = NULL;
  try {
    rc = sqlite3_open(notes_database_filename().c_str(), &db);
    if (rc)
      throw runtime_error(sqlite3_errmsg(db));
    sqlite3_busy_timeout(db, 1000);
    char *sql;
    sql = g_strdup_printf("update '%s' set project = '%s' where project = '%s';", TABLE_NOTES, to.c_str(), from.c_str());
    rc = sqlite3_exec(db, sql, NULL, NULL, &error);
    g_free(sql);
    if (rc)
      throw runtime_error(error);
  }
  catch(exception & ex) {
    gw_critical(ex.what());
  }
  sqlite3_close(db);
}


void notes_read(vector < unsigned int >ids, vector < ustring > &data)
// Reads notes.
{
  sqlite3 *db;
  int rc;
  char *error = NULL;
  try {
    data.clear();
    rc = sqlite3_open(notes_database_filename().c_str(), &db);
    if (rc)
      throw runtime_error(sqlite3_errmsg(db));
    sqlite3_busy_timeout(db, 1000);
    for (unsigned int c = 0; c < ids.size(); c++) {
      SqliteReader reader(0);
      char *sql;
      sql = g_strdup_printf("select note from '%s' where id = %d;", TABLE_NOTES, ids[c]);
      rc = sqlite3_exec(db, sql, reader.callback, &reader, &error);
      g_free(sql);
      if (rc != SQLITE_OK) {
        throw runtime_error(error);
      }
      data.push_back(reader.ustring0[0]);
    }
  }
  catch(exception & ex) {
    gw_critical(ex.what());
  }
  sqlite3_close(db);
}


void notes_update_old_one(ustring & note)
/*
 If there are newlines, and no <BR>, then this is an old note, 
 and the <BR>'s need to be added for proper display of the newlines
 in the GtkHtml viewer.
 */
{
  size_t pos = note.find("\n");
  if (pos == string::npos)
    return;
  pos = note.find("<BR>");
  if (pos != string::npos)
    return;
  replace_text(note, "\n", "<BR>\n");
}


const gchar *notes_cursor_anchor()
// Gives the name of the anchor where the cursor has to jump to.
{
  return "cursoranchor";
}


unsigned int notes_count (const ustring& alternate_directory)
{
  sqlite3 *db;
  sqlite3_open(notes_database_filename(alternate_directory).c_str(), &db);
  sqlite3_busy_timeout(db, 1000);
  SqliteReader reader(0);
  char *sql;
  sql = g_strdup_printf("select count(*) from '%s';", TABLE_NOTES);
  sqlite3_exec(db, sql, reader.callback, &reader, NULL);
  g_free(sql);
  gint count = 0;
  if (!reader.ustring0.empty()) {
    count = convert_to_int(reader.ustring0[0]);
  }
  sqlite3_close(db);
  return count;
}

