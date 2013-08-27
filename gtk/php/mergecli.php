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


// This calls Bibledit-Web's three-way merge filter from the command line.


// Security: The script runs from the cli SAPI only.
if (php_sapi_name () != "cli") {
  echo "Fatal: This only runs through the cli Server API\n";
  die;
}


if ($argc != 5) {
  echo "Three-way file merge with grapheme granularity.\n";
  echo "It needs the following four arguments:\n";
  echo "- Name of the file with the merge base\n";
  echo "- Name of the file with the user modifications\n";
  echo "- Name of the file with the server modifications\n";
  echo "- Name of the file to store the merged output\n";
  die;
}


$mergeBase = file_get_contents ($argv [1]);
$userModification  = file_get_contents ($argv [2]);
$serverModification = file_get_contents ($argv [3]);


include ("merge.php");
include ("rmdir.php");


$output = Filter_Merge::run ($mergeBase, $userModification, $serverModification);


file_put_contents ($argv [4], $output);


?>
