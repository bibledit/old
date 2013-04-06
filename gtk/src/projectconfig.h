/*
** Copyright (©) 2003-2013 Teus Benschop.
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


#ifndef INCLUDED_PROJECTCONFIGURATION_H
#define INCLUDED_PROJECTCONFIGURATION_H


#include "libraries.h"
#include "constants.h"
#include "settings.h"


ustring project_configuration_filename (const ustring& project);
void upgrade_project_configuration (const ustring& project);


// Definitions of various types of variables in the project configuration.
#define DEFINE_PROJECTCONFIG_H(type, parameter) \
private: \
  type    parameter; \
  bool    parameter##_loaded; \
  const gchar * parameter##_key (); \
public: \
  type    parameter##_get (); \
  void    parameter##_set (type value) \


class ProjectConfiguration
// Manages program settings.
{
public:
  ProjectConfiguration (ustring project_in, bool save_on_destroy = true);
  ~ProjectConfiguration ();
  void save ();
  ustring project;
  // Private and public definitions follow.
  DEFINE_PROJECTCONFIG_H (ustring, versification);
  DEFINE_PROJECTCONFIG_H (vector<ustring>, printing_fonts);
  DEFINE_PROJECTCONFIG_H (int, text_line_height);
  DEFINE_PROJECTCONFIG_H (ustring, xetex_font_mapping_file);
  DEFINE_PROJECTCONFIG_H (int, xetex_shaping_engine);
  DEFINE_PROJECTCONFIG_H (ustring, sword_name);
  DEFINE_PROJECTCONFIG_H (ustring, sword_description);
  DEFINE_PROJECTCONFIG_H (ustring, sword_about);
  DEFINE_PROJECTCONFIG_H (ustring, sword_license);
  DEFINE_PROJECTCONFIG_H (ustring, sword_version);
  DEFINE_PROJECTCONFIG_H (ustring, sword_language);
  DEFINE_PROJECTCONFIG_H (vector<ustring>, reordered_books);
  DEFINE_PROJECTCONFIG_H (vector<bool>, reordered_includes);
  DEFINE_PROJECTCONFIG_H (vector<ustring>, reordered_portions);
  DEFINE_PROJECTCONFIG_H (ustring, language);
  DEFINE_PROJECTCONFIG_H (vector<int>, book_order);
  DEFINE_PROJECTCONFIG_H (bool, editable);
  DEFINE_PROJECTCONFIG_H (int, backup_incremental_last_time);
  DEFINE_PROJECTCONFIG_H (ustring, backup_comment);
  DEFINE_PROJECTCONFIG_H (bool, git_use_remote_repository);
  DEFINE_PROJECTCONFIG_H (ustring, git_remote_repository_url);
  DEFINE_PROJECTCONFIG_H (int, git_remote_repository_conflict_handling);
  DEFINE_PROJECTCONFIG_H (int, changes_last_review);
  DEFINE_PROJECTCONFIG_H (int, changes_since);
  DEFINE_PROJECTCONFIG_H (int, changes_till);
  DEFINE_PROJECTCONFIG_H (bool, editor_font_default);
  DEFINE_PROJECTCONFIG_H (ustring, editor_font_name);
  DEFINE_PROJECTCONFIG_H (bool, editor_default_color);
  DEFINE_PROJECTCONFIG_H (int, editor_normal_text_color);
  DEFINE_PROJECTCONFIG_H (int, editor_background_color);
  DEFINE_PROJECTCONFIG_H (int, editor_selected_text_color);
  DEFINE_PROJECTCONFIG_H (int, editor_selection_color);
  DEFINE_PROJECTCONFIG_H (bool, right_to_left);
  DEFINE_PROJECTCONFIG_H (int, planning_project_start);
  DEFINE_PROJECTCONFIG_H (vector<ustring>, planning_tasks);
  DEFINE_PROJECTCONFIG_H (bool, depending_on_switch);
  DEFINE_PROJECTCONFIG_H (ustring, depending_on_project);
  DEFINE_PROJECTCONFIG_H (ustring, depending_on_script);
  DEFINE_PROJECTCONFIG_H (bool, spelling_check);
  DEFINE_PROJECTCONFIG_H (vector<ustring>, spelling_dictionaries);
private:
  bool my_save_on_destroy;
  bool bool_get (const gchar * key, bool& store, bool& loaded, bool standard);
  int int_get (const gchar * key, int& store, bool& loaded, int standard);
  ustring string_get (const gchar * key, ustring& store, bool& loaded, const ustring& standard);
  double double_get (const gchar * key, double& store, bool& loaded, double standard);
  vector<bool> vector_bool_get (const gchar * key, vector<bool>& store, bool& loaded, void * dummy);
  vector<ustring> vector_string_get (const gchar * key, vector<ustring>& store, bool& loaded, void * dummy);
  vector<int> vector_int_get (const gchar * key, vector<int>& store, bool& loaded, void * dummy);
};


#endif
