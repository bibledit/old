<?php
/**
* @package bibledit
*/
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

require_once ("../bootstrap/bootstrap.php");
page_access_level (MANAGER_LEVEL);
Assets_Page::header (gettext ("Import"));
$view = new Assets_View (__FILE__);

// The name of the Bible.
$bible = $_GET['bible'];
$view->view->bible = Filter_Html::sanitize ($bible);

// Move the uploaded file to a temporary name.
$datafile = tempnam (sys_get_temp_dir(), '');
unlink ($datafile);
@$datafile .= $_FILES['data']['name'];
@$tmpfile = $_FILES['data']['tmp_name'];
if(move_uploaded_file($tmpfile, $datafile)) {
  $view->view->filename = $datafile;
  $view->render ("import2.php");
} else {
  $view->render ("import2error.php");
}
Assets_Page::footer ();

?>
