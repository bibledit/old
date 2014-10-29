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


// Remove the setup files.
$folder = realpath (".");
$iterator = new DirectoryIterator ($folder);
foreach ($iterator as $fileInfo) {
  if ($fileInfo->isDot ()) continue;
  if ($fileInfo->isDir ()) continue;
  unlink ($fileInfo->getFilename());
}
unset ($iterator);
// Changing the mode gives an error on iOS: Suppress it.
@chmod ($folder, 0777);
// Deleting the folder on Windows gives an error: Suppress it.
@rmdir ($folder);


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
<h1>Ready!</h1>
<p>Bibledit-Web has been installed.</p>
<p class="step"><a href="../index.php" class="button button-large">Go to Bibledit-Web</a></p>
</body>
</html>
