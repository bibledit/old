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
ini_set ('display_errors', '1');


function display_header ($redirect)
{
?>
<!DOCTYPE html>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<?php if ($redirect) { ?>
<meta http-equiv="refresh" content="1;url=step.php">
<?php } ?>
<title>Bibledit-Web Installation</title>
<link rel="stylesheet" href="stylesheet.css" type="text/css" />
</head>
<body>
<h1><img src="../assets/bibledit.png"> <a href="http://bibledit.org/">Bibledit-Web</a></h1>
<?php
}


function display_footer ()
{
?>
</body>
</html>
<?php
}


function display_paragraph ($content)
{
  open_paragraph ();
  echo $content;
  close_paragraph ();
}


function open_paragraph ()
{
  echo "<p>\n";
}


function close_paragraph ()
{
  echo "</p>\n";
}


function display_link ($href, $text)
{
  echo '  <a href="' . $href . '" class="button button-large">' . $text . '</a>' . "\n";
}


function display_active ()
{
?>
<p><img src="../pix/loading.gif"></p>
<?php
}


function display_okay ()
{
?>
<p><img src="../pix/check.png"></p>
<?php
}
