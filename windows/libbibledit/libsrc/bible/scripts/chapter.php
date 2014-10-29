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
<h1><?php echo $this->book_name ?> <?php echo $this->chapter ?></h1>
<p><?php echo Locale_Translate::_("Bible") ?>: <a href="settings.php?bible=<?php echo $this->bible ?>"><?php echo $this->bible ?></a></p>
<p><?php echo Locale_Translate::_("Book") ?>: <a href="book.php?bible=<?php echo $this->bible ?>&book=<?php echo $this->book ?>"><?php echo $this->book_name ?></a></p>
<?php if ($this->write_access) { ?>
  <p><a href="book.php?bible=<?php echo $this->bible ?>&book=<?php echo $this->book ?>&deletechapter=<?php echo $this->chapter ?>"><?php echo Locale_Translate::_("Delete this chapter") ?></a></p>
<?php } ?>
