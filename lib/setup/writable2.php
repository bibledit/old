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


include ("utils.php");


// Check that the setup folder, plus the files in it, are writable.
$folder = realpath ("..");
$folders = array ();
foreach (new DirectoryIterator ($folder) as $fileInfo) {
  if ($fileInfo->isDot ()) continue;
  if ($fileInfo->isDir ()) {
    $basename = $fileInfo->getFilename();
    $filename = "$folder/$basename";
    if (!is_writable ($filename)) {
      $folders [] = $filename;
    }
  }
}

$writable = count ($folders) == 0;

if (count ($folders) > 5) {
  $folders = array_slice ($folders, 0, 5);
  $folders [] = "and so on ...";
}

display_header ($writable);
if ($writable) {
  display_okay ();
  display_paragraph ("Write access okay.");
} else {
  $message = "Bibledit-Web needs write access to the following folders and the files in them:";
  display_paragraph ($message);
  foreach ($folders as $folder) {
    display_paragraph ("<code>$folder</code>");
  }
  display_paragraph ("Provide write access, or ask your hosting provider to do that for you.");
  open_paragraph ();
  display_link ("writable1.php", "Retry");
  close_paragraph ();
}
display_footer ();


?>
