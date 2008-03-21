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



#include "oldgenconfig.h"
#include "directories.h"
#include "gwrappers.h"
#include "sqlite3.h"
#include "sqlite_reader.h"
#include "utilities.h"
#include <libxml/xmlwriter.h>
#include "tiny_utilities.h"


ustring old_project_configuration_filename (const ustring& project)
{
  return gw_build_filename (directories_get_projects (), project, "configuration.sqlite.1");
}


ustring new_project_configuration_filename (const ustring& project)
{
  return gw_build_filename (directories_get_projects (), project, "configuration.1.xml");
}


vector<ustring> old_project_valuelist_get (const ustring& project, const char * key)
{
  vector<ustring> value;
  vector<unsigned int> sequence;
  sqlite3 *db;
  sqlite3_open (old_project_configuration_filename (project).c_str (), &db);
  sqlite3_busy_timeout (db, 1000);
  SqliteReader sqlitereader (0);
  char * sql = g_strdup_printf ("select value, sequence from data where key = '%s';", key);
  sqlite3_exec(db, sql, sqlitereader.callback, &sqlitereader, NULL);
  g_free (sql);
  value.assign (sqlitereader.ustring0.begin(), sqlitereader.ustring0.end());
  for (unsigned int i = 0; i < sqlitereader.ustring1.size(); i++) {
    sequence.push_back (convert_to_int (sqlitereader.ustring1[i]));
  }
  quick_sort (sequence, value, 0, sequence.size());
  sqlite3_close (db);
  return value;
}


ustring new_projectconfig_element (char * name) 
{
  ustring element (name);
  replace_text (element, "_", "-");
  return element;
}


typedef struct {
  char *key;   // The key, as it is stored in the old configuration database.
  char *name;  // The name, as it will be used in the new code.
} old_project_config_record;


typeof (old_project_config_record) old_project_config_table [] = {
  { "stylesheet", "stylesheet" },
  { "versification", "versification" },
  { "print_fonts2", "printing_fonts" },
  { "print_font_size", "printing_font_size" },
  { "print_line_height", "printing_line_height" },
  { "sword_name", "sword_name" },
  { "sword_desc", "sword_description" },
  { "sword_about", "sword_about" },
  { "sword_lcsh", "sword_lcsh" },
  { "sword_license", "sword_license" },
  { "sword_version", "sword_version" },
  { "sword_lang", "sword_language" },
  { "reorder_books", "reordered_books" },
  { "books_incl", "reordered_includes" },
  { "books_portion", "reordered_portions" },
  { "language", "language" },
  { "book_order", "book_order" },
  { "editable", "editable" },
  { "backup_incremental_time", "backup_incremental_last_time" },
  { "backup_full", "backup_full" },
  { "repo_use", "git_use_remote_repository" },
  { "repo_conflict", "git_remote_repository_conflict_handling" },
  { "changes_last_time", "changes_last_review" },
  { "changes_since_time", "changes_since" },
  { "editor_font_default", "editor_font_default" },
  { "editor_font", "editor_font_name" },
  { "editor_color_default", "editor_default_color" },
  { "editor_normal_text_color", "editor_normal_text_color" },
  { "editor_background_color", "editor_background_color" },
  { "editor_selected_text_color", "editor_selected_text_color" },
  { "editor_selection_color", "editor_selection_color" }
};


void upgrade_old_project_configuration (const ustring& project)
{
  // No old configuration to upgrade: bail out.
  if (!g_file_test (old_project_configuration_filename (project).c_str(), G_FILE_TEST_IS_REGULAR)) return;
  
  cout << "Upgrading project configuration of " << project << endl;

  // Get a list of all distinct keys stored in the configuration.
  vector <ustring> keys;
  {
    sqlite3 *db;
    sqlite3_open (old_project_configuration_filename (project).c_str (), &db);
    sqlite3_busy_timeout (db, 1000);
    SqliteReader sqlitereader (0);
    char * sql = g_strdup_printf ("select distinct key from data");
    sqlite3_exec(db, sql, sqlitereader.callback, &sqlitereader, NULL);
    g_free (sql);
    keys = sqlitereader.ustring0;
    sqlite3_close (db);
  }
  
  // Create a new XML buffer, to which the document will be written
  xmlBufferPtr buffer = xmlBufferCreate();
  
  // Create a new XmlWriter for memory, with no compression.
  xmlTextWriterPtr writer = xmlNewTextWriterMemory(buffer, 0);

  // Create first bit of the document.
  xmlTextWriterStartDocument(writer, NULL, "UTF-8", NULL);
  
  // Indent the output.
  xmlTextWriterSetIndent(writer, 1);
    
  // Write the main element.
  xmlTextWriterStartElement (writer, BAD_CAST "bibledit-project-configuration");
    
  // Get the value of all the keys, and write them to the document.
  for (unsigned int i = 0; i < keys.size(); i++) {
    ustring key = keys[i];
    ustring name;
    for (unsigned int i2 = 0; i2 < sizeof (old_project_config_table) / sizeof (*old_project_config_table); i2++) {
      if (key == old_project_config_table[i2].key) {
        name = new_projectconfig_element (old_project_config_table[i2].name);
      }
    }
    if (name.empty()) continue;
    xmlTextWriterStartElement (writer, BAD_CAST name.c_str());
    vector <ustring> values = old_project_valuelist_get (project, key.c_str());
    for (unsigned int i2 = 0; i2 < values.size(); i2++) {
      xmlTextWriterStartElement (writer, BAD_CAST "value");
      xmlTextWriterWriteFormatString (writer, "%s", values[i2].c_str());
      xmlTextWriterEndElement (writer);      
    }
    xmlTextWriterEndElement (writer);
  }
  
  // We could close any open elements by hand, but closing the document does it all.
  xmlTextWriterEndDocument (writer);
  
  // Flush the writer, else nothing gets transferred to the buffer.
  xmlTextWriterFlush (writer);

  // Write the lines to disk.
  g_file_set_contents (new_project_configuration_filename (project).c_str(), (const gchar *) buffer->content, -1, NULL);

  // Free memory.
  if (writer) xmlFreeTextWriter (writer);
  if (buffer) xmlBufferFree (buffer);

  // Remove old configuration file.
  unlink (old_project_configuration_filename (project).c_str());
}
