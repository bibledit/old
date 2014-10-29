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

$database_logs = Database_Logs::getInstance ();
$database_logs->log (Locale_Translate::_("Import consultation notes has started"), true);

// Security: Page only runs from the cli SAPI.
Filter_Cli::assert ();

$folder = Filter_Cli::argument (@$argv, 1);
$folder = "$folder/data";
$database_logs->log (Locale_Translate::_("Importing from folder:") . " " . $folder, true);

$counter = 0;
foreach (new DirectoryIterator ($folder) as $fileInfo) {
  if($fileInfo->isDot()) continue;
  if($fileInfo->isDir()) continue;
  $counter++;
  $basename = $fileInfo->getFilename();
  $information = Locale_Translate::_("Note") . " " . $basename . " ($counter) ";
  if (is_numeric ($basename)) {
    $path = "$folder/$basename";
    $note_identifier = Filter_Notes::importFromBibleditGtkFile ($path);
    if (is_numeric ($note_identifier)) {
      $database_logs->log ($information . Locale_Translate::_("Imported"), true);
    } else {
      $database_logs->log ($information . Locale_Translate::_("Could not import the note"), true);
    }
  } else {
    $database_logs->log ($information . Locale_Translate::_("This is not a note"), true);
  }
}

$database_logs->log (Locale_Translate::_("Import consultation notes has finished"), true);

?>
