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
<h3><?php echo Locale_Translate::_("Workbenches") ?></h3>
<div id="sortable">
<?php foreach ($this->workbenches as $workbench) { ?>
  <p>
    <a href="?remove=<?php echo $workbench ?>" title="<?php echo Locale_Translate::_("Delete workbench") ?>"> ✗ </a>
    |
    <a href="settings.php?name=<?php echo $workbench ?>" title="<?php echo Locale_Translate::_("Edit workbench") ?>"> ✎ </a>
    |
    <span class="drag"><?php echo $workbench ?></span>
  </p>
<?php } ?>
</div>
<br>
<form action="organize.php" name="form" method="post">
  <input type="text" name="add" maxlength="300"  />
  <input type="submit" name="create" value=<?php echo Locale_Translate::_("Create") ?> />
</form>
<br>
<p>
  <?php echo Locale_Translate::_("Drag the list of workbenches into the desired order.") ?>
  <?php echo Locale_Translate::_("Click ✗ to remove.") ?>
  <?php echo Locale_Translate::_("Click ✎ to edit.") ?>
  <?php echo Locale_Translate::_("Or create a new workbench.") ?>
</p>
<script type="text/javascript" src="organize.js?<?php echo Config_Logic::version () ?>"></script>
