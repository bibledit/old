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


#ifndef INCLUDED_HTML_H
#define INCLUDED_HTML_H


#include "libraries.h"


ustring html_remove_code_between_less_than_and_greater_than_signs(const ustring& text);
ustring html_create_anchor(const ustring& anchor, const ustring& text);
ustring html_extract_title(const ustring& filename);
void html_entities_to_utf8(ustring& code);
int html_hexit(char c);
void html_url_decode(char *buf);


#endif
