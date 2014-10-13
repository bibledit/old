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
page_access_level (Filter_Roles::CONSULTANT_LEVEL);
$ipc_notes = Ipc_Notes::getInstance();
$action = $_GET ['action'];
if ($action == "alive") {
  $ipc_notes->alive (true);
  $identifier = $ipc_notes->get ();
  if ($identifier) {
    $ipc_notes->delete ();
    $url = "note.php?id=$identifier";
    echo $url;
  }
} else if ($action == "unload") {
  $ipc_notes->alive (false);
}
?>
