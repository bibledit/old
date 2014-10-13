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


@$referer = $_SERVER ["HTTP_REFERER"];
@$referer = basename ($referer);
if ($referer == "setup") $referer = "index.php";


if (isset ($_GET ['referer'])) {
  $referer = $_GET ['referer'] . ".php";
}


$steps = array (
  "index.php",
  "writable1.php",
  "writable2.php",
  "libraries.php",
  "users1.php",
  "users2.php",
  "mail1.php",
  "mail2.php",
  "confirm1.php",
  "confirm2.php",
  "logs1.php",
  "logs2.php",
  "books1.php",
  "books2.php",
  "versifications1.php",
  "versifications2.php",
  "styles1.php",
  "styles2.php",
  "notes1.php",
  "notes2.php",
  "noteactions1.php",
  "noteactions2.php",
  "shell1.php",
  "shell2.php",
  "check1.php",
  "check2.php",
  "volatile1.php",
  "volatile2.php",
  "sprint1.php",
  "sprint2.php",
  "navigation1.php",
  "navigation2.php",
  "history1.php",
  "history2.php",
  "commits1.php",
  "commits2.php",
  "jobs1.php",
  "jobs2.php",
  "search1.php",
  "search2.php",
  "mappings1.php",
  "mappings2.php",
  "bibleactions1.php",
  "bibleactions2.php",
  "modifications1.php",
  "modifications2.php",
  "databases1.php",
  "databases2.php",
  "client1.php",
  "client2.php",
  "details.php",
  "binaries.php",
  "timer.php",
  "crontab.php",
  "apache.php",
  "write.php",
  "ready.php",
);


$key = array_search ($referer, $steps);
if (is_numeric ($key)) {
  $key++;
} else {
  $key = 0;
}


@$location = $steps [$key];
if (!$location) $location = "..";


include ("../filter/url.php");
Filter_Url::redirect ("$location");


?>
