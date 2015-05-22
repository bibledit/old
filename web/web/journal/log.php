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


// This script logs a message in the Journal through the web.


// Skip empty message.
@$msg = $_GET ["msg"];
if ($msg == "") die;


// It can be called from localhost only for security reasons.
if ($_SERVER ['SERVER_ADDR'] != $_SERVER ['REMOTE_ADDR']) die;


require_once ("../bootstrap/bootstrap.php");


if (Filter_Cli::not_yet_ready ()) die;


// Keep pure text only.
$msg = Filter_Html::html2text ($msg);
$msg = trim ($msg);
if ($msg == "") die;


// Log the message.
$database_logs = Database_Logs::getInstance ();
$database_logs->log ($msg);


?>
