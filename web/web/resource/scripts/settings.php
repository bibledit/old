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
<h2><?php echo gettext ("Settings for Resources") ?></h2>
<p><a href="index.php">[<?php echo gettext ("back") ?></a>]</p>
<h3><?php echo gettext ("Active resources") ?></h3>
<div id="sortable">
<?php foreach ($this->actives as $active) { ?>
  <p><a href="?remove=<?php echo $active ?>"> ✗ </a><?php echo $active ?></p>
<?php } ?>
</div>
<br>
<p>
  <?php echo gettext ("To change the order, drag an item to another location.") ?>
  <?php echo gettext ("Click ✗ to remove.") ?>
</p>
<h3><?php echo gettext ("Available resources") ?></h3>
<p>
<?php echo gettext ("Internal Bibles:") ?>
<?php foreach ($this->bibles as $offset => $bible) { ?>
  <?php if ($offset) echo " | " ?>
  <a href="?add=<?php echo $bible ?>"><?php echo $bible ?></a>
<?php } ?>
</p>
<p>
<?php echo gettext ("Internal USFM resources:") ?>
<?php foreach ($this->usfms as $offset => $usfm) { ?>
  <?php if ($offset) echo " | " ?>
  <a href="?add=<?php echo $usfm ?>"><?php echo $usfm ?></a>
<?php } ?>
</p>
<p>
<?php echo gettext ("External resources:") ?>
<?php foreach ($this->externals as $offset => $external) { ?>
  <?php if ($offset) echo " | " ?>
  <a href="?add=<?php echo $external ?>"><?php echo $external ?></a>
<?php } ?>
</p>
<script type="text/javascript" src="settings.js"></script>
