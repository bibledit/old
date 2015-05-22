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

require_once ("../bootstrap/bootstrap.php");

$crontab = new Filter_Which ("crontab");
$crontab = $crontab->available;

$php = new Filter_Which ("php");
$php = $php->available;

if ($crontab && $php) {
  include ("../filter/url.php");
  Filter_Url::redirect ("step.php?referer=binaries");
  die;
}

$binaries = dirname (__FILE__) . "/../binaries";
$binaries = realpath ($binaries);

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
<p>Bibledit-Web has tried to access the programs it relies on, and has found problems.</p>
<ul>
  <strong>
    <?php if (!$crontab) { ?>
      <li>Bibledit-Web cannot find program <code>crontab</code>. It is needed for scheduled tasks.</li>
    <?php } ?>
    <?php if (!$php) { ?>
      <li>Bibledit-Web cannot find the <code>CLI</code> version of program <code>php</code>. It is needed for running time-consuming tasks.</li>
    <?php } ?>
  </strong>
</ul>
<p>Please install the missing programs. Your hosting provider will be able to advise what to do with regard to the problems mentioned.</p>
<p><a href="binaries.php" class="button button-large">Retry</a></p>
</body>
</html>
