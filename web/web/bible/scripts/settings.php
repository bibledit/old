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
<h1><?php echo Locale_Translate::_("Bible") ?> <?php echo $this->bible ?></h1>
<p>
  <?php echo Locale_Translate::_("Available books") ?>
  (<?php echo $this->book_count; ?>):
  <?php foreach ($this->book_ids as $offset => $book_id) { ?>
    <a href="book.php?bible=<?php echo $this->bible ?>&book=<?php echo $this->book_ids[$offset] ?>"><?php echo $this->book_names[$offset] ?></a>
  <?php } ?>
</p>
<p>
  <?php echo Locale_Translate::_("Versification system") ?>:
  <?php if ($this->write_access) { ?>
    <a href="settings.php?bible=<?php echo $this->bible ?>&versification=">
  <?php } ?>
  <?php echo $this->versification ?>
  <?php if ($this->write_access) { ?>
  </a>
  <?php } ?>
</p>
<p>
  <?php echo Locale_Translate::_("Verse mapping") ?>:
  <?php if ($this->write_access) { ?>
    <a href="settings.php?bible=<?php echo $this->bible ?>&mapping=">
  <?php } ?>
  <?php echo $this->mapping ?>
  <?php if ($this->write_access) { ?>
  </a>
  <?php } ?>
</p>
<?php if ($this->write_access) { ?>
  <p><a href="settings.php?bible=<?php echo $this->bible ?>&createbook="><?php echo Locale_Translate::_("Create book") ?></a></p>
  <p><a href="import_usfm.php?bible=<?php echo $this->bible ?>"><?php echo Locale_Translate::_("Import USFM") ?></a></p>
  <p><a href="import_bibleworks.php?bible=<?php echo $this->bible ?>"><?php echo Locale_Translate::_("Import BibleWorks") ?></a></p>
<?php } ?>
<p><a href="manage.php?copy=<?php echo $this->bible ?>"><?php echo Locale_Translate::_("Copy this Bible") ?></a></p>
<?php if ($this->write_access) { ?>
  <p><a href="manage.php?delete=<?php echo $this->bible ?>"><?php echo Locale_Translate::_("Delete this Bible") ?></a></p>
  <p><a href="../resource/bible2resource.php?bible=<?php echo $this->bible ?>"><?php echo Locale_Translate::_("Convert to USFM resource") ?></a></p>
<?php } ?>
<p><a href="../compare/index.php?bible=<?php echo $this->bible ?>"><?php echo Locale_Translate::_("Compare with ...") ?></a></p>
<p>
<?php if ($this->write_access) { ?>
  <a href="?bible=<?php echo $this->bible ?>&viewable=">
<?php } ?>
<?php if ($this->viewable == true) { ?> ☑ <?php } else { ?> ☐ <?php } ?>
<?php if ($this->write_access) { ?>
</a>
<?php } ?>
<?php echo Locale_Translate::_("Viewable by all users") ?>
</p>
<?php if ($this->write_access) { ?>
  <p><a href="abbreviations.php?bible=<?php echo $this->bible ?>"><?php echo Locale_Translate::_("Book abbreviations") ?></a></p>
  <p><a href="order.php?bible=<?php echo $this->bible ?>"><?php echo Locale_Translate::_("Book order") ?></a></p>
  <p><a href="css.php?bible=<?php echo $this->bible ?>"><?php echo Locale_Translate::_("Font and text direction") ?></a></p>
<?php } ?>
