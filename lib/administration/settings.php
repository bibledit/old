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


// This script is called on a client device and makes settings for Bibledit-Web.
// It can be called from localhost only for security reasons.
if ($_SERVER ['SERVER_ADDR'] != $_SERVER ['REMOTE_ADDR']) die;


require_once ("../bootstrap/bootstrap.php");


if (Filter_Cli::not_yet_ready ()) die;


$database_config_general = Database_Config_General::getInstance ();


// Set the system's time zone.
// Bibledit for Android calls this function.
@$timezone = $_GET ['1'];
if (isset ($timezone)) {
  $timezones = Filter_Datetime::timezones ();
  if (in_array ($timezone, $timezones)) {
    $database_config_general->setTimezone ($timezone);
  }
}


// Set the system's time zone from Windows.
// The UTC offset is given in minutes positive or negative.
@$utc_offset_minutes = $_GET ['utcoffset'];
if (isset ($utc_offset_minutes)) {
  $timezone = timezone_name_from_abbr ("", $utc_offset_minutes * 60, 0);
  if ($timezone) {
    $database_config_general->setTimezone ($timezone);
  }
}


?>
