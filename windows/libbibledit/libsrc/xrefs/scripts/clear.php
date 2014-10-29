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
<p>
  <?php echo Locale_Translate::_("This chapter of the target Bible contains cross references:") ?>
  <?php echo $this->count; ?>
</p>
<?php if ($this->tags != 0) { ?>
<p>
  <?php echo Locale_Translate::_("A cross reference is malformed.") ?>
  <?php echo Locale_Translate::_("The opening tag does not match the closing tag.") ?>
  <?php echo Locale_Translate::_("Please fix it first.") ?>
</p>
<?php } else { ?>
<p><a href="clear.php?overwrite="><?php echo Locale_Translate::_("Overwrite the existing cross references.") ?></a></p>
<?php } ?>
<p><a href="index.php"><?php echo Locale_Translate::_("Cancel") ?></a></p>
