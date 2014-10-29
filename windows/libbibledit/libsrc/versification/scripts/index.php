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
<h1><?php echo Locale_Translate::_("Versifications") ?></h1>
<p><?php echo Locale_Translate::_("This lists the currently available versification systems.") ?> <a href="index.php?new="><?php echo Locale_Translate::_ ("You can create a new versification system.") ?></a></p>
<?php foreach ($this->systems as $system) { ?>
  <p><a href="system.php?name=<?php echo $system ?>"><?php echo $system ?></a></p>
<?php } ?>
