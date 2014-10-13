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
page_access_level (Filter_Roles::TRANSLATOR_LEVEL);


$ipc_focus = Ipc_Focus::getInstance();
$database_ipc = Database_Ipc::getInstance ();


// Optionally handle a call to the page to set the passage.
@$switchbook = $_GET ['switchbook'];
@$switchchapter = $_GET ['switchchapter'];
@$switchverse = $_GET ['switchverse'];
if (isset ($switchbook)) {
  $switchbook = Filter_Numeric::integer_in_string ($switchbook);
  if (!isset ($switchchapter)) $switchchapter = 1;
  $switchchapter = Filter_Numeric::integer_in_string ($switchchapter);
  if (!isset ($switchverse)) $switchverse = 1;
  $switchverse = Filter_Numeric::integer_in_string ($switchverse);
  $ipc_focus->set ($switchbook, $switchchapter, $switchverse);
  Navigation_Passage::recordHistory ($switchbook, $switchchapter, $switchverse);
}


// Check whether a Bible editor is alive.
$timestamp = $database_ipc->getBibleAlive ();
$alive = ($timestamp > (time () - 5));


if ($alive) {
  // If a Bible editor is alive, send javascript to the browser to close this new window.
  $message = Locale_Translate::_("The passage has been opened in the existing Bible editor in another browser tab.");
$script = <<< EOD
<!doctype html>
<html>
<head>
</head>
<body>
<script>
  setTimeout (closeself, 1000);
  function closeself () {
    var win = window.open ("" ,"_self", "");
    win.close();
  }
</script>
<div>$message</div>
</body>
</html>
EOD;
  echo $script;
} else {
  // If no Bible editor is alive, forward the browser to the Bible editor.
  Filter_Url::redirect ("index.php");
}


?>
