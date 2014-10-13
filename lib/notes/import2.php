<?php
/*
Copyright (©) 2003-2014 Teus Benschop.

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

require_once ("../bootstrap/bootstrap.php");
page_access_level (Filter_Roles::MANAGER_LEVEL);
Assets_Page::header (Locale_Translate::_("Import"));

// Move the uploaded file to a temporary name.
$notesfile = tempnam (sys_get_temp_dir(), '');
unlink ($notesfile);
@$notesfile .= $_FILES['notes']['name'];
@$tmpfile = $_FILES['notes']['tmp_name'];
$view = new Assets_View (__FILE__);
if(move_uploaded_file($tmpfile, $notesfile)) {
  $view->view->filename = $notesfile;
  $notesfolder = Filter_Archive::uncompress ($notesfile, true);
  $view->view->folder = $notesfolder;
  $view->render ("import2.php");
} else {
  $view->render ("import2error.php");
}
Assets_Page::footer ();

?>
