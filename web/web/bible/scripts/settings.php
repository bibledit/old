<?php
/*
Copyright (©) 2003-2013 Teus Benschop.

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
<h1><?php echo gettext ("Bible") ?> <?php echo $this->bible ?></h1>
<p>
  <?php echo gettext ("Available books") ?>:
  <?php foreach ($this->book_ids as $offset => $book_id) { ?>
    <a href="book.php?bible=<?php echo $this->bible ?>&book=<?php echo $this->book_ids[$offset] ?>"><?php echo $this->book_names[$offset] ?></a>
  <?php } ?>
</p>
<p><?php echo gettext ("Versification system") ?>: <a href="settings.php?bible=<?php echo $this->bible ?>&versification="><?php echo $this->versification ?></a></p>
<p><a href="settings.php?bible=<?php echo $this->bible ?>&createbook="><?php echo gettext ("Create book") ?></a></p>
<p><a href="import_usfm.php?bible=<?php echo $this->bible ?>"><?php echo gettext ("Import USFM") ?></a></p>
<p><a href="import_bibleworks.php?bible=<?php echo $this->bible ?>"><?php echo gettext ("Import BibleWorks") ?></a></p>
<p><a href="manage.php?copy=<?php echo $this->bible ?>"><?php echo gettext ("Copy this Bible") ?></a></p>
<p><a href="manage.php?delete=<?php echo $this->bible ?>"><?php echo gettext ("Delete this Bible") ?></a></p>
<p><a href="../resource/bible2resource.php?bible=<?php echo $this->bible ?>"><?php echo gettext ("Convert to USFM resource") ?></a></p>
