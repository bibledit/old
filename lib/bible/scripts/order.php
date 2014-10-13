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
<h3><?php echo Locale_Translate::_("Book order") ?></h3>
<p><?php echo Locale_Translate::_("Bible") ?>: <a href="settings.php?bible=<?php echo $this->bible ?>"><?php echo $this->bible ?></a></p>
<br>
<p>
  <?php echo Locale_Translate::_("Drag books to another location to update the order of the books in this Bible.") ?>
  <a href="?bible=<?php echo $this->bible ?>&reset="><?php echo Locale_Translate::_("Reset order.") ?></a>
</p>
<br>
<div id="sortable">
<?php foreach ($this->names as $name) { ?>
  <p> ⇕ <span class="drag"><?php echo $name ?></span> ⇕ </p>
<?php } ?>
</div>
<script>
<?php echo $this->script; ?>
</script>
<script type="text/javascript" src="order.js?<?php echo Config_Logic::version () ?>"></script>
