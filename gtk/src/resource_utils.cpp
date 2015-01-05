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


#include "resource_utils.h"
#include "directories.h"
#include "gwrappers.h"
#include "unixwrappers.h"
#include "utilities.h"
#include "books.h"
#include "settings.h"
#include "html.h"
#include "tiny_utilities.h"
#include "dialoglistview.h"
#include "gtkwrappers.h"


ustring resource_viewer_directory()
{
  return gw_build_filename(Directories->get_temp(), "resource_viewer");
}

void resource_viewer_ensure_directory()
// Ensures that the working directory for the resource viewer is there.
{
  ustring directory = resource_viewer_directory();
  if (!g_file_test(directory.c_str(), G_FILE_TEST_IS_DIR)) {
    gw_mkdir_with_parents(directory);
  }
}

ustring resource_viewer_reference_html_filename()
{
  ustring filename("bibledit_resource_viewer_reference.html");
  return filename;
}

ustring resource_viewer_main_html_filename()
{
  ustring filename("bibledit_resource_viewer_main.html");
  return filename;
}

ustring resource_template_ini ()
{
  return "resource-template.ini";
}

ustring resource_viewer_produce_anchor(unsigned int book, unsigned int chapter, unsigned int verse)
// Produces the anchor, e.g.: Matthew_1_1 or Song_of_Solomon_2_2.
{
  ustring anchor(books_id_to_english(book) + "_" + convert_to_string(chapter) + "_" + convert_to_string(verse));
  replace_text(anchor, " ", "_");
  return anchor;
}

bool resource_add_name_to_deleted_ones_if_standard_template(const ustring & filename)
/*
 If the filename given is a standard template, it adds the name of the resource
 to the list of deleted ones and then returns true.
 */
{
  ustring directory = gw_path_get_dirname(filename);
  if (directory == Directories->get_package_data()) {
    ustring deleted_filename = gw_build_filename(Directories->get_resources(), "deleted");
    ReadText rt(deleted_filename, true, true);
    ustring title = resource_get_title(filename);
    rt.lines.push_back(title);
    write_lines(deleted_filename, rt.lines);
    return true;
  }
  return false;
}

vector < ustring > resource_get_resources(vector < ustring > &filenames, bool list_deleted_ones)
{
  // Storage.
  vector < ustring > resources;
  filenames.clear();

  // Read the user's templates.
  ReadDirectories rd(Directories->get_resources(), "", "");
  for (unsigned int i = 0; i < rd.directories.size(); i++) {
    ustring filename = gw_build_filename(Directories->get_resources(), rd.directories[i], resource_template_ini ());
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
    deleted_filename = gw_build_filename(Directories->get_resources(), "deleted");
  ReadText rt(deleted_filename, true, true);
  set < ustring > unwanted_ones(rt.lines.begin(), rt.lines.end());
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
  ReadFiles rf(Directories->get_package_data(), "resource", ".ini");
  for (unsigned int i = 0; i < rf.files.size(); i++) {
    ustring filename = gw_build_filename(Directories->get_package_data(), rf.files[i]);
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

ustring resource_get_title(const ustring & templatefile)
{
  ustring title;
  GKeyFile *keyfile = g_key_file_new();
  if (g_key_file_load_from_file(keyfile, templatefile.c_str(), G_KEY_FILE_NONE, NULL)) {
    gchar *value;
    value = g_key_file_get_string(keyfile, resource_template_general_group(), resource_template_title_key(), NULL);
    if (value) {
      title = value;
      g_free(value);
    }
    g_key_file_free(keyfile);
  }
  return title;
}

ustring resource_get_home_page(const ustring & templatefile)
{
  ustring home_page;
  GKeyFile *keyfile = g_key_file_new();
  if (g_key_file_load_from_file(keyfile, templatefile.c_str(), G_KEY_FILE_NONE, NULL)) {
    gchar *value;
    value = g_key_file_get_string(keyfile, resource_template_general_group(), resource_template_home_page_key(), NULL);
    if (value) {
      home_page = value;
      g_free(value);
    }
    g_key_file_free(keyfile);
  }
  return home_page;
}

ustring resource_get_url_constructor(const ustring & templatefile)
{
  ustring url_constructor;
  GKeyFile *keyfile = g_key_file_new();
  if (g_key_file_load_from_file(keyfile, templatefile.c_str(), G_KEY_FILE_NONE, NULL)) {
    gchar *value;
    value = g_key_file_get_string(keyfile, resource_template_general_group(), resource_template_url_constructor_key(), NULL);
    if (value) {
      url_constructor = value;
      g_free(value);
    }
    g_key_file_free(keyfile);
  }
  return url_constructor;
}

const gchar *resource_url_constructor_book()
{
  return "<book>";
}

const gchar *resource_url_constructor_book2()
{
  return "<book2>";
}

const gchar *resource_url_constructor_chapter()
{
  return "<chapter>";
}

const gchar *resource_url_constructor_verse()
{
  return "<verse>";
}

const gchar *resource_template_general_group()
{
  return "general";
}

const gchar *resource_template_books_group()
{
  return "books";
}

const gchar *resource_template_books2_group()
{
  return "books2";
}

const gchar *resource_template_title_key()
{
  return "title";
}

const gchar *resource_template_home_page_key()
{
  return "home page";
}

const gchar *resource_template_url_constructor_key()
{
  return "url constructor";
}

map < unsigned int, ustring > resource_get_books(const ustring & templatefile)
{
  map < unsigned int, ustring > books;
  GKeyFile *keyfile = g_key_file_new();
  if (g_key_file_load_from_file(keyfile, templatefile.c_str(), G_KEY_FILE_NONE, NULL)) {
    vector < unsigned int >ids = books_type_to_ids(btUnknown);
    for (unsigned int i = 0; i < ids.size(); i++) {
      ustring english_name = books_id_to_english(ids[i]);
      gchar *value;
      value = g_key_file_get_string(keyfile, resource_template_books_group(), english_name.c_str(), NULL);
      if (value) {
        books[i] = value;
        g_free(value);
      }
    }
    g_key_file_free(keyfile);
  }
  return books;
}

map < unsigned int, ustring > resource_get_books2(const ustring & templatefile)
{
  map < unsigned int, ustring > books2;
  GKeyFile *keyfile = g_key_file_new();
  if (g_key_file_load_from_file(keyfile, templatefile.c_str(), G_KEY_FILE_NONE, NULL)) {
    vector < unsigned int >ids = books_type_to_ids(btUnknown);
    for (unsigned int i = 0; i < ids.size(); i++) {
      ustring english_name = books_id_to_english(ids[i]);
      gchar *value;
      value = g_key_file_get_string(keyfile, resource_template_books2_group(), english_name.c_str(), NULL);
      if (value) {
        books2[i] = value;
        g_free(value);
      }
    }
    g_key_file_free(keyfile);
  }
  return books2;
}

const gchar *resource_template_anchors_group()
{
  return "anchors";
}

const gchar *resource_file_prefix()
{
  return "file://";
}


ustring resource_url_get(const ustring& url, const ustring& templatefile)
/*
 Some urls are given as full ones, e.g. http://bibledit.org.
 These don't need any modification.
 Other urls are given as plain filenames only. It is assumed for these that 
 they are given relative to the resource directory where these reside. 
 These need to be modified so as to include the full path and the file:// prefix.
 */
{
  ustring modified_url(url);
  if (url.find ("http") != 0) {
    ustring path = gw_path_get_dirname(templatefile);
    modified_url = resource_file_prefix();
    modified_url.append(gw_build_filename(path, url));
  }
  return modified_url;
}


ustring resource_url_enter_reference(const ustring& constructor, map <unsigned int, ustring>& books, map <unsigned int, ustring>& books2, const Reference& reference)
{
  ustring url (constructor);
  replace_text(url, resource_url_constructor_book(), books[reference.book]);
  replace_text(url, resource_url_constructor_book2(), books2[reference.book]);
  replace_text(url, resource_url_constructor_chapter(), convert_to_string(reference.chapter));
  replace_text(url, resource_url_constructor_verse(), number_in_string(reference.verse));
  return url;
}


ustring resource_select (ustring * filename)
// This allows the user to select a resource.
// It returns the name of the resource that was selected.
// filename: If a pointer is passed, it give the filename of the selected resource.
{
  ustring resource;
  vector <ustring> filenames;
  vector <ustring> resources = resource_get_resources(filenames, false);
  quick_sort(resources, filenames, 0, resources.size());
  ListviewDialog dialog("Open resource", resources, "", false, NULL);
  if (dialog.run() == GTK_RESPONSE_OK) {
    resource = dialog.focus;
  }
  if (filename) {
    for (unsigned int i = 0; i < resources.size(); i++) {
      if (resource == resources[i]) {
        * filename = filenames[i];
      }
    }
  }
  return resource;
}
