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


// Include version number in stylesheet to refresh the browser's cache after bibledit-web upgrade.
include ("../config/version.php");


?>
<!doctype html>
<html>
<head>
<meta http-equiv="content-type" content="text/html; charset=UTF-8"></meta>
<meta charset="utf-8" />
<link rel="shortcut icon" href="../pix/favicon.ico">
<?php if (isset ($this->head_lines)) foreach ($this->head_lines as $line) echo "$line\n"; ?>
<title><?php echo $this->title ?></title>
<link rel="stylesheet" type="text/css" href="../css/stylesheet.css?<?php echo $version; ?>"></link>
<script type="text/javascript" src="../jquery/jquery-1.10.2.min.js"></script>
<?php if ($this->include_jquery_ui) { ?>
<link rel="stylesheet" type="text/css" href="../jquery/jquery-ui-1.10.3.css"></link>
<script type="text/javascript" src="../jquery/jquery-ui-1.10.3.<?php echo $this->include_jquery_ui_subset ?>min.js"></script>
<?php } ?>
<?php if ($this->included_stylesheet) { ?>
<link rel="stylesheet" type="text/css" href="../dyncss/basic<?php echo $this->included_stylesheet ?>.css?<?php echo Config_Logic::version () ?>"></link>
<?php } ?>
<?php if ($this->included_editor_stylesheet) { ?>
<link rel="stylesheet" type="text/css" href="../dyncss/editor<?php echo $this->included_editor_stylesheet ?>.css?<?php echo Config_Logic::version () ?>"></link>
<link rel="stylesheet" type="text/css" href="../css/editor.css?<?php echo Config_Logic::version () ?>"></link>
<?php } ?>
<script type="text/javascript" src="../assets/header.js"></script>
<?php if ($this->display_navigator) { ?>
  <script type="text/javascript" src="../navigation/shortcuts.js?<?php echo Config_Logic::version () ?>"></script>
<?php } ?>
</head>
<?php
echo "<body";
if (isset ($this->onLoadStatement)) {
  echo ' onload="' . $this->onLoadStatement . '"';
}
echo ">\n";
?>
