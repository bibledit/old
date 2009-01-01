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
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
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

ExportTranslationNotes::ExportTranslationNotes(const ustring & filename, ExportNotesFormat format, const vector < unsigned int >&ids_to_display, bool export_all, GtkWidget * parent)
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
  myformat = format;

  // Start process.
  try {

    // Write to outputfile.
    WriteText wt(filename);
    my_wt = &wt;

    // Opening lines.
    switch (format) {
    case BibleditVersion3:
      {
        wt.text("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
        wt.text("<bibledit-notes version=\"3\">\n");
        break;
      }
    case ScriptureNotesVersion20:
      {
        wt.text("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
        wt.text("<ScriptureNotes Version=\"2.0\">\n");
        break;
      }
    }

    // Connect to database.
    error = NULL;
    rc = sqlite3_open(notes_database_filename().c_str(), &db);
    if (rc)
      throw runtime_error(sqlite3_errmsg(db));
    sqlite3_busy_timeout(db, 1000);

    // Get the number of notes.
    ustring sql;
    sql = "select count(*) from '";
    sql.append(TABLE_NOTES);
    sql.append("';");
    rc = sqlite3_exec(db, sql.c_str(), count_callback, this, &error);
    if (rc != SQLITE_OK) {
      throw runtime_error(error);
    }
    progresswindow.set_iterate(0, 1, notes_count);

    // Exporting everything or a selection?
    set < gint > ids(ids_to_display.begin(), ids_to_display.end());
    my_ids = &ids;

    // Go through all the notes.
    note_counter = 0;
    sql = "select ref_osis, project, category, note, created, modified, user, id from '";
    sql.append(TABLE_NOTES);
    sql.append("';");
    rc = sqlite3_exec(db, sql.c_str(), data_callback, this, &error);
    if (rc != SQLITE_OK) {
      throw runtime_error(error);
    }
    // Closing lines.
    switch (format) {
    case BibleditVersion3:
      {
        wt.text("</bibledit-notes>\n");
        break;
      }
    case ScriptureNotesVersion20:
      {
        wt.text("</ScriptureNotes>\n");
        break;
      }
    }

  }
  catch(exception & ex) {
    if (!progresswindow.cancel) {
      gtkw_dialog_error(parent, ex.what());
      gw_critical(ex.what());
    }
  }
}

ExportTranslationNotes::~ExportTranslationNotes()
{
  // Close connection.  
  sqlite3_close(db);
}

int ExportTranslationNotes::count_callback(void *userdata, int argc, char **argv, char **column_names)
{
  ((ExportTranslationNotes *) userdata)->on_count(argc, argv);
  return 0;
}

void ExportTranslationNotes::on_count(int argc, char **argv)
{
  notes_count = convert_to_int(argv[0]);
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
  switch (myformat) {
  case BibleditVersion3:
    {
      my_wt->text("  <note>\n");
      break;
    }
  case ScriptureNotesVersion20:
    {
      my_wt->text("  <Note>\n");
      break;
    }
  }

  // Deal with the references.
  ustring reference;
  reference = argv[0];
  // Parse the string into its possible several references.
  Parse parse(reference, false);
  reference.clear();
  switch (myformat) {
  case BibleditVersion3:
    {
      // Go through each reference and store it.
      for (unsigned int i = 0; i < parse.words.size(); i++) {
        if (!reference.empty())
          reference.append("\n");
        reference.append(parse.words[i]);
      }
      my_wt->text("    <references>");
      my_wt->text(reference);
      my_wt->text("</references>\n");
      break;
    }
  case ScriptureNotesVersion20:
    {
      // Go through each reference and store it.
      for (unsigned int i = 0; i < parse.words.size(); i++) {
        if (!reference.empty())
          reference.append("\n");
        reference.append(parse.words[i]);
      }
      my_wt->text("    <References>");
      my_wt->text(reference);
      my_wt->text("</References>\n");
      break;
    }
  }

  // Deal with the project name.
  ustring project;
  project = argv[1];
  switch (myformat) {
  case BibleditVersion3:
    {
      my_wt->text("    <project>" + project + "</project>\n");
      break;
    }
  case ScriptureNotesVersion20:
    {
      my_wt->text("    <Project>" + project + "</Project>\n");
      break;
    }
  }

  // Deal with the status of the note.
  ustring category;
  category = argv[2];
  switch (myformat) {
  case BibleditVersion3:
    {
      ustring referred_to;
      my_wt->text("    <category>" + category + "</category>\n");
      break;
    }
  case ScriptureNotesVersion20:
    {
      ustring tne_status;
      ustring category_lowercase = lowerCase(category);
      if (category_lowercase == "no issue")
        tne_status = "No Issue";
      else if (category_lowercase == "unresolved")
        tne_status = "Unresolved";
      else if (category_lowercase == "resolved")
        tne_status = "Resolved";
      else
        tne_status = "Unresolved";
      my_wt->text("    <Status>" + tne_status + "</Status>\n");
      break;
    }
  }

  // The note text.
  ustring note;
  note = argv[3];
  switch (myformat) {
  case BibleditVersion3:
    {
      my_wt->text("    <text>" + note + "</text>\n");
      break;
    }
  case ScriptureNotesVersion20:
    {
      my_wt->text("    <Contents><P>" + note + "</P></Contents>\n");
      break;
    }
  }

  // The date it was created.
  int created;
  created = convert_to_int(argv[4]);
  ustring date;
  date = trim(date_time_julian_human_readable(created, false));
  switch (myformat) {
  case BibleditVersion3:
    {
      my_wt->text("    <date-created>" + date + "</date-created>\n");
      break;
    }
  case ScriptureNotesVersion20:
    {
      my_wt->text("    <CreationDate>" + date + "</CreationDate>\n");
      break;
    }
  }

  // Deal with the modification date.
  int modified;
  modified = convert_to_int(argv[5]);
  date = trim(date_time_julian_human_readable(modified, false));
  switch (myformat) {
  case BibleditVersion3:
    {
      my_wt->text("    <date-modified>" + date + "</date-modified>\n");
      break;
    }
  case ScriptureNotesVersion20:
    {
      my_wt->text("    <ModificationDate>" + date + "</ModificationDate>\n");
      break;
    }
  }

  // Mention the one who created the note.
  ustring user;
  user = argv[6];
  switch (myformat) {
  case BibleditVersion3:
    {
      my_wt->text("    <created-by>" + user + "</created-by>\n");
      break;
    }
  case ScriptureNotesVersion20:
    {
      my_wt->text("    <CreatedBy>" + user + "</CreatedBy>\n");
      break;
    }
  }

  // Finish note.
  switch (myformat) {
  case BibleditVersion3:
    {
      my_wt->text("  </note>\n");
      break;
    }
  case ScriptureNotesVersion20:
    {
      my_wt->text("  </Note>\n");
      break;
    }
  }

  return 0;
}
