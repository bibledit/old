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
<body onload="document.form.entry.focus();">
<h2><?php echo $this->question ?></h2>
<form action="<?php echo $this->base_url ?>" name="form" method="post">
  <p><input type="text" name="entry" maxlength="300" value="<?php echo $this->value ?>" /></p>
  <p><input type="submit" name="<?php echo $this->submit ?>" value=<?php echo Locale_Translate::_("Submit") ?> /></p>
</form>
<h3><a href="<?php echo $this->base_url ?>"><?php echo Locale_Translate::_("Cancel") ?></a></h3>
<p><?php echo $this->help ?></p>
