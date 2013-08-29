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


/*

Note: This object should not call other objects within the Bibledit-Web source,
because this object is also called from Bibledit-Gtk, and does not have access
to those other objects.
Calling other objects would result in faral errors that break Bibledit-Gtk.

*/


// This calls Bibledit-Web's conflict resolution filter from the command line.


// Security: The script runs from the cli SAPI only.
if (php_sapi_name () != "cli") {
  echo "Fatal: This only runs through the cli Server API\n";
  die;
}


if ($argc != 2) {
  echo "Conflict resolver with grapheme granularity.\n";
  echo "It needs the following argument:\n";
  echo "- Path to the git repository with conflicts\n";
  die;
}


include ("conflict.php");
include ("merge.php");
include ("rmdir.php");


$repository = $argv [1];
Filter_Conflict::run ($repository);


?>
