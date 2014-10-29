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
<h1><?php echo Locale_Translate::_("Edit Note Source") ?></h1>
<p class="error"><?php echo $this->error ?></p>
<p class="success"><?php echo $this->success ?></p>
<p><?php echo Locale_Translate::_("You can edit the source of a Consultation Note.") ?></p>
<p><?php echo Locale_Translate::_("Each Consultation Notes has an identifier.") ?></p>
<p><?php echo Locale_Translate::_("The identifier of a Consultation Note can be seen through the Actions link of that note.") ?></p>
<p><?php echo Locale_Translate::_("Example:") ?></p>
<p><?php echo Locale_Translate::_("Identifier:") ?> 943430030.</p>
<br>
<p><?php echo Locale_Translate::_("Enter the identifier of the Consultation Note to be edited and press Submit.") ?></p>
<form action="editsource.php" name="noteid" method="post">
  <p>
  <input type="text" name="identifier" maxlength="15" class="focus" value="<?php echo $this->identifier ?>" />
  <input type="submit" name="submit" value=<?php echo Locale_Translate::_("Submit") ?> />
  </p>
</form>
<br>
<form action="editsource.php?identifier=<?php echo $this->identifier ?>" name="notetext" method="post">
  <p><textarea name="data"><?php echo $this->data ?></textarea></p>
  <p><input type="submit" name="save" value=<?php echo Locale_Translate::_("Save") ?> /></p>
</form>
