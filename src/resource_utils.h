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


#ifndef INCLUDED_RESOURCE_UTILS_H
#define INCLUDED_RESOURCE_UTILS_H


#include "libraries.h"
#include <glib.h>
#include "reference.h"
#include "resourceviewergui.h"
#include "types.h"


ustring resource_viewer_produce_anchor (unsigned int book, unsigned int chapter, unsigned int verse);


void resource_open (ResourcesGUI * resourcesgui);
void resource_close (ResourcesGUI * resourcesgui);
void resource_delete (ResourcesGUI * resourcesgui);

bool resource_add_name_to_deleted_ones_if_standard_template (const ustring& filename);

vector <ustring> resource_get_resources (vector <ustring>& filenames, bool list_deleted_ones);

gchar * resource_url_constructor_book ();
gchar * resource_url_constructor_book_anchor ();
gchar * resource_url_constructor_chapter ();
gchar * resource_url_constructor_verse ();
ustring resource_construct_url (const ustring& constructor, map <unsigned int, ustring>& books, const Reference& reference);
ustring resource_construct_url (const ustring& constructor, map <unsigned int, ustring>& books, const Reference& reference, const ustring& workingdirectory);
ustring resource_construct_index_file (const ustring& workingdirectory, ustring constructor, map <unsigned int, ustring>& books, const Reference& reference, bool vary_filename);
ustring resource_construct_index_file (const ustring& workingdirectory, ustring constructor, map <unsigned int, ustring>& books, map <unsigned int, ustring>& anchors, const Reference& reference, bool vary_filename);
gchar * resource_file_prefix ();
ustring resource_url_modifier (const ustring& url, ResourceType resource_type, const ustring& templatefile);

gchar * resource_template_general_group ();
gchar * resource_template_type_key ();
ResourceType resource_get_type (const ustring& templatefile);
gchar * resource_type_to_text (ResourceType type);
ResourceType resource_text_to_type (const ustring& text);
gchar * resource_template_title_key ();
ustring resource_get_title (const ustring& templatefile);
gchar * resource_template_home_page_key ();
ustring resource_get_home_page (const ustring& templatefile);
gchar * resource_template_url_constructor_key ();
ustring resource_get_url_constructor (const ustring& templatefile);
gchar * resource_template_index_file_constructor_key ();
ustring resource_get_index_file_constructor (const ustring& templatefile);

gchar * resource_template_books_group ();
map <unsigned int, ustring> resource_get_books (const ustring& templatefile);

gchar * resource_template_anchors_group ();
map <unsigned int, ustring> resource_get_anchors (const ustring& templatefile);


#endif
