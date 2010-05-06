<?php
/**
* @package bibledit
*/
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

require_once ("../bootstrap/bootstrap.php");
page_access_level (MANAGER_LEVEL);
Assets_Page::header (gettext ("Import"));
set_time_limit (0);
$folder = $_GET ['folder'];
if (file_exists ($folder)) {
  $folder = "$folder/data";
  Assets_Page::message (gettext ("Checking folder") . " " . $folder);
  foreach (new DirectoryIterator ($folder) as $fileInfo) {
    if($fileInfo->isDot()) continue;
    $basename = $fileInfo->getFilename();
    Assets_Page::message ($basename);
    if (is_numeric ($basename)) {
      $path = "$folder/$basename";
      $note_identifier = Filter_Notes::importFromBibleditGtkFile ($path);
      if (is_numeric ($note_identifier)) {
        Assets_Page::message (gettext ("OK"));
      } else {
        Assets_Page::error (gettext ("Could not import the note"));
      }
    } else {
      Assets_Page::error (gettext ("This is not a note"));
    }
    fflush ();
  }
} else {
  Assets_Page::error (gettext ("Could not find the folder with the consultation notes"));
}
Assets_Page::footer ();

?>
