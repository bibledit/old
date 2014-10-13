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
<h2><?php echo Locale_Translate::_("External resources") ?></h2>
<?php foreach ($this->resources as $resource) { ?>
  <p>
    <a href="?delete=<?php echo $resource ?>" class="deleteresource" title="<?php echo Locale_Translate::_("Delete resource") ?>"> ✗ </a>
    <a href="edit.php?name=<?php echo $resource ?>" title="<?php echo Locale_Translate::_("Edit resource") ?>"> ✎ </a>
    <a href="download.php?name=<?php echo $resource ?>" title="<?php echo Locale_Translate::_("Download resource") ?>"> ⇓ </a>
    <?php echo $resource ?>
  </p>
<?php } ?>
<br>
<form action="admin.php" name="form" method="post">
  <input type="text" name="entry" maxlength="300"  />
  <input type="submit" name="submit" value=<?php echo Locale_Translate::_("Add") ?> />
</form>
<div id="dialog-confirm" title="<?php echo Locale_Translate::_("Remove the resource?") ?>" style="display:none;">
  <p>
    <span class="ui-icon ui-icon-alert" style="float: left; margin: 0 7px 20px 0;"></span>
    <?php echo Locale_Translate::_("Are you sure?") ?>
  </p>
</div>
<br>
<p>
  <?php echo Locale_Translate::_("You can add, edit, download, and delete resources.") ?>
  <?php echo Locale_Translate::_("A new resource needs at least one capital in its name to be valid.") ?>
</p>
<p>
  <?php echo Locale_Translate::_("Bibledit-Web comes with a set of default resources.") ?>
  <?php echo Locale_Translate::_("Changes in them will be overwritten during upgrade of Bibledit-Web.") ?>
</p>
<link rel="stylesheet" href="../jquery/smoothness/jquery-ui-1.10.3.css" />
<script type="text/javascript" src="admin.js?<?php echo Config_Logic::version () ?>"></script>
