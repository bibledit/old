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


#ifndef INCLUDED_RESOURCE_UTILS_H
#define INCLUDED_RESOURCE_UTILS_H


#include "libraries.h"
#include <glib.h>
#include "reference.h"
#include "types.h"


ustring resource_template_ini ();

ustring resource_viewer_produce_anchor(unsigned int book, unsigned int chapter, unsigned int verse);

bool resource_add_name_to_deleted_ones_if_standard_template(const ustring& filename);

vector <ustring> resource_get_resources(vector <ustring>& filenames, bool list_deleted_ones);

const gchar * resource_url_constructor_book();
const gchar * resource_url_constructor_book2();
const gchar * resource_url_constructor_chapter();
const gchar * resource_url_constructor_verse();
const gchar * resource_file_prefix();
ustring resource_url_get(const ustring& url, const ustring& templatefile);
ustring resource_url_enter_reference(const ustring& constructor, map <unsigned int, ustring>& books, map <unsigned int, ustring>& books2, const Reference& reference);

const gchar * resource_template_general_group();
const gchar * resource_template_title_key();
ustring resource_get_title(const ustring& templatefile);
const gchar * resource_template_home_page_key();
ustring resource_get_home_page(const ustring& templatefile);
const gchar * resource_template_url_constructor_key();
ustring resource_get_url_constructor(const ustring& templatefile);

const gchar * resource_template_books_group();
map <unsigned int, ustring> resource_get_books(const ustring& templatefile);

const gchar * resource_template_books2_group();
map <unsigned int, ustring> resource_get_books2(const ustring& templatefile);

ustring resource_select (ustring * filename);


#endif
