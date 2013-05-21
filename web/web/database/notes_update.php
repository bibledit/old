<?php
/**
* @package bibledit
*/
/*
 ** Copyright (©) 2003-2013 Teus Benschop.
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


if (php_sapi_name () != "cli") {
  die;
}


$database_notes = Database_Notes::getInstance ();
$identifiers = $database_notes->getIdentifiers ();
echo "Will update columns cleantext and reversedtext in table notes\n";
echo "Note count: " . count ($identifiers) . "\n";
foreach ($identifiers as $identifier) {
  echo "Note $identifier\n";
  $database_notes->updateSearchFields ($identifier);
}
echo "Total notes done: " . count ($identifiers) . "\n";


?>
