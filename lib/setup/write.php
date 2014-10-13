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


error_reporting (E_ALL);


$messages = array ();


// Test write access to the exports folder.
$folder = realpath ("../exports");
if (!is_writable ($folder)) {
  $messages [] = "Bibledit-Web needs write access to folder $folder for exporting Bibles to.";
}


// Test write access to the git folder.
$folder = realpath ("../git");
if (!is_writable ($folder)) {
  $messages [] = "Bibledit-Web needs write access to folder $folder to store git repositories for collaboration.";
}


// Test write access to the revisions folder.
$folder = realpath ("../revisions");
if (!is_writable ($folder)) {
  $messages [] = "Bibledit-Web needs write access to folder $folder to store the daily changes.";
}


// Test write access to the dyncss folder.
$folder = realpath ("../dyncss");
if (!is_writable ($folder)) {
  $messages [] = "Bibledit-Web needs write access to folder $folder to store the dynamically created stylesheets.";
}


if (count ($messages) == 0) {
  include ("../filter/url.php");
  Filter_Url::redirect ("step.php?referer=write");
  die;
}


?>
<!DOCTYPE html>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<title>Bibledit-Web Installation</title>
<link rel="stylesheet" href="stylesheet.css" type="text/css" />
</head>
<body>
<h1><img src="../assets/bibledit.png"> <a href="http://bibledit.org/">Bibledit-Web</a></h1>
<p>Bibledit-Web tried to gain write access to certain locations on the web server, but did not succeed.</p>
<p>More information is below.</p>
<ul>
<?php
foreach ($messages as $message) {
  echo "<li>$message</li>\n";
}
?>
</ul>
<p>Your hosting provider will be able to advise what to do with regard to the problems mentioned.</p>
<p><a href="write.php" class="button button-large">Retry</a></p>
</body>
</html>
