<?php
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

/*


If certain characters do not get indexed, or cannot be searched on, 
then the solution is to add these characters to sphinx.conf 
to the 'charset_table'.


# Sphinxsearch indexes the data through the configuration file.
indexer --rotate --all --config sphinx.conf


# The search daemon is is started like this:
searchd --config /path/to/sphinx.conf


# Do a test search
search -c /path/to/sphinx.conf -a "Bible"


*/
?>