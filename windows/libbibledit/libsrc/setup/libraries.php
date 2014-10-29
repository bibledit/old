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


$extensions = get_loaded_extensions ();


// Multi-byte string support.
$mbstring = in_array ("mbstring", $extensions);


// JSON support.
$json = in_array ("json", $extensions);


// DOMDocument support.
$dom = in_array ("dom", $extensions);


// Posix support.
$posix = in_array ("posix", $extensions);


// Tidy support. 
// Bibledit-Web used to required the PHP Tidy extension.
// But this extension was not available on reputable Android PHP servers.
// The software was updated with a Tidy_Html filter that replaces the "tidy" functionality.


// SQLite support.
$sqlite = in_array ("pdo_sqlite", $extensions);


// Iconv support: Bibledit does not need it. Some Zend functions use it.
// $iconv = in_array ("iconv", $extensions);


// XML support.
$xmlreader = in_array ("xmlreader", $extensions);
$xmlwriter = in_array ("xmlwriter", $extensions);
$simplexml = in_array ("SimpleXML", $extensions);


/*
While writing this, Zend Framework 1 works well enough.
In the future it may be needed to switch to Zend Framework 2.
This would mean that Zend Framework 1 is no longer supported.
*/


if ($mbstring && $json && $dom && $posix && $sqlite && $xmlreader && $xmlwriter && $simplexml) {
  include ("../filter/url.php");
  Filter_Url::redirect ("step.php?referer=libraries");
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
<p>Bibledit-Web has tried to locate the libraries it relies on, and has found problems.</p>
<ul>
  <strong>
    <?php if (!$json) { ?>
      <li>Bibledit-Web cannot find PHP JSON support. Install package <code>php-json</code> or <code>php5-json</code>.</li>
    <?php } ?>
    <?php if (!$mbstring) { ?>
      <li>Bibledit-Web cannot find PHP Multibyte String support. Install package <code>php-mbstring</code> or <code>php5-mbstring</code>.</li>
    <?php } ?>
    <?php if (!$dom) { ?>
      <li>Bibledit-Web cannot find PHP DOMDocument support. Install package <code>php-xml</code> or <code>php5-xml</code>.</li>
    <?php } ?>
    <?php if (!$posix) { ?>
      <li>Bibledit-Web cannot find PHP POSIX support. Install package <code>php-posix</code> or <code>php5-posix</code>.</li>
    <?php } ?>
    <?php if (!$sqlite) { ?>
      <li>
        Bibledit-Web cannot find PHP PDO SQLite support. 
        Install package <code>php-pdo_sqlite</code> 
        or <code>php5-pdo_sqlite</code> 
        or <code>php-sqlite</code> 
        or <code>php5-sqlite</code> 
        or <code>php-sqlite3</code> 
        or <code>php5-sqlite3</code>.
      </li>
    <?php } ?>
    <?php if (!$xmlreader) { ?>
      <li>Bibledit-Web cannot find PHP XML reader support. Install package <code>php-xmlreader</code> or <code>php5-xmlreader</code>.</li>
    <?php } ?>
    <?php if (!$xmlwriter) { ?>
      <li>Bibledit-Web cannot find PHP XML writer support. Install package <code>php-xmlwriter</code> or <code>php5-xmlwriter</code>.</li>
    <?php } ?>
    <?php if (!$simplexml) { ?>
      <li>Bibledit-Web cannot find PHP Simple XML support. Install package <code>php-simplexml</code> or <code>php5-simplexml</code>.</li>
    <?php } ?>
  </strong>
</ul>
<p>Please install the missing libraries. Your hosting provider will be able to advise what to do with regard to the problems mentioned.</p>
<p><a href="libraries.php" class="button button-large">Retry</a></p>
</body>
</html>
