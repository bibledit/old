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
<h3><?php echo Locale_Translate::_("Book abbreviations") ?></h3>
<p class="success"><?php echo $this->success_message ?></p>
<p class="error"><?php echo $this->error_message ?></p>
<p><?php echo Locale_Translate::_("Bible") ?>: <a href="settings.php?bible=<?php echo $this->bible ?>"><?php echo $this->bible ?></a></p>
<br>
<p>
  <?php echo Locale_Translate::_("Update the abbreviations.") ?>
  <?php echo Locale_Translate::_("One line per book.") ?>
  <?php echo Locale_Translate::_("Book lines may repeat with different abbreviations for the same book.") ?>
</p>
<form action="?bible=<?php echo $this->bible ?>" name="form" method="post">
  <p><textarea name="data"><?php echo $this->data ?></textarea></p>
  <p><input type="submit" name="submit" value=<?php echo Locale_Translate::_("Save") ?> /></p>
</form>
<br>
<p><a href="../xrefs/index.php" target="_blank"><?php echo Locale_Translate::_("Book abbreviations are used while copying cross references from a source Bible to a target Bible.") ?></a></p>
