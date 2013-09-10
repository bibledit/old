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


/**
* SQL Injection projection.
*/
class Database_SQLInjection
{
  public static function no ($value) {
    // Slashes that may have been added by the "magic quotes" system will have been removed in bootstrap.php.
    // If we use mysql_real_escape_string, it says, e.g.:
    // Warning: mysql_real_escape_string() [function.mysql-real-escape-string]: Access denied for user 'www-data'@'localhost' (using password: NO)
    //$value = mysql_real_escape_string ($value);
    // Replacement:
    // mysql_real_escape_string() calls MySQL's library function mysql_real_escape_string, 
    // which prepends backslashes to the following characters: 
    // \x00, \n, \r, \, ', " and \x1a.
    //str_replace ("\x00", "\\\x00", $value);
    //str_replace ("\n",   "\\\n",   $value);
    //str_replace ("\r",   "\\\r",   $value);
    //str_replace ("\\",   "\\\\",   $value);
    //str_replace ("'",    "\\'",    $value);
    //str_replace ("\"",   "\\\"",   $value);
    //str_replace ("\x1a", "\\\x1a", $value);
    //$server = Database_Instance::getInstance();
    //$value = mysql_real_escape_string ($value, $server->mysqli);
    $value = addslashes ($value);
    return $value;
  }
}


?>
