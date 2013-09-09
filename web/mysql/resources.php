<?php
/*
Copyright (©) 2003-2013 Teus Benschop.

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


// The purpose of this file is to create a file with SQL statements.
// The SQL statements contain the data for the default external resources.

$folder = $argv [1];
$files = array ();
foreach (new DirectoryIterator ($folder) as $fileInfo) {
  if($fileInfo->isDot()) continue;
  if($fileInfo->isDir()) continue;
  $files [] = $fileInfo->getFilename();
}

echo "USE `BIBLEDITDATABASE`; \n";

foreach ($files as $file) {
 
  if (strpos ($file, "default") === false) continue;
  $suffix = pathinfo ($file, PATHINFO_EXTENSION);
  $name = substr ($file, 7, -1 - strlen ($suffix));
  $name = "$name (bibledit)";
  $code = file_get_contents ("$folder/$file");
  $code = addslashes ($code);

  echo "DELETE FROM resources WHERE name = '$name'; \n";
  echo "INSERT INTO resources VALUES ( \n";
  echo "NULL, \n";
  echo "'$name', \n";
  echo "'php script', \n";
  echo "'$code'); \n";
  
}

?>
