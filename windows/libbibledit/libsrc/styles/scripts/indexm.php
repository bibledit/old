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
?>
<h2><?php echo Locale_Translate::_("Stylesheets") ?></h2>
<p>
  <?php echo Locale_Translate::_("This lists the currently available stylesheets.") ?>
  <a href="indexm.php?new="><?php echo Locale_Translate::_("Create a new stylesheet.") ?></a>
</p>
<?php foreach ($this->sheets as $offset => $sheet) { ?>
  <p>
    <?php echo $sheet ?>
    <?php if ($this->editable [$offset]) { ?>
    <a href="sheetm.php?name=<?php echo $sheet ?>">[<?php echo Locale_Translate::_("edit") ?>]</a>
    <?php } ?>
  </p>
<?php } ?>
