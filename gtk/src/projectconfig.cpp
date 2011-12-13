/*
** Copyright (©) 2003-2012 Teus Benschop.
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

#include "directories.h"
#include "gwrappers.h"
#include "utilities.h"
#include "date_time_utils.h"
#include "settings.h"
#include "stylesheetutils.h"
#include "settings.h"
#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>
#include "config.xml.h"

ustring project_configuration_filename(const ustring & project)
{
  return gw_build_filename(directories_get_projects(), project, "configuration.1.xml");
}

void upgrade_project_configuration(const ustring & project)
{
}

ProjectConfiguration::ProjectConfiguration(ustring project_in, bool save_on_destroy)
{
  // If no project given, take one currently opened.
  if (project.empty()) {
    extern Settings *settings;
    project = settings->genconfig.project_get();
  }
  // Save parameters.
  project = project_in;
  my_save_on_destroy = save_on_destroy;

  // Function definition for initializing variables.
#define INITIALIZE(parameter) parameter##_loaded = false

  // Initialize variables.
  INITIALIZE(versification);
  INITIALIZE(printing_fonts);
  INITIALIZE(text_line_height);
  INITIALIZE(xetex_font_mapping_file);
  INITIALIZE(xetex_shaping_engine);
  INITIALIZE(sword_name);
  INITIALIZE(sword_description);
  INITIALIZE(sword_about);
  INITIALIZE(sword_license);
  INITIALIZE(sword_version);
  INITIALIZE(sword_language);
  INITIALIZE(reordered_books);
  INITIALIZE(reordered_includes);
  INITIALIZE(reordered_portions);
  INITIALIZE(language);
  INITIALIZE(book_order);
  INITIALIZE(editable);
  INITIALIZE(backup_incremental_last_time);
  INITIALIZE(backup_comment);
  INITIALIZE(git_use_remote_repository);
  INITIALIZE(git_remote_repository_url);
  INITIALIZE(git_remote_repository_conflict_handling);
  INITIALIZE(changes_last_review);
  INITIALIZE(changes_since);
  INITIALIZE(changes_till);
  INITIALIZE(editor_font_default);
  INITIALIZE(editor_font_name);
  INITIALIZE(editor_default_color);
  INITIALIZE(editor_normal_text_color);
  INITIALIZE(editor_background_color);
  INITIALIZE(editor_selected_text_color);
  INITIALIZE(editor_selection_color);
  INITIALIZE(right_to_left);
  INITIALIZE(planning_project_start);
  INITIALIZE(planning_tasks);
  INITIALIZE(depending_on_switch);
  INITIALIZE(depending_on_project);
  INITIALIZE(depending_on_script);
  INITIALIZE(spelling_check);
  INITIALIZE(spelling_dictionaries);
}

ProjectConfiguration::~ProjectConfiguration()
{
  if (my_save_on_destroy)
    save();
}

void ProjectConfiguration::save()
// Saves all settings to disk.
{
  vector < ConfigXmlPair > values;

#define SAVE_VALUE(item) if (item##_loaded) config_xml_values_set_assemble (values, item##_key(), item)

  SAVE_VALUE(versification);
  SAVE_VALUE(printing_fonts);
  SAVE_VALUE(text_line_height);
  SAVE_VALUE(xetex_font_mapping_file);
  SAVE_VALUE(xetex_shaping_engine);
  SAVE_VALUE(sword_name);
  SAVE_VALUE(sword_description);
  SAVE_VALUE(sword_about);
  SAVE_VALUE(sword_license);
  SAVE_VALUE(sword_version);
  SAVE_VALUE(sword_language);
  SAVE_VALUE(reordered_books);
  SAVE_VALUE(reordered_includes);
  SAVE_VALUE(reordered_portions);
  SAVE_VALUE(language);
  SAVE_VALUE(book_order);
  SAVE_VALUE(editable);
  SAVE_VALUE(backup_incremental_last_time);
  SAVE_VALUE(backup_comment);
  SAVE_VALUE(git_use_remote_repository);
  SAVE_VALUE(git_remote_repository_url);
  SAVE_VALUE(git_remote_repository_conflict_handling);
  SAVE_VALUE(changes_last_review);
  SAVE_VALUE(changes_since);
  SAVE_VALUE(changes_till);
  SAVE_VALUE(editor_font_default);
  SAVE_VALUE(editor_font_name);
  SAVE_VALUE(editor_default_color);
  SAVE_VALUE(editor_normal_text_color);
  SAVE_VALUE(editor_background_color);
  SAVE_VALUE(editor_selected_text_color);
  SAVE_VALUE(editor_selection_color);
  SAVE_VALUE(right_to_left);
  SAVE_VALUE(planning_project_start);
  SAVE_VALUE(planning_tasks);
  SAVE_VALUE(depending_on_switch);
  SAVE_VALUE(depending_on_project);
  SAVE_VALUE(depending_on_script);
  SAVE_VALUE(spelling_check);
  SAVE_VALUE(spelling_dictionaries);

  config_xml_values_set_execute(project_configuration_filename(project), values);
}

bool ProjectConfiguration::bool_get(const gchar * key, bool & store, bool & loaded, bool standard)
{
  if (!loaded) {
    store = config_xml_bool_get(project_configuration_filename(project), key, standard);
    loaded = true;
  }
  return store;
}

int ProjectConfiguration::int_get(const gchar * key, int &store, bool & loaded, int standard)
{
  if (!loaded) {
    store = config_xml_int_get(project_configuration_filename(project), key, standard);
    loaded = true;
  }
  return store;
}

ustring ProjectConfiguration::string_get(const gchar * key, ustring & store, bool & loaded, const ustring & standard)
{
  if (!loaded) {
    store = config_xml_string_get(project_configuration_filename(project), key, standard);
    loaded = true;
  }
  return store;
}

double ProjectConfiguration::double_get(const gchar * key, double &store, bool & loaded, double standard)
{
  if (!loaded) {
    store = config_xml_double_get(project_configuration_filename(project), key, standard);
    loaded = true;
  }
  return store;
}

vector < bool > ProjectConfiguration::vector_bool_get(const gchar * key, vector < bool > &store, bool & loaded, void *dummy)
{
  if (!loaded) {
    store = config_xml_vector_bool_get(project_configuration_filename(project), key);
    loaded = true;
  }
  return store;
}

vector < ustring > ProjectConfiguration::vector_string_get(const gchar * key, vector < ustring > &store, bool & loaded, void *dummy)
{
  if (!loaded) {
    store = config_xml_vector_string_get(project_configuration_filename(project), key);
    loaded = true;
  }
  return store;
}

vector < int >ProjectConfiguration::vector_int_get(const gchar * key, vector < int >&store, bool & loaded, void *dummy)
{
  if (!loaded) {
    store = config_xml_vector_int_get(project_configuration_filename(project), key);
    loaded = true;
  }
  return store;
}

// Definitions of the implementation of the code in the project configuration.
#define IMPLEMENT(type, getter, store, defaultvalue) \
const gchar * ProjectConfiguration::store##_key () \
{ \
  return #store; \
} \
type ProjectConfiguration::store##_get () \
{ \
  return getter (store##_key (), store, store##_loaded, defaultvalue); \
} \
void ProjectConfiguration::store##_set (type value) \
{ \
  store = value; \
  store##_loaded = true; \
}

IMPLEMENT(ustring, string_get, versification, "English")
IMPLEMENT(vector < ustring >, vector_string_get, printing_fonts, NULL)
IMPLEMENT(int, int_get, text_line_height, 100)
IMPLEMENT(ustring, string_get, xetex_font_mapping_file, "")
IMPLEMENT(int, int_get, xetex_shaping_engine, 0)
IMPLEMENT(ustring, string_get, sword_name, "")
IMPLEMENT(ustring, string_get, sword_description, " Bibledit project")
IMPLEMENT(ustring, string_get, sword_about, " Bibledit project")
IMPLEMENT(ustring, string_get, sword_license, "GNU General Public License")
IMPLEMENT(ustring, string_get, sword_version, "1.0")
IMPLEMENT(ustring, string_get, sword_language, "en")
IMPLEMENT(vector < ustring >, vector_string_get, reordered_books, NULL)
IMPLEMENT(vector < bool >, vector_bool_get, reordered_includes, NULL)
IMPLEMENT(vector < ustring >, vector_string_get, reordered_portions, NULL)
IMPLEMENT(ustring, string_get, language, "English")
IMPLEMENT(vector < int >, vector_int_get, book_order, NULL)
IMPLEMENT(bool, bool_get, editable, true)
IMPLEMENT(int, int_get, backup_incremental_last_time, 0)
IMPLEMENT(ustring, string_get, backup_comment, "")
IMPLEMENT(bool, bool_get, git_use_remote_repository, false)
IMPLEMENT(ustring, string_get, git_remote_repository_url, "")
IMPLEMENT(int, int_get, git_remote_repository_conflict_handling, 0)
IMPLEMENT(int, int_get, changes_last_review, 0)
IMPLEMENT(int, int_get, changes_since, 0)
IMPLEMENT(int, int_get, changes_till, 0)
IMPLEMENT(bool, bool_get, editor_font_default, true)
IMPLEMENT(ustring, string_get, editor_font_name, "Sans 14")
IMPLEMENT(bool, bool_get, editor_default_color, true)
IMPLEMENT(int, int_get, editor_normal_text_color, 0)
IMPLEMENT(int, int_get, editor_background_color, 16777215)
IMPLEMENT(int, int_get, editor_selected_text_color, 16777215)
IMPLEMENT(int, int_get, editor_selection_color, 4294343)
IMPLEMENT(bool, bool_get, right_to_left, false) IMPLEMENT(int, int_get, planning_project_start, date_time_julian_day_get_current()) IMPLEMENT(vector < ustring >, vector_string_get, planning_tasks, NULL) IMPLEMENT(bool, bool_get, depending_on_switch, false) IMPLEMENT(ustring, string_get, depending_on_project, "") IMPLEMENT(ustring, string_get, depending_on_script, "") IMPLEMENT(bool, bool_get, spelling_check, false) IMPLEMENT(vector < ustring >, vector_string_get, spelling_dictionaries, NULL)
