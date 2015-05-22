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


// Get the message parameters from the query.
$user = $_GET['user'];
$channel = $_GET['channel'];
$id = $_GET['id'];
if (!isset ($id)) $id = 0;


// Bibledit-* clients don't include the 'focus' command.
// So if no command is included, assume it's the 'focus' command that's intended.
@$command = $_GET ['command'];
if (!isset ($command)) $command = "focus";



$database_ipc = Database_Ipc::getInstance();


// This routine has a timeout so that, even if the focused reference does not
// change, it still will pass a message to the caller.
// The reason of this is that some callers' code time out on a web request,
// if there is no answer for a long time.
// Rather than to fix the different types of code, it is quicker to just
// prevent timeout by passing a message after some time.
$counter = 0;
@$values = "";
while (!is_array ($values)) {
  sleep (1);
  clearstatcache ();
  $counter++;
  if ($counter > 600) $id--;
  $values = $database_ipc->retrieveMessage ($id, $user, $channel, $command);
}


$id      = $values['id'];
$channel = $values['channel'];
$command = $values['command'];
$message = $values['message'];


if (is_int ($id)) {
  if ($channel != "") {
    $database_ipc->deleteMessage ($id);
  }
}


echo "$id\n";
echo "$command\n";
echo "$message\n";


?>
