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
<h1><?php echo $this->book_name ?></h1>
<p class="success"><?php echo $this->success_message ?></p>
<p class="error"><?php echo $this->error_message ?></p>
<p><?php echo Locale_Translate::_("Bible") ?>: <a href="settings.php?bible=<?php echo $this->bible ?>"><?php echo $this->bible ?></a></p>
<p><?php echo Locale_Translate::_("Chapters:") ?>
<?php foreach ($this->chapters as $offset => $chapter) { ?>
  <a href="chapter.php?bible=<?php echo $this->bible ?>&book=<?php echo $this->book ?>&chapter=<?php echo $this->chapters[$offset] ?>"><?php echo $this->chapters[$offset] ?></a>
<?php } ?>
</p>
<?php if ($this->write_access) { ?>
  <p><a href="book.php?bible=<?php echo $this->bible ?>&book=<?php echo $this->book ?>&createchapter="><?php echo Locale_Translate::_("Create chapter") ?></a></p>
  <p><a href="settings.php?bible=<?php echo $this->bible ?>&deletebook=<?php echo $this->book ?>"><?php echo Locale_Translate::_("Delete this book") ?></a></p>
<?php } ?>
