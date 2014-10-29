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
<h1><?php echo Locale_Translate::_("Verse mappings") ?> <?php echo $this->name ?></h1>
<form action="map.php?name=<?php echo $this->name ?>" name="form" method="post">
  <p><textarea name="data"><?php echo $this->data ?></textarea></p>
  <p><input type="submit" name="submit" value=<?php echo Locale_Translate::_("Save") ?> /></p>
</form>
<br>
<p><?php echo Locale_Translate::_("To update the verse mappings, make changes above, and save them.") ?></p>
<p><?php echo Locale_Translate::_("Let's look at a sample mapping:") ?></p>
<code>Genesis 31:55 = Genesis 32:1</code>
<p>
  <?php echo Locale_Translate::_("It means that Genesis 31:55 in this verse mapping matches with Genesis 31:1 in the original Bible text.") ?>
  <?php echo Locale_Translate::_("If this is the English verse mapping, it means that Genesis 31:55 in Bibles with the English versification system matches with Genesis 31:1 in Hebrew Bibles.") ?>
  <?php echo Locale_Translate::_("The same applies to verse mappings for the New Testament.") ?>
  <?php echo Locale_Translate::_("In short: At the left side of the equation sign is the passage in your Bible.") ?>
  <?php echo Locale_Translate::_("And at the right side is the passage in the Hebrew or Greek Bible.") ?>
</p>
