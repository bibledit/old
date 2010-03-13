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
#include "usfmtools.h"


void notes_store_index_entry (sqlite3 *db, gint32 id);


ustring notes_shared_storage_base_name ()
{
  return "data";
}


ustring notes_shared_storage_folder ()
{
  return gw_build_filename (directories_get_notes (), notes_shared_storage_base_name ());
}


ustring notes_index_filename ()
// Returns the filename of the notes index.
{ 
  return gw_build_filename(directories_get_notes(), "index.sql");
}


void notes_storage_verify()
// Verify / setup the project notes storage system.
{
  // Check an/or create the notes storage area.
  ustring directory;
  directory = notes_shared_storage_folder ();
  if (!g_file_test(directory.c_str(), G_FILE_TEST_IS_DIR)) {
    gw_mkdir_with_parents (directory);
  }

  // Convert old notes database to new format in separate files.
  // The reason for this is that notes in separate files can be shared through git.
  notes_convert_database_to_plain_files ();

  // If there's no index, create one.
  if (!g_file_test(notes_index_filename ().c_str(), G_FILE_TEST_IS_REGULAR)) {
    notes_create_index ();
  }
}


ustring notes_file_name (gint32 id)
// The filename for a note with "id".
{
  return gw_build_filename (notes_shared_storage_folder (), convert_to_string (id));
}


gint notes_database_get_unique_id()
// This generates a unique id, one that is not yet used in the notes storage.
{
  gint32 id = 0;
  bool found = false;
  while (!found) {
    id = g_random_int_range(1, 100000000);
    ustring filename = notes_file_name (id);
    found = !g_file_test (filename.c_str(), G_FILE_TEST_EXISTS);
  }
  return id;
}


void notes_delete_one(int id)
// Deletes the note with id.
{
  // Unlink the note file.
  unlink (notes_file_name (id).c_str());
  // Remove the entry from the index database
  sqlite3 *db;
  sqlite3_open(notes_index_filename().c_str(), &db);
  char *sql;
  sql = g_strdup_printf("delete from notes where id = %d;", id);
  sqlite3_exec(db, sql, NULL, NULL, NULL);
  g_free(sql);
  sqlite3_close(db);
  // Register this database for maintenance.
  maintenance_register_database (notes_index_filename());
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


void notes_select(vector <unsigned int>& ids, unsigned int & id_cursor, const ustring& currentreference)
/*
 This selects notes for display.
 It does this by calling another function that does the real work.
 The resulting selection will be stored in ids.
 It gives "id_cursor" which contains the id the cursor has to be put at.
 */
{
  // Get variables.
  extern Settings *settings;
  ustring category = settings->genconfig.notes_selection_category_get();
  NotesSelectionReferenceType refselection = (NotesSelectionReferenceType) settings->genconfig.notes_selection_reference_get();
  NotesSelectionEditedType editedselection = (NotesSelectionEditedType) settings->genconfig.notes_selection_edited_get();
  bool currentprojectselection = settings->genconfig.notes_selection_current_project_get();
  int date_from = settings->genconfig.notes_selection_date_from_get();
  int date_to = settings->genconfig.notes_selection_date_to_get();
  // Select notes.
  notes_select (ids, id_cursor, currentreference, category, refselection, editedselection, currentprojectselection, date_from, date_to);
}


void notes_select(vector <unsigned int>& ids, unsigned int & id_cursor, const ustring& currentreference,
                   const ustring& category, NotesSelectionReferenceType refselection, NotesSelectionEditedType editedselection,
                   bool currentprojectselection, int date_from, int date_to) 
/*
 This selects notes for display.
 It does this based on the variables passed, including the current reference.
 The resulting selection will be stored in ids.
 It gives "id_cursor" which contains the id the cursor has to be put at.
 */
{
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
  // Cursor focus calculation.
  int minimum_cursor_distance = G_MAXINT;
  // Database variables.
  sqlite3 *db;
  int rc;
  char *error = NULL;
  try {
    // Connect to database.
    rc = sqlite3_open(notes_index_filename().c_str(), &db);
    if (rc) throw runtime_error(sqlite3_errmsg(db));
    sqlite3_busy_timeout(db, 1000);
    SqliteReader sqlitereader(0);
    // See which notes to select.
    switch (refselection) {
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
          sql = g_strdup_printf("select id, reference, modified, project, category from notes where reference glob ('* %s *');", this_verse.c_str());
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
        sql = g_strdup_printf("select id, reference, modified, project, category from notes where reference glob ('* %s??? *');", this_chapter.c_str());
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
        sql = g_strdup_printf("select id, reference, modified, project, category from notes where reference glob ('* %s?????? *');", this_book.c_str());
        rc = sqlite3_exec(db, sql, sqlitereader.callback, &sqlitereader, &error);
        g_free(sql);
        if (rc != SQLITE_OK) {
          throw runtime_error(error);
        }
        break;
      }
      case nsrtAny:
      {
        rc = sqlite3_exec(db, "select id, reference, modified, project, category from notes;", sqlitereader.callback, &sqlitereader, &error);
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
      switch (editedselection) {
        case nsetToday:
        {
          if (modified_date != currentdate)
            continue;
          break;
        }
        case nsetDateRange:
        {
          if (modified_date < date_from)
            continue;
          if (modified_date > date_to)
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
      if (currentprojectselection) {
        bool project_ok = false;
        ustring project_in_db = sqlitereader.ustring3[rc];
        extern Settings *settings;
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


void notes_display_internal(const ustring& language, bool show_reference_text, bool show_summary, ustring& note_buffer, unsigned int id, const gchar * text, unsigned int cursor_id, unsigned int &cursor_offset)
{
  // Optionally display the extra text.
  if (text) {
    note_buffer.append(text);
    note_buffer.append("<BR>\n");
  }

  // Get data from the note file.
  ustring note;
  ustring project;
  ustring reference;
  ustring category;
  int date_created;
  ustring user_created;
  int date_modified;
  ustring logbook;
  notes_read_one_from_file (id, note, project, reference, category, date_created, user_created, date_modified, logbook);

  // Parse the reference(s) string into its possible several references.
  Parse parse(reference, false);
  reference.clear();

  // Keep list of references.
  vector <Reference> references;

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
  
  // Start creating the heading with links.
  ustring linkheading;
  // If this note is to be focused, then insert a special anchor for that: <a name="cursoranchor"></a>
  if (id == cursor_id) {
    linkheading.append("<a name=\"");
    linkheading.append(notes_cursor_anchor());
    linkheading.append("\"></a>");
  }
  extern Settings * settings;
  if (settings->session.project_notes_show_title) {
    // Insert a link with this heading, e.g.: <a href="10">Genesis 1.1</a>
    linkheading.append("<a href=\"" + convert_to_string(id) + "\">");
    linkheading.append(reference);
    if (settings->genconfig.notes_display_project_get())
      linkheading.append(" " + project);
    if (settings->genconfig.notes_display_category_get())
      linkheading.append(" " + category);
    if (settings->genconfig.notes_display_date_created_get())
      linkheading.append(" " + date_time_julian_human_readable(date_created, true));
    if (settings->genconfig.notes_display_created_by_get())
      linkheading.append(" " + user_created);
    linkheading.append("</a>");
    // Append a [delete] link too, e.g.: <a href="d10">[delete]</a>
    linkheading.append(" <a href=\"d" + convert_to_string(id) + "\">");
    linkheading.append("[delete]");
    linkheading.append("</a>");
    // Append a [references] link too, e.g.: <a href="r10">[references]</a>
    linkheading.append(" <a href=\"r" + convert_to_string(id) + "\">");
    linkheading.append("[references]");
    linkheading.append("</a>");
  }
  // Add the heading to the note data.
  note_buffer.append(linkheading);
  
  // Handle summary. Show only the first few words.
  if (show_summary) {
    ustring summary = note;
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
      vector <unsigned int> simple_verses = verse_range_sequence(references[r].verse);
      for (unsigned int sv = 0; sv < simple_verses.size(); sv++) {
        Reference ref(references[r]);
        ref.verse = convert_to_string(simple_verses[sv]);
        note_buffer.append(ref.human_readable(language));
        note_buffer.append(" ");
        ustring text = project_retrieve_verse(project, ref.book, ref.chapter, ref.verse);
        if (!text.empty()) {
          text = usfm_get_verse_text_only (text);
        }
        note_buffer.append(text);
        note_buffer.append("<BR>\n");
      }
    }
  }
  // Get the text of the note.
  if (!show_summary) {
    note_buffer.append(note);
    note_buffer.append("<BR>\n");
  }
}


void notes_display(ustring& note_buffer, vector <unsigned int> ids, unsigned int cursor_id, unsigned int &cursor_offset, bool & stop, unsigned int edited_note_id)
/*
 This collect html data for displaying the notes.
 It collects data for all the notes that have an id given in ids.
 It inserts a html anchor at the start of the note whose id is "cursor_id".
 If an "edited_note_id" is given that is not in the list of "ids", then it will display that one too, together with a message.
 */
{
  extern Settings *settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(settings->genconfig.project_get());
  ustring language = projectconfig->language_get();

  // Whether to show the text of the reference(s).
  bool show_reference_text = settings->genconfig.notes_display_reference_text_get();

  // Whether to show the summary only.
  bool show_summary = settings->genconfig.notes_display_summary_get();

  // See whether to display an extra note, one just edited.
  if (edited_note_id) {
    set <unsigned int> id_set (ids.begin(), ids.end());
    if (id_set.find (edited_note_id) == id_set.end()) {
      notes_display_internal(language, show_reference_text, show_summary, note_buffer, edited_note_id, 
                             "The following note is displayed because it was created or edited. Normally it would not have been displayed.", 
                             cursor_id, cursor_offset);
    }
  }    

  // Go through all the notes.
  for (unsigned int c = 0; c < ids.size(); c++) {

    // Handle possible stop command.
    if (stop)
      continue;

    // Display this note.
    notes_display_internal(language, show_reference_text, show_summary, note_buffer, ids[c], NULL, cursor_id, cursor_offset);

  }

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
    rc = sqlite3_open(notes_index_filename().c_str(), &db);
    if (rc) {
      throw runtime_error(sqlite3_errmsg(db));
    }
    sqlite3_busy_timeout(db, 1000);
    SqliteReader reader(0);
    rc = sqlite3_exec(db, "select category from notes;", reader.callback, &reader, &error);
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


void notes_projects_add_from_database(vector <ustring>& projects)
// Takes the existing projects, if there are any, and adds any
// extra projects found in the database.
{
  sqlite3 *db;
  int rc;
  char *error = NULL;
  try {
    // Get the unique categories.
    set < ustring > database_projects;
    rc = sqlite3_open(notes_index_filename().c_str(), &db);
    if (rc)
      throw runtime_error(sqlite3_errmsg(db));
    sqlite3_busy_timeout(db, 1000);
    SqliteReader reader(0);
    rc = sqlite3_exec(db, "select project from notes;", reader.callback, &reader, &error);
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


void notes_change_category(const ustring & from, const ustring & to)
// Changes all notes in category "from" to category "to".
{
  sqlite3 *db;
  int rc;
  char *error = NULL;
  try {
    rc = sqlite3_open(notes_index_filename().c_str(), &db);
    if (rc)
      throw runtime_error(sqlite3_errmsg(db));
    sqlite3_busy_timeout(db, 1000);

    // Read the id's of the notes with the old category.
    SqliteReader sqlitereader(0);
    char *sql;
    sql = g_strdup_printf("select id from notes where category = '%s';", from.c_str());
    rc = sqlite3_exec(db, sql, sqlitereader.callback, &sqlitereader, &error);
    g_free(sql);
    if (rc)
      throw runtime_error(error);
      
    // Change the category in the notes in file.
    // This will update the index database as well.
    for (unsigned int i = 0; i < sqlitereader.ustring0.size(); i++) {
      gint32 id = convert_to_int (sqlitereader.ustring0[i]);
      ustring note;
      ustring project;
      ustring references;
      ustring category;
      int date_created;
      ustring user_created;
      int date_modified;
      ustring logbook;
      notes_read_one_from_file (id, note, project, references, category, date_created, user_created, date_modified, logbook);
      category = to;
      notes_store_one_in_file(id, note, project, references, category, date_created, user_created, date_modified, logbook);
    }
  }
  catch(exception & ex) {
    gw_critical(ex.what());
  }
  sqlite3_close(db);
  // Register this database for maintenance.
  maintenance_register_database (notes_index_filename());
}


void notes_change_project(const ustring & from, const ustring & to)
// Changes all notes in project "from" to project "to".
{
  sqlite3 *db;
  int rc;
  char *error = NULL;
  try {
    rc = sqlite3_open(notes_index_filename().c_str(), &db);
    if (rc)
      throw runtime_error(sqlite3_errmsg(db));
    sqlite3_busy_timeout(db, 1000);

    // Read the id's of the notes with the old project.
    SqliteReader sqlitereader(0);
    char *sql;
    sql = g_strdup_printf("select id from notes where project = '%s';", from.c_str());
    rc = sqlite3_exec(db, sql, sqlitereader.callback, &sqlitereader, &error);
    g_free(sql);
    if (rc)
      throw runtime_error(error);
      
    // Change the project in the notes in file.
    // This will update the index database as well.
    for (unsigned int i = 0; i < sqlitereader.ustring0.size(); i++) {
      gint32 id = convert_to_int (sqlitereader.ustring0[i]);
      ustring note;
      ustring project;
      ustring references;
      ustring category;
      int date_created;
      ustring user_created;
      int date_modified;
      ustring logbook;
      notes_read_one_from_file (id, note, project, references, category, date_created, user_created, date_modified, logbook);
      project = to;
      notes_store_one_in_file(id, note, project, references, category, date_created, user_created, date_modified, logbook);
    }

  }
  catch(exception & ex) {
    gw_critical(ex.what());
  }
  sqlite3_close(db);
  // Register this database for maintenance.
  maintenance_register_database (notes_index_filename());
}


void notes_read(vector < unsigned int >ids, vector < ustring > &data)
// Reads notes.
{
  data.clear();
  for (unsigned int i = 0; i < ids.size(); i++) {
    ustring note;
    ustring project;
    ustring references;
    ustring category;
    int date_created;
    ustring user_created;
    int date_modified;
    ustring logbook;
    notes_read_one_from_file (ids[i], note, project, references, category, date_created, user_created, date_modified, logbook);
    data.push_back(note);
  }
}


const gchar *notes_cursor_anchor()
// Gives the name of the anchor where the cursor has to jump to.
{
  return "cursoranchor";
}


unsigned int notes_count ()
{
  sqlite3 *db;
  sqlite3_open(notes_index_filename().c_str(), &db);
  sqlite3_busy_timeout(db, 1000);
  SqliteReader reader(0);
  sqlite3_exec(db, "select count(*) from notes;", reader.callback, &reader, NULL);
  gint count = 0;
  if (!reader.ustring0.empty()) {
    count = convert_to_int(reader.ustring0[0]);
  }
  sqlite3_close(db);
  return count;
}


void notes_convert_database_to_plain_files ()
{
  // Bail out if there's no database to convert.
  ustring database_filename = gw_build_filename(directories_get_notes(), "notes.sql2");
  if (!g_file_test(database_filename.c_str(), G_FILE_TEST_IS_REGULAR)) {
    return;
  }

  // Progress window.
  ProgressWindow progresswindow ("Converting project notes", false);
  
  // Access the database.
  sqlite3 *db;
  int rc;
  char *error = NULL;
  try {
    rc = sqlite3_open(database_filename.c_str(), &db);
    if (rc) throw runtime_error(sqlite3_errmsg(db));
    sqlite3_busy_timeout(db, 1000);
    {
      // Read how many notes there are.
      SqliteReader reader(0);
      char *sql;
      sql = g_strdup_printf("select count(*) from notes;");
      rc = sqlite3_exec(db, sql, reader.callback, &reader, &error);
      g_free(sql);
      if (rc != SQLITE_OK) throw runtime_error(error);
      if (!reader.ustring0.empty()) {
        progresswindow.set_iterate (0, 1, convert_to_int (reader.ustring0[0]));
      }
    }
    {
      SqliteReader reader(0);
      rc = sqlite3_exec(db, "select id, ref_osis, project, category, note, created, modified, user, logbook from notes;", reader.callback, &reader, &error);
      if (rc != SQLITE_OK) throw runtime_error(error);
      for (unsigned int i = 0; i < reader.ustring0.size(); i++) {
        progresswindow.iterate ();
        notes_store_one_in_file(convert_to_int (reader.ustring0[i]), // ID.
                                reader.ustring4[i],                  // Note.
                                reader.ustring2[i],                  // Project.
                                reader.ustring1[i],                  // References.
                                reader.ustring3[i],                  // Category.
                                convert_to_int (reader.ustring5[i]), // Date created.
                                reader.ustring7[i],                  // User.
                                convert_to_int (reader.ustring6[i]), // Date modified.
                                reader.ustring8[i]);                 // Logbook.
      }
    }
    // Delete the notes database that was converted.
    unlink (database_filename.c_str());
  }
  catch(exception & ex) {
    gw_critical(ex.what());
  }
  sqlite3_close(db);
}


void notes_store_one_in_file(int id, ustring & note, const ustring & project, const ustring& references, const ustring & category, int date_created, const ustring & user_created, int date_modified, ustring & logbook)
// Stores a note in a file.
// It should be flat files, not xml, because of the merging done by git.
{
  // Store the note.
  vector <ustring> lines;

  // Store date created.
  lines.push_back (convert_to_string (date_created));
  // Store user who created it.
  lines.push_back (user_created);
  // Store references.
  lines.push_back (references);
  // Store category.
  lines.push_back (category);
  // Store project.
  lines.push_back (project);
  // Store date modified.
  lines.push_back (convert_to_string (date_modified));
  // Store note.
  lines.push_back (note);
  // Store logbook separator.
  lines.push_back (notes_logbook_line ());
  // Store logbook.
  lines.push_back (logbook);

  // Write the note. The filename is the note id.
  ustring note_filename = notes_file_name (id);
  write_lines (note_filename, lines);
  
  // Update the index.
  sqlite3 *db;
  sqlite3_open(notes_index_filename ().c_str(), &db);
  sqlite3_busy_timeout(db, 1000);
  notes_store_index_entry (db, id);
  sqlite3_close(db);
}


void notes_read_one_from_file (int id, ustring& note, ustring& project, ustring& references, ustring& category, int& date_created, ustring& user_created, int& date_modified, ustring& logbook)
{
  note.clear();
  logbook.clear();
  ustring filename = notes_file_name (id);
  ReadText rt (filename, true, false);
  bool logbook_indicator_encountered = false;
  for (unsigned int i = 0; i < rt.lines.size(); i++) {
    ustring line = rt.lines[i];
    if (i == 0) {
      // Retrieve date created.
      date_created = convert_to_int (line);
    }
    else if (i == 1) {
      // Retrieve user who created it.
      user_created = line;
    }
    else if (i == 2) {
      // Retrieve references.
      references = line;
    }
    else if (i == 3) {
      // Retrieve category.
      category = line;
    }
    else if (i == 4) {
      // Retrieve project.
      project = line;
    }
    else if (i == 5) {
      // Retrieve date modified.
      date_modified = convert_to_int (line);
    }
    else if (line == notes_logbook_line ()) {
      logbook_indicator_encountered = true;
    }
    else {
      if (logbook_indicator_encountered) {
        if (!logbook.empty()) logbook.append ("\n");
        logbook.append (line);
      } else {
        if (!note.empty()) note.append ("\n");
        note.append (line);
      }
    }
  }
  note = trim (note);
  logbook = trim (logbook);
}


ustring notes_logbook_line ()
{
  return "Logbook:";
}


void notes_store_index_entry (sqlite3 *db, gint32 id)
{
  gchar *sql;

  // Delete optional previous entry with "id".
  sql = g_strdup_printf("delete from notes where id = %d;", id);
  sqlite3_exec(db, sql, NULL, NULL, NULL);
  g_free(sql);

  // Read the note with "id".
  ustring note;
  ustring project;
  ustring references;
  ustring category;
  int date_created;
  ustring user_created;
  int date_modified;
  ustring logbook;
  notes_read_one_from_file (id, note, project, references, category, date_created, user_created, date_modified, logbook);
  
  // Bail out if there's no note.
  if (note.empty()) {
    return;
  }

  // Attend to the id: use variable "id".
  
  // Attend to the encoded references.
  Parse parse(references, false);
  ustring encoded_references;
  for (unsigned int i = 0; i < parse.words.size(); i++) {
    Reference reference(0);
    reference_discover(0, 0, "", parse.words[i], reference.book, reference.chapter, reference.verse);
    ustring book = books_id_to_english(reference.book);
    ustring chapter = convert_to_string(reference.chapter);
    vector < int >verses = verses_encode(reference.verse);
    int book_chapter = reference_to_numerical_equivalent(book, chapter, "0");
    for (unsigned int i2 = 0; i2 < verses.size(); i2++) {
      encoded_references.append(" ");
      encoded_references.append(convert_to_string(int (book_chapter + verses[i2])));
    }
  }
  encoded_references.append(" ");

  // Attend to the project: use variable "project".
  // Apostrophies need to be doubled before storing them.
  project = double_apostrophy(project);

  // Attend to the category: use variable "category".
  // Apostrophies need to be doubled before storing them.
  category = double_apostrophy(category);
  
  // Attend to the note text in case folded form
  // Apostrophies need to be doubled before storing them.
  note = note.casefold ();
  note = double_apostrophy(note);
  
  // Attend to the date created: use variable "date_created".
  
  // Attend to the date modified: use variable "date_modified".

  // Put new data in the database.
  sql = g_strdup_printf("insert into notes values (%d, '%s', '%s', '%s', '%s', %d, %d);", 
                        id, encoded_references.c_str(), project.c_str(), category.c_str(), note.c_str(), date_created, date_modified);
  sqlite3_exec(db, sql, NULL, NULL, NULL);
  g_free(sql);
}


void notes_create_index ()
// This creates an index for the notes stored in plain files.
{
  ProgressWindow progresswindow ("Creating notes index", false);

  // Remove any old index.
  unlink (notes_index_filename ().c_str());
  
  // Create index database.
  sqlite3 *db;
  int rc;
  char *error = NULL;
  try {

    // Connect to the index database.
    rc = sqlite3_open(notes_index_filename ().c_str(), &db);
    if (rc)
      throw runtime_error(sqlite3_errmsg(db));
    sqlite3_busy_timeout(db, 1000);

    // Create the notes table.
    char *sql;
    sql = g_strdup_printf("create table notes (id integer, reference text, project text, category text, casefolded text, created integer, modified integer);");
    rc = sqlite3_exec(db, sql, NULL, NULL, &error);
    g_free(sql);
    // Fast writing.
    sqlite3_exec(db, "PRAGMA synchronous=OFF;", NULL, NULL, NULL); 
    
    // Create an index for all the note files. The index will speed up note selection.
    ReadFiles note_files (notes_shared_storage_folder (), "", "");
    progresswindow.set_iterate (0, 1, note_files.files.size());
    for (unsigned int i = 0; i < note_files.files.size(); i++) {
      progresswindow.iterate ();
      notes_store_index_entry (db, convert_to_int (note_files.files[i]));
    }
    
  }
  catch(exception & ex) {
    gw_critical(ex.what());
    unlink (notes_index_filename ().c_str());
  }

  // Close connection.  
  sqlite3_close(db);
}


void notes_handle_vcs_feedback (const ustring& directory, const ustring& feedback)
// This handles the feedback that comes from the version control system.
{
  if (directory == notes_shared_storage_folder ()) {

    unsigned int note_id = 0;

    // The following feedback indicates that somebody created a new note: 
    // create mode 100644 27185458
    if (feedback.find ("create mode") != string::npos) {
      ustring s (feedback);
      s.erase (12);
      Parse parse (s);
      if (parse.words.size () == 2) {
        note_id = convert_to_int (parse.words[1]);
      }
    }

    // The following feedback indicates that somebody modified a note:
    // #	modified:   27185458
    if (feedback.find ("modified:") != string::npos) {
      ustring s (feedback);
      s.erase (12);
      note_id = convert_to_int (number_in_string (feedback));
    }

    // The following feedback indicates that somebody deleted a note:
    // #	deleted:    46473236    
    if (feedback.find ("deleted:") != string::npos) {
      ustring s (feedback);
      s.erase (11);
      note_id = convert_to_int (number_in_string (feedback));
    }
  
    // See the following:
    // 95935882 |    9 +++++++++
    // It means that this note was edited.
    if (feedback.find (" |  ") != string::npos) {
      note_id = convert_to_int (number_in_string (feedback));
    }
    
    if (note_id != 0) {
      gw_message ("Change detected for note " + convert_to_string (note_id));
      // Update the index.
      sqlite3 *db;
      sqlite3_open(notes_index_filename ().c_str(), &db);
      sqlite3_busy_timeout(db, 1000);
      notes_store_index_entry (db, note_id);
      sqlite3_close(db);
    }    
  }
}


