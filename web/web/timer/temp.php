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


// Security: Page only runs from the cli SAPI.
Filter_Cli::assert ();


$database_logs = Database_Logs::getInstance ();
$database_logs->log ("Removing expired temporal files", Filter_Roles::ADMIN_LEVEL);


$expired = strtotime ("-3 days");
$directory = dirname (__DIR__) . "/tmp";


$names = scandir ($directory);
$names = Filter_Folders::cleanup ($names);


foreach ($names as $name) {
  $filename = "$directory/$name";
  $mtime = filemtime ($filename);
  if ($mtime < $expired) {
    if (is_file ($filename)) unlink ($filename);
    if (is_dir ($filename)) Filter_Rmdir::rmdir ($filename);
  }
}


?>
