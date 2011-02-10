<?php
/**
* @package bibledit
*/
/*
 ** Copyright (©) 2003-2009 Teus Benschop.
 **
 ** This program is free software; you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation; either version 3 of the License, or
 ** (at your option) any later version.
 **  
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **  
 ** You should have received a copy of the GNU General Public License
 ** along with this program; if not, write to the Free Software
 ** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 **  
 */

require_once ("../bootstrap/bootstrap.php");
page_access_level (ADMIN_LEVEL);
Assets_Page::header (gettext ("Software update"));

$smarty = new Smarty_Bibledit (__FILE__);

$success = true;
$errormessage = "";

$url = $_POST ['uri'];
$smarty->assign ("url", $url);

// Check on minimum length of url: http://x.y/bibledit-web-x.x.tar.gz
if (strlen ($url) < 34) {
  $success = false;
  $errormessage = gettext ("The URL is too short.");
}

// Check that the basename looks like bibledit-web-x.x.tar.gz.
$basename = basename ($url);
$smarty->assign ("basename", $basename);
if ($success) {
  $pos = strpos ($basename, "bibledit-web-");
  if ($pos !== 0) {
    $success = false;
    $errormessage = gettext ("The filename does not look like Bibledit-Web software.");
  } else {
    $basename = substr ($basename, 13);
  }
}
if ($success) {
  $pos = strpos ($basename, ".tar.gz");
  if ($pos === false) {
    $success = false;
    $errormessage = gettext ("The filename does not look like a Bibledit-Web tarball.");
  } else {
    $basename = substr ($basename, 0, -7);
  }
}

// Check the version number.
include ("version.php");
if ($success) {
  if (version_compare ($basename, $version) === -1) {
    $success = false;
    $errormessage = gettext ("It does not downgrade Bibledit-Web to an older version.");
  }
}

// Get the file's contents.
if ($success) {
  $contents = file_get_contents ($url);
  if ($contents === false) {
    $success = false;
    $error = error_get_last ();
    $errormessage = nl2br (gettext ("Could not get the file at the URL.") . "\n" . $error[message]);
  }
}

// Fhe file's length.
if ($success) {
  $length = strlen ($contents);
  $smarty->assign ("length", $length);
  if ($length < 4833203) {
    $success = false;
    $errormessage = gettext ("The file is too short.");
  }
}
 
// Save the data to a tarball.
if ($success) {
  $tarball = tempnam (sys_get_temp_dir(), '');
  unlink ($tarball);
  $tarball .= ".tar.gz";
  file_put_contents ($tarball, $contents);
  $smarty->assign ("tarball", $tarball);
}

// Display the page.
$smarty->assign ("success", $success);
$smarty->assign ("errormessage", $errormessage);
$smarty->display ("update2.tpl");
Assets_Page::footer ();

?>
