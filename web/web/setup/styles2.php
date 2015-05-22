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


ob_start ();


require_once ("../database/sqlite.php");
require_once ("../database/sqliteinjection.php");
require_once ("../database/styles.php");


$database_styles = Database_Styles::getInstance ();
$database_styles->create ();


$okay = !ob_get_flush ();


display_header ($okay);
if ($okay) {
  display_okay ();
  display_paragraph ("Styles database okay.");
} else {
  display_paragraph ("Errors creating or upgrading the styles database");
  open_paragraph ();
  display_link ("styles1.php", "Retry");
  close_paragraph ();
}
display_footer ();


?>
