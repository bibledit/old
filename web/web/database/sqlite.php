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


/*
It has been seen on a shared hosting platform that the MySQL server did not have
sufficient available concurrent connections at times.
Other processes were using many connections, so that none remained for Bibledit-Web.
That is a reason against using MySQL on shared hosting.

A reason for using SQLite is that it is easier to set up.
No users, no privileges, no server.

Another reason is that a backup of the web space also back ups the SQLite databases,
provided they are stored in the web space.
With MySQL this is hard to do.

Usually with shared hosting, the web space is big, but the MySQL database space is small.
With a few Bibles, and several notes, plus resources, the MySQL database is full.
SQLite uses the huge web space that usually come with shared hosting.
Therefore it can store much more data with the same shared hosting package.

While MySQL is a faster database than SQLite in isolated experiments, 
on shared hosts it may be different.
The reasons is that on a shared host, SQLite gets the data in the same process 
straight from disk. This works differently for MySQL. In most cases, the shared
host uses a separate database server. Thus the web server fetches its data
from another host through the network. This introduces some delays.
For smaller data sets SQLite is much faster in this scenario.

Some often-used databases at times display database errors:
  disk I/O error
  unable to open database file
It was tried wiether the errors go away when "PRAGMA busy_timeout = 100;"
is executed after opening the database connection. The errors did not go away.

It was tried whether the above errors go away on shared hosting with this command:
PRAGMA journal_mode = TRUNCATE;
The errors did not go away.

It was tried whether the above errors go away on shared hosting with this command:
PRAGMA temp_store = MEMORY;
The errors did not go away.

It was tried whether the above errors go away on shared hosting with this command:
PRAGMA journal_mode = MEMORY;
The errors went away, but the database behaved in an inconsistent way.
It did not keeps its data properly, and did not update it properly.
The same behaviour was found with:
PRAGMA journal_mode = OFF;

It was tried whether setting the environment variable TMPDIR to a directory
in our own web space would improve SQLite, but this did not improve SQLite.
However, in other areas it looked as if this did give improvement.

What made a big difference is this:
1. Changing the database for the logbook from SQLite to the filesystem.
2. Setting the TMPDIR to a folder inside Bibledit's own webspace.
3. Letting the tasks runner not use a SQLite database, but the file system and memory.
The database errors went away.

*/


class Database_SQLite {


  public static function connect ($database)
  {
    $file = dirname (__DIR__) . "/databases/$database.sqlite";
    try {
      $db = new PDO ("sqlite:$file");
    } catch (PDOException $e) {
      $message = $e->getMessage () . " " . $file;
      if (class_exists ("Database_Logs", true)) {
        $database_logs = Database_Logs::getInstance ();
        $database_logs->log ($message, Filter_Roles::ADMIN_LEVEL);
      } else {
        echo "<p>$message</p>";
      }
      if (php_sapi_name () == "cli") {
        echo "$message\n";
      }
    }
    return $db;
  }


  public static function exec ($db, $sql)
  {
    $success = $db->exec ($sql);
    if ($success === false) {
      $message = $db->errorInfo ();
      $message = "Database error " . $message [0] . " - " . $message [2] . " - Query was: " . $sql;
      if (class_exists ("Database_Logs", true)) {
        $database_logs = Database_Logs::getInstance ();
        $database_logs->log ($message, Filter_Roles::ADMIN_LEVEL);
      } else {
        echo "<p>$message</p>";
      }
      if (php_sapi_name () == "cli") {
        echo "$message\n";
      }
    }
  }


  public static function query ($db, $sql)
  {
    // Run the SQL on the database handle.
    $success = $result = $db->query ($sql);
    // Check / log errors.
    if ($success === false) {
      $message = $db->errorInfo ();
      $message = "Database error: " . $message [0] . " - " . $message [2] . " - Query was: " . $sql;
      if (class_exists ("Database_Logs", true)) {
        $database_logs = Database_Logs::getInstance ();
        $database_logs->log ($message, Filter_Roles::ADMIN_LEVEL);
      } else {
        echo "<p>$message</p>";
      }
      if (php_sapi_name () == "cli") {
        echo "$message\n";
      }
    }
    // In case of errors, the $result won't be an array.
    // Make it an ampty array so it won't produce further errors down the code.
    if (!$result) {
      $result = array ();
    }
    return $result;
  }


}


?>
