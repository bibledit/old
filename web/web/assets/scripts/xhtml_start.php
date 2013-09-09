<?php
/*
Copyright (©) 2003-2013 Teus Benschop.

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
?>
<!doctype html>
<html>
<head>
<meta http-equiv="content-type" content="text/html; charset=UTF-8"></meta>
<meta charset="utf-8" />
<?php if (isset ($this->head_lines)) foreach ($this->head_lines as $line) echo "$line\n"; ?>
<title><?php echo "$this->title" ?></title>
<link rel="stylesheet" type="text/css" href="../css/stylesheet.css"></link>
<?php if ($this->include_jquery) { ?>
<script type="text/javascript" src="../jquery/jquery-1.10.2.min.js"></script>
<?php } ?>
<?php if ($this->include_jquery_ui) { ?>
<link rel="stylesheet" type="text/css" href="../jquery/jquery-ui-1.10.3.css"></link>
<script type="text/javascript" src="../jquery/jquery-ui-1.10.3.<?php echo $this->include_jquery_ui_subset ?>min.js"></script>
<?php } ?>
</head>
<?php  
echo "<body";
if (isset ($this->onLoadStatement)) {
  echo ' onload="' . $this->onLoadStatement . '"';
}
echo ">\n";
?>
