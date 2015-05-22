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
<h3><?php echo Locale_Translate::_("Settings for Resources") ?></h3>
<p><a href="index.php">[<?php echo Locale_Translate::_("back") ?></a>]</p>
<h4><?php echo Locale_Translate::_("Active resources") ?></h4>
<div id="sortable">
<?php foreach ($this->actives as $active) { ?>
  <p><a href="?remove=<?php echo $active ?>"> ✗ </a><?php echo $active ?></p>
<?php } ?>
</div>
<br>
<p>
  <?php echo Locale_Translate::_("To change the order, drag an item to another location.") ?>
  <?php echo Locale_Translate::_("Click ✗ to remove.") ?>
</p>
<h4><?php echo Locale_Translate::_("Available resources") ?></h4>
<p>
<?php echo Locale_Translate::_("Click to activate:") ?>
<?php foreach ($this->selectables as $offset => $selectable) { ?>
  <?php if ($offset) echo " | " ?>
  <a href="?add=<?php echo $selectable ?>"><?php echo $selectable ?></a>
<?php } ?>
</p>
<script type="text/javascript" src="organize.js?<?php echo Config_Logic::version () ?>"></script>
