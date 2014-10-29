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
<h1><?php echo Locale_Translate::_("Bibles") ?></h1>
<p class="success"><?php echo $this->success_message ?></p>
<p class="error"><?php echo $this->error_message ?></p>
<p><?php echo Locale_Translate::_("Available Bibles:") ?></p>
<ul>
<?php foreach ($this->bibles as $offset => $bible) { ?>
  <li><a href="settings.php?bible=<?php echo $this->bibles[$offset] ?>"><?php echo $this->bibles[$offset] ?></a></li>
<?php } ?>
</ul>
<p><a href="manage.php?new="><?php echo Locale_Translate::_("Create a new empty Bible") ?></a></p>
