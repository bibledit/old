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


class Filter_Sql
{


  public static function notesSelectIdentifier ()
  {
    return " SELECT identifier ";
  }


  public static function notesOptionalFulltextSearchRelevanceStatement ($search)
  {
    if ($search == "") return;

    $search = str_replace (",", "", $search);
    $search = Database_SQLiteInjection::no ($search);

    $query = "";

    return $query;
  }


  public static function notesFromWhereStatement ()
  {
    return " FROM notes WHERE 1 ";
  }


  public static function notesOptionalFulltextSearchStatement ($search)
  {
    if ($search == "") return;
    $search = str_replace (",", "", $search);
    $search = Database_SQLiteInjection::no ($search);
    $query = " AND cleantext LIKE '%$search%' ";
    return $query;
  }


  public static function notesOrderByRelevanceStatement ()
  {
    return "";
  }


}


/*

On Ubuntu the server runs with a character set of latin1.

Enable full-text search on character sets other than latin1.
Create a script that moves the server from latin1 to utf8.
Create a script that moves the server from utf8 to latin1.
Try them on localhost first with Greek and Hebrew texts to see if it works.

It is recommended to set the utf8 for the server for the active connection, rather than for the entire
server globally, as this may affect other services running on the same MySQL server.

Make utf8 permanent in the bibledit-web software.
Install software on production server.
Run the scripts on the production server.

Enable MySQL general log:
gedit /etc/mysql/my.cnf
general_log_file        = /var/log/mysql/mysql.log
general_log             = 1

82 Query	SET CHARACTER SET 'utf8'
82 Query	SET collation_connection = 'utf8_general_ci'

SET NAMES 'utf8' ;
SHOW VARIABLES LIKE 'coll%' ;
SHOW VARIABLES LIKE 'char%';

set global character_set_server = utf8;
SHOW VARIABLES LIKE 'char%';

set global character_set_server = utf8;
SELECT cleantext FROM notes WHERE cleantext LIKE '%ἐγὼ%';

set global character_set_server = utf8;

What are the implications for changing the character set of the server from latin to utf8?
Probably all data has to be updated.
Looks like we need to update the data once.

*/


?>
