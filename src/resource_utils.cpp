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

#include "resource_utils.h"
#include "directories.h"
#include "gwrappers.h"
#include "unixwrappers.h"
#include "utilities.h"
#include "books.h"
#include "settings.h"
#include "htmlbrowser.h"
#include "html.h"
#include "tiny_utilities.h"
#include "dialoglistview.h"
#include "gtkwrappers.h"

ustring resource_viewer_directory() {
  return gw_build_filename(directories_get_temp(), "resource_viewer");
}

void resource_viewer_ensure_directory()
// Ensures that the working directory for the resource viewer is there.
{
  ustring directory = resource_viewer_directory();
  if (!g_file_test(directory.c_str(), G_FILE_TEST_IS_DIR)) {
    gw_mkdir_with_parents(directory);
  }
}

ustring resource_viewer_reference_html_filename() {
  ustring filename("bibledit_resource_viewer_reference.html");
  return filename;
}

ustring resource_viewer_main_html_filename() {
  ustring filename("bibledit_resource_viewer_main.html");
  return filename;
}

ustring resource_viewer_produce_anchor(unsigned int book, unsigned int chapter, unsigned int verse)
// Produces the anchor, e.g.: Matthew_1_1 or Song_of_Solomon_2_2.
{
  ustring anchor(books_id_to_english(book) + "_" + convert_to_string(chapter) + "_" + convert_to_string(verse));
  replace_text(anchor, " ", "_");
  return anchor;
}

void resource_open(ResourcesGUI * resourcesgui)
// This function cares for opening a resource through the menu.
{
  if (!resourcesgui)
    return;
  vector <ustring> filenames;
  vector <ustring> resources = resource_get_resources(filenames, false);
  quick_sort(resources, filenames, 0, resources.size());
  ListviewDialog dialog("Open resource", resources, "", false, NULL);
  if (dialog.run() == GTK_RESPONSE_OK) {
    ustring filename;
    for (unsigned int i = 0; i < resources.size(); i++) {
      if (dialog.focus == resources[i]) {
        filename = filenames[i];
      }
    }
    if (filename.empty())
      return;
    resourcesgui->open(filename, -1);
  }
}

void resource_close(ResourcesGUI * resourcesgui) {
  if (!resourcesgui)
    return;
  resourcesgui->close();
}

void resource_delete(ResourcesGUI * resourcesgui)
// Delete a resource from the available ones.
{
  vector <ustring> filenames;
  vector <ustring> resources = resource_get_resources(filenames, false);
  ListviewDialog dialog("Delete resource", resources, "", false, NULL);
  if (dialog.run() == GTK_RESPONSE_OK) {
    int result = gtkw_dialog_question(NULL, "Are you sure you want to delete resource " + dialog.focus + "?");
    if (result == GTK_RESPONSE_YES) {
      ustring filename;
      for (unsigned int i = 0; i < resources.size(); i++) {
        if (dialog.focus == resources[i])
          filename = filenames[i];
      }
      // There are two methods of deleting resources:
      if (resource_add_name_to_deleted_ones_if_standard_template(filename)) {
        // 1. A template that comes with bibledit: We can't delete this as we don't 
        // have write access to the folder where they are stored. Therefore 
        // we "delete" them by placing them in a file that lists deleted
        // resources.
      } else {
        // 2. A user-generated resource: Just delete it.
        ustring directory = gw_path_get_dirname(filename);
        unix_rmdir(directory);
      }
    }
  }
}

bool resource_add_name_to_deleted_ones_if_standard_template(const ustring& filename)
/*
 If the filename given is a standard template, it adds the name of the resource
 to the list of deleted ones and then returns true.
 */
{
  ustring directory = gw_path_get_dirname(filename);
  if (directory == directories_get_package_data()) {
    ustring deleted_filename = gw_build_filename(directories_get_resources(), "deleted");
    ReadText rt(deleted_filename, true, true);
    ustring title = resource_get_title(filename);
    rt.lines.push_back(title);
    write_lines(deleted_filename, rt.lines);
    return true;
  }
  return false;
}

vector <ustring> resource_get_resources(vector <ustring>& filenames, bool list_deleted_ones) {
  // Storage.
  vector <ustring> resources;
  filenames.clear();

  // Read the user's templates.
  ReadDirectories rd(directories_get_resources(), "", "");
  for (unsigned int i = 0; i < rd.directories.size(); i++) {
    ustring filename = gw_build_filename(directories_get_resources(), rd.directories[i], "resource-template.ini");
    if (g_file_test(filename.c_str(), G_FILE_TEST_IS_REGULAR)) {
      filenames.push_back(filename);
      resources.push_back(resource_get_title(filename));
    }
  }

  // Make a list of "deleted" resources and add to it the unwanted ones.
  // Add the user's resources to these too in order that the user's resources
  // override any templates named the same.
  ustring deleted_filename;
  if (!list_deleted_ones)
    deleted_filename = gw_build_filename(directories_get_resources(), "deleted");
  ReadText rt(deleted_filename, true, true);
  set <ustring> unwanted_ones(rt.lines.begin(), rt.lines.end());
  for (unsigned int i = 0; i < resources.size(); i++) {
    unwanted_ones.insert(resources[i]);
  }

  // Add some standard templates to the "deleted" resource. Reason:
  // Some standard templates have been added that come with Bibledit.
  // These templates reference material that is not available by default,
  // but requires user action to install that. To facilitate easy creation
  // if these resources by the user, some templates have been added. These
  // templates are not for display, but only to base a new template upon.
  // Therefore these templates should not normally be visible, only when used
  // to create a new resource based upon these.
  if (!list_deleted_ones) {
    unwanted_ones.insert("NetBible");
  }

  // Read the templates that come with Bibledit.
  ReadFiles rf(directories_get_package_data(), "resource", ".ini");
  for (unsigned int i = 0; i < rf.files.size(); i++) {
    ustring filename = gw_build_filename(directories_get_package_data(), rf.files[i]);
    ustring title = resource_get_title(filename);
    if (unwanted_ones.find(title) != unwanted_ones.end())
      continue;
    filenames.push_back(filename);
    resources.push_back(title);
  }

  // Sort them.
  quick_sort(resources, filenames, 0, resources.size());

  // Give resources.
  return resources;
}

ResourceType resource_get_type(const ustring& templatefile) {
  ResourceType type = rtEnd;
  GKeyFile * keyfile = g_key_file_new();
  if (g_key_file_load_from_file(keyfile, templatefile.c_str(), G_KEY_FILE_NONE, NULL)) {
    type = (ResourceType) g_key_file_get_integer(keyfile, resource_template_general_group(), resource_template_type_key(), NULL);
    g_key_file_free(keyfile);
  }
  return type;
}

ustring resource_get_title(const ustring& templatefile) {
  ustring title;
  GKeyFile * keyfile = g_key_file_new();
  if (g_key_file_load_from_file(keyfile, templatefile.c_str(), G_KEY_FILE_NONE, NULL)) {
    gchar * value;
    value = g_key_file_get_string(keyfile, resource_template_general_group(), resource_template_title_key(), NULL);
    if (value) {
      title = value;
      g_free(value);
    }
    g_key_file_free(keyfile);
  }
  return title;
}

ustring resource_get_home_page(const ustring& templatefile) {
  ustring home_page;
  GKeyFile * keyfile = g_key_file_new();
  if (g_key_file_load_from_file(keyfile, templatefile.c_str(), G_KEY_FILE_NONE, NULL)) {
    gchar * value;
    value = g_key_file_get_string(keyfile, resource_template_general_group(), resource_template_home_page_key(), NULL);
    if (value) {
      home_page = value;
      g_free(value);
    }
    g_key_file_free(keyfile);
  }
  return home_page;
}

ustring resource_get_url_constructor(const ustring& templatefile) {
  ustring url_constructor;
  GKeyFile * keyfile = g_key_file_new();
  if (g_key_file_load_from_file(keyfile, templatefile.c_str(), G_KEY_FILE_NONE, NULL)) {
    gchar * value;
    value = g_key_file_get_string(keyfile, resource_template_general_group(), resource_template_url_constructor_key(), NULL);
    if (value) {
      url_constructor = value;
      g_free(value);
    }
    g_key_file_free(keyfile);
  }
  return url_constructor;
}

gchar * resource_type_to_text(ResourceType type) {
  switch (type)
  {
    case rtForeignDataURLForEachVerse:
      return "Data lives somewhere else, retrieve a different URL for each verse";
    case rtURLForEachVerseAboveURLFilterBelowWithDifferentAnchors:
      return "Retrieve a different URL for each chapter, above, and use a filter for below, and use different anchors";
    case rtURLForEachVerse:
      return "Retrieve a different URL for each verse";
    case rtEnd:
      return "";
  }
  return "";
}

ResourceType resource_text_to_type(const ustring& text) {
  for (unsigned int i = 0; i < rtEnd; i++) {
    if (text == resource_type_to_text(ResourceType(i)))
      return (ResourceType) i;
  }
  return rtEnd;
}

gchar * resource_url_constructor_book() {
  return "<book>";
}

gchar * resource_url_constructor_book_anchor() {
  return "<bookanchor>";
}

gchar * resource_url_constructor_chapter() {
  return "<chapter>";
}

gchar * resource_url_constructor_verse() {
  return "<verse>";
}

gchar * resource_template_general_group() {
  return "general";
}

gchar * resource_template_books_group() {
  return "books";
}

gchar * resource_template_type_key() {
  return "type";
}

gchar * resource_template_title_key() {
  return "title";
}

gchar * resource_template_home_page_key() {
  return "home page";
}

gchar * resource_template_url_constructor_key() {
  return "url constructor";
}

map <unsigned int, ustring> resource_get_books(const ustring& templatefile) {
  map <unsigned int, ustring> books;
  GKeyFile * keyfile = g_key_file_new();
  if (g_key_file_load_from_file(keyfile, templatefile.c_str(), G_KEY_FILE_NONE, NULL)) {
    vector <unsigned int> ids = books_type_to_ids(btUnknown);
    for (unsigned int i = 0; i < ids.size(); i++) {
      ustring english_name = books_id_to_english(ids[i]);
      gchar * value;
      value = g_key_file_get_string(keyfile, resource_template_books_group(), english_name.c_str(), NULL);
      if (value) {
        books [i] = value;
        g_free(value);
      }
    }
    g_key_file_free(keyfile);
  }
  return books;
}

ustring resource_construct_url(const ustring& constructor, map <unsigned int, ustring>& books, const Reference& reference) {
  return resource_construct_url(constructor, books, reference, "");
}

ustring resource_construct_url(const ustring& constructor, map <unsigned int, ustring>& books, const Reference& reference, const ustring& workingdirectory) {
  unsigned int book = reference.book;
  unsigned int chapter = reference.chapter;
  if (chapter == 0)
    chapter = 1;
  ustring verse = number_in_string(reference.verse);
  if (verse == "0")
    verse = "1";
  ustring url(constructor);
  replace_text(url, resource_url_constructor_book(), books [book]);
  replace_text(url, resource_url_constructor_chapter(), convert_to_string(chapter));
  replace_text(url, resource_url_constructor_verse(), verse);
  if (!workingdirectory.empty())
    url = gw_build_filename(workingdirectory, url);
  return url;
}

ustring resource_construct_url(const ustring& constructor, map <unsigned int, ustring>& books, map <unsigned int, ustring>& anchors, const Reference& reference) {
  return resource_construct_url(constructor, books, anchors, reference, "");
}

ustring resource_construct_url(const ustring& constructor, map <unsigned int, ustring>& books, map <unsigned int, ustring>& anchors, const Reference& reference, const ustring& workingdirectory) {
  unsigned int book = reference.book;
  unsigned int chapter = reference.chapter;
  if (chapter == 0)
    chapter = 1;
  ustring verse = number_in_string(reference.verse);
  if (verse == "0")
    verse = "1";
  ustring url(constructor);
  replace_text(url, resource_url_constructor_book(), books [book]);
  replace_text(url, resource_url_constructor_book_anchor(), anchors [book]);
  replace_text(url, resource_url_constructor_chapter(), convert_to_string(chapter));
  replace_text(url, resource_url_constructor_verse(), verse);
  if (!workingdirectory.empty())
    url = gw_build_filename(workingdirectory, url);
  return url;
}

ustring resource_construct_index_file(const ustring& workingdirectory, ustring constructor, map <unsigned int, ustring>& books, const Reference& reference, bool vary_filename) {
  ustring filename("bibledit-index-on-fly.html");
  if (vary_filename) {
    Reference ref(reference);
    ustring prefix = ref.human_readable("");
    filename.insert(0, prefix);
  }
  filename = gw_build_filename(workingdirectory, filename);
  unsigned int book = reference.book;
  unsigned int chapter = reference.chapter;
  if (chapter == 0)
    chapter = 1;
  ustring verse = number_in_string(reference.verse);
  if (verse == "0")
    verse = "1";
  replace_text(constructor, resource_url_constructor_book(), books [book]);
  replace_text(constructor, resource_url_constructor_chapter(), convert_to_string(chapter));
  replace_text(constructor, resource_url_constructor_verse(), verse);
  g_file_set_contents(filename.c_str(), constructor.c_str(), -1, NULL);
  return filename;
}

ustring resource_construct_index_file(const ustring& workingdirectory, ustring constructor, map <unsigned int, ustring>& books, map <unsigned int, ustring>& anchors, const Reference& reference, bool vary_filename) {
  ustring filename("bibledit-index-on-fly.html");
  if (vary_filename) {
    Reference ref(reference);
    ustring prefix = ref.human_readable("");
    filename.insert(0, prefix);
  }
  filename = gw_build_filename(workingdirectory, filename);
  unsigned int book = reference.book;
  unsigned int chapter = reference.chapter;
  if (chapter == 0)
    chapter = 1;
  ustring verse = number_in_string(reference.verse);
  if (verse == "0")
    verse = "1";
  replace_text(constructor, resource_url_constructor_book(), books [book]);
  replace_text(constructor, resource_url_constructor_book_anchor(), anchors [book]);
  replace_text(constructor, resource_url_constructor_chapter(), convert_to_string(chapter));
  replace_text(constructor, resource_url_constructor_verse(), verse);
  g_file_set_contents(filename.c_str(), constructor.c_str(), -1, NULL);
  return filename;
}

gchar * resource_template_index_file_constructor_key() {
  return "index file constructor";
}

ustring resource_get_index_file_constructor(const ustring& templatefile) {
  ustring index_file_constructor;
  GKeyFile * keyfile = g_key_file_new();
  if (g_key_file_load_from_file(keyfile, templatefile.c_str(), G_KEY_FILE_NONE, NULL)) {
    gchar * value;
    value = g_key_file_get_string(keyfile, resource_template_general_group(), resource_template_index_file_constructor_key(), NULL);
    if (value) {
      index_file_constructor = value;
      g_free(value);
    }
    g_key_file_free(keyfile);
  }
  return index_file_constructor;
}

map <unsigned int, ustring> resource_get_anchors(const ustring& templatefile) {
  map <unsigned int, ustring> anchors;
  GKeyFile * keyfile = g_key_file_new();
  if (g_key_file_load_from_file(keyfile, templatefile.c_str(), G_KEY_FILE_NONE, NULL)) {
    vector <unsigned int> ids = books_type_to_ids(btUnknown);
    for (unsigned int i = 0; i < ids.size(); i++) {
      ustring english_name = books_id_to_english(ids[i]);
      gchar * value;
      value = g_key_file_get_string(keyfile, resource_template_anchors_group(), english_name.c_str(), NULL);
      if (value) {
        anchors [i] = value;
        g_free(value);
      }
    }
    g_key_file_free(keyfile);
  }
  return anchors;
}

gchar * resource_template_anchors_group() {
  return "anchors";
}

gchar * resource_file_prefix() {
  return "file://";
}

ustring resource_url_modifier(const ustring& url, ResourceType resource_type, const ustring& templatefile)
/*
 Some urls are given as full ones, e.g. http://somesite.org.
 These don't need any modification.
 Other urls are given as plain filenames only. It is assumed for these that 
 they are given relative to the resource directory where these reside. 
 These need to be modified so as to include the full path and the file:// prefix.
 */
{
  ustring modified_url(url);
  switch (resource_type)
  {
    case rtForeignDataURLForEachVerse:
    {
      break;
    }
    case rtURLForEachVerseAboveURLFilterBelowWithDifferentAnchors:
    case rtURLForEachVerse:
    {
      ustring path = gw_path_get_dirname(templatefile);
      modified_url = resource_file_prefix();
      modified_url.append(gw_build_filename(path, url));
      break;
    }
    case rtEnd:
    {
      break;
    }
  }
  return modified_url;
}
