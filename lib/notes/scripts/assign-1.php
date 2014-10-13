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
<h3><?php echo Locale_Translate::_("Select the user to assign the note to") ?></h3>
<ul>
<?php foreach ($this->users as $user) { ?>
  <li><a href="assign-1.php?id=<?php echo $this->id ?>&assign=<?php echo $user ?>"><?php echo $user ?></a></li>
<?php } ?>
</ul>
<h4><a href="actions.php?id=<?php echo $this->id ?>"><?php echo Locale_Translate::_("Cancel") ?></a></h4>
