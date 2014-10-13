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
<h2><?php echo Locale_Translate::_("Patterns") ?></h2>
<p class="error"><?php echo $this->error ?></p>
<p class="success"><?php echo $this->success ?></p>
<p>
  <?php echo Locale_Translate::_("Bible") ?>: <?php echo $this->bible ?>.
  <?php echo Locale_Translate::_("Enter the patterns to search for.") ?>
  <?php echo Locale_Translate::_("One per line.") ?>
</p>
<form action="settingspatterns.php" name="form" method="post">
  <p><textarea name="patterns" class="fullwidth"><?php echo $this->patterns ?></textarea></p>
  <p><input type="submit" name="submit" value=<?php echo Locale_Translate::_("Save") ?> /></p>
</form>
<br>
<p><?php echo Locale_Translate::_("It will search the plain text of the Bible, without verse numbers, heading, and other additions.") ?></p>
<p><a href="settings.php"><?php echo Locale_Translate::_("Go back") ?></a></p>
