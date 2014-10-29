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
<style type="text/css">
<?php echo $this->custom_css ?>
</style>
<h3><?php echo Locale_Translate::_("Font and text direction") ?></h3>
<p><?php echo Locale_Translate::_("Bible") ?>: <a href="settings.php?bible=<?php echo $this->bible ?>"><?php echo $this->bible ?></a></p>
<br>
<p>
  <?php echo Locale_Translate::_("Set the font, the text direction, and the writing mode for the Bible editors.") ?>
</p>
<br>
<form action="?bible=<?php echo $this->bible ?>" name="form" method="post">

  <p><?php echo Locale_Translate::_("Font") ?>: <input type="text" name="font" value="<?php echo $this->font ?>"></p>
  <p><?php echo Locale_Translate::_("A font can be entered in different ways:") ?></p>
  <ul>
  <li>
    <?php echo Locale_Translate::_("Enter the name of a locally available font.") ?>
    <?php echo Locale_Translate::_("Example:") ?>
    Gentium Plus
  </li>
  <li>
    <a href="../fonts/index.php" target="_blank"><?php echo Locale_Translate::_("Upload a font file.") ?></a>
    <?php echo Locale_Translate::_("Then enter its name.") ?>
    <?php echo Locale_Translate::_("Example:") ?>
    GentiumPlus-R.ttf
  </li>
  <li>
    <?php echo Locale_Translate::_("Enter the name of a font available on the Internet.") ?>
    <?php echo Locale_Translate::_("Example:") ?>
    http://website.org/fonts/GentiumPlus-R.ttf
  </li>
  </ul>


  <br>
  <p><?php echo Locale_Translate::_("Direction") ?>:</p>
  <p><input type="radio" name="direction" value="" <?php echo $this->direction_none ?>><?php echo Locale_Translate::_("Unspecified") ?></p>
  <p><input type="radio" name="direction" value="ltr" <?php echo $this->direction_ltr ?>><?php echo Locale_Translate::_("Left-to-right") ?></p>
  <p><input type="radio" name="direction" value="rtl" <?php echo $this->direction_rtl ?>><?php echo Locale_Translate::_("Right-to-left") ?></p>
  <br>
  <p><?php echo Locale_Translate::_("Writing mode") ?>:</p>
  <p><input type="radio" name="mode" value="" <?php echo $this->mode_none ?>><?php echo Locale_Translate::_("Unspecified") ?></p>
  <p><input type="radio" name="mode" value="tb-lr" <?php echo $this->mode_tblr ?>><?php echo Locale_Translate::_("Top to bottom and left to right") ?></p>
  <p><input type="radio" name="mode" value="tb-rl" <?php echo $this->mode_tbrl ?>><?php echo Locale_Translate::_("Top to bottom and right to left") ?></p>
  <p><input type="radio" name="mode" value="bt-lr" <?php echo $this->mode_btlr ?>><?php echo Locale_Translate::_("Bottom to top and left to right") ?></p>
  <p><input type="radio" name="mode" value="bt-rl" <?php echo $this->mode_btrl ?>><?php echo Locale_Translate::_("Bottom to top and right to left") ?></p>
  <br>
  <p><input type="submit" name="submit" value=<?php echo Locale_Translate::_("Save") ?> /></p>
</form>
<br>
<div class="<?php echo $this->custom_class ?>" contenteditable="true">
  <?php echo Locale_Translate::_("Save the settings to apply them to this editable sample text.") ?>
</p>
