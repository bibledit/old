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


// Outputs a message with information for the user.
// It takes a string or an array of strings.
function message_information ($message) {
  $messages = func_get_args ();
  foreach ($messages as $msg) {
    echo $msg;
    echo "<br>\n";
  }
}


// Outputs a message with a warning for the user.
// It takes a string or an array of strings.
function message_warning_header ($message) {
  $messages = func_get_args ();
  foreach ($messages as $msg) {
    echo "<h1> ", gettext ("Warning"), ": $msg</h1>\n";
  }
}


// Outputs a message with a warning for the user.
// It takes a string or an array of strings.
function message_warning ($message) {
  $messages = func_get_args ();
  foreach ($messages as $msg) {
    echo gettext ("Warning"), ": $msg<br>\n";
  }
}


// Outputs a message with code for the user.
// It takes a string or an array of strings.
function message_code ($message) {
  $messages = func_get_args ();
  foreach ($messages as $msg) {
    echo "<p><code>$msg</code></p>\n";
  }
}


function message_error ($message) {
  $messages = func_get_args ();
  foreach ($messages as $msg) {
    echo "<p class=error>$msg</p>\n";
  }
}


?>
