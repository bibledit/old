<?php
/*
Copyright (©) 2003-2015 Teus Benschop.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

require_once ("../bootstrap/bootstrap");
page_access_level (Filter_Roles::manager ());
Assets_Page::header (gettext("Import"));
$view = new Assets_View (__FILE__);
$view.set_variable ("post_max_size = ini_get ("post_max_size");
$view.set_variable ("upload_max_filesize = ini_get ("upload_max_filesize");
/*
Other variables of importance are:
file_uploads
max_input_time
memory_limit
max_execution_time
For just now these are left as they are.
*/
$view->render ("import1");
Assets_Page::footer ();

?>
