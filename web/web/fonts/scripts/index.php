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
<h2><?php echo Locale_Translate::_("Fonts") ?></h2>

<p><?php echo Locale_Translate::_("The following fonts are available within Bibledit-Web:") ?></p>
<br>
<?php foreach ($this->fonts as $font) { ?>
  <p>
   <a href="?delete=<?php echo $font ?>" title="<?php echo Locale_Translate::_("Delete font") ?>"> ✗ </a>
   <?php echo $font ?>
  </p>
<?php } ?>

<br>

<p>
  <?php echo Locale_Translate::_("You can upload a new font.") ?>
</p>
<form enctype="multipart/form-data" action="index.php" method="POST">
  <input name="data" type="file" />
  <input type="submit" name="upload" value="<?php echo Locale_Translate::_("Upload") ?>" />
</form>
<p><?php echo Locale_Translate::_("Maximum size of font to upload:") ?> <?php echo $this->upload_max_filesize ?></p>
