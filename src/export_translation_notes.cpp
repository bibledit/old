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
#include "export_utils.h"
#include "bible.h"
#include "usfm.h"
#include "usfmtools.h"
#include "notes_utils.h"
#include "date_time_utils.h"
#include "export_translation_notes.h"
#include "gwrappers.h"
#include "gtkwrappers.h"
#include "tiny_utilities.h"
#include "sqlite_reader.h"


ExportTranslationNotes::ExportTranslationNotes(const ustring & filename, const vector < unsigned int >&ids_to_display, bool export_all)
// Exports the notes from the database.
/*
The exported xml file will look so:

<?xml version="1.0" encoding="UTF-8"?>
<bibledit-notes version="3">
  <note>
    <reference>
    </reference>
    <project>
    </project>
    <category>
    </category>
    <text>
    </text>
    <date-created>
    </date-created>
    <date-modified>
    </date-modified>
    <user>
    </user>
  </note>
</bibledit-notes>

This is subject to change as bibledit's notes system develop.
*/
:progresswindow("Exporting notes", true)
{
  // Save variables.
  my_export_all = export_all;

  // Start process.
  try {

    // Write to outputfile.
    WriteText wt(filename);
    my_wt = &wt;

    // Opening lines.
    wt.text("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    wt.text("<bibledit-notes version=\"3\">\n");

    // Connect to database.
    error = NULL;
    rc = sqlite3_open(notes_index_filename().c_str(), &db);
    if (rc)
      throw runtime_error(sqlite3_errmsg(db));
    sqlite3_busy_timeout(db, 1000);

    // Get the number of notes.
    SqliteReader sqlitereader(0);
    rc = sqlite3_exec(db, "select count(*) from notes;", sqlitereader.callback, &sqlitereader, &error);
    if (rc != SQLITE_OK) {
      throw runtime_error(error);
    }
    if (!sqlitereader.ustring0.empty()) {
      notes_count = convert_to_int (sqlitereader.ustring0[0]);
    }
    progresswindow.set_iterate(0, 1, notes_count);

    // Exporting everything or a selection?
    set < gint > ids(ids_to_display.begin(), ids_to_display.end());
    my_ids = &ids;

    // Go through all the notes.
    note_counter = 0;
    rc = sqlite3_exec(db, "select ref_osis, project, category, note, created, modified, user, id from notes;", data_callback, this, &error);
    if (rc != SQLITE_OK) {
      throw runtime_error(error);
    }
    // Closing lines.
    wt.text("</bibledit-notes>\n");

  }
  catch(exception & ex) {
    if (!progresswindow.cancel) {
      gtkw_dialog_error(NULL, ex.what());
      gw_critical(ex.what());
    }
  }
}


ExportTranslationNotes::~ExportTranslationNotes()
{
  // Close connection.  
  sqlite3_close(db);
}


int ExportTranslationNotes::data_callback(void *userdata, int argc, char **argv, char **column_names)
{
  return ((ExportTranslationNotes *) userdata)->on_data(argc, argv);
}


int ExportTranslationNotes::on_data(int argc, char **argv)
{
  // Progress information.
  note_counter++;
  progresswindow.iterate();
  if (progresswindow.cancel) {
    return 1;
  }
  // See whether to display this particular id.
  if (!my_export_all) {
    gint id;
    id = convert_to_int(argv[7]);
    if (my_ids->find(id) == my_ids->end())
      return 0;
  }
  // Start note.
  my_wt->text("  <note>\n");

  // Deal with the references.
  ustring reference;
  reference = argv[0];
  // Parse the string into its possible several references.
  Parse parse(reference, false);
  reference.clear();
  // Go through each reference and store it.
  for (unsigned int i = 0; i < parse.words.size(); i++) {
    if (!reference.empty())
      reference.append("\n");
    reference.append(parse.words[i]);
  }
  my_wt->text("    <references>");
  my_wt->text(reference);
  my_wt->text("</references>\n");

  // Deal with the project name.
  ustring project;
  project = argv[1];
  my_wt->text("    <project>" + project + "</project>\n");

  // Deal with the status of the note.
  ustring category;
  category = argv[2];
  my_wt->text("    <category>" + category + "</category>\n");

  // The note text.
  ustring note;
  note = argv[3];
  my_wt->text("    <text>" + note + "</text>\n");

  // The date it was created.
  int created;
  created = convert_to_int(argv[4]);
  ustring date;
  date = trim(date_time_julian_human_readable(created, false));
  my_wt->text("    <date-created>" + date + "</date-created>\n");

  // Deal with the modification date.
  int modified;
  modified = convert_to_int(argv[5]);
  date = trim(date_time_julian_human_readable(modified, false));
  my_wt->text("    <date-modified>" + date + "</date-modified>\n");

  // Mention the one who created the note.
  ustring user;
  user = argv[6];
  my_wt->text("    <created-by>" + user + "</created-by>\n");

  // Finish note.
  my_wt->text("  </note>\n");

  return 0;
}

