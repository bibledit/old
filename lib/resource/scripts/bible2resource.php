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
<h2><?php echo gettext("Convert Bible to USFM Resource") ?></h2>
<p><?php echo gettext("Bible") ?>: <?php echo $this->bible ?></p>
<br>
<p>
  <?php echo gettext("You can convert a Bible to a USFM Resource.") ?>
  <?php echo gettext("It means that the text of the Bible will be moved to a Resource.") ?>
  <a href="index.php" target="_blank"><?php echo gettext("The users can display this resource.") ?></a>
</p>
<br>
<p>
  <?php echo gettext("A Resource will be created with the same name and text as the Bible.") ?>
  <?php echo gettext("When the conversion is complete, the Bible will be deleted.") ?>
  <?php echo gettext("A Bible can be edited, but a USFM Resource cannot.") ?>
  <?php echo gettext("A Bible can be searched, but a USFM Resource cannot.") ?>
  <?php echo gettext("The USFM Resource uses less space.") ?>
  <?php echo gettext("Operations on the remaining Bibles will be faster, because the table for Bibles in the database will be smaller.") ?>
</p>
<br>
<p class="error"><?php echo $this->error ?></p>
<p class="success"><?php echo $this->success ?></p>
<p><a href="?bible=<?php echo $this->bible ?>&convert="><?php echo gettext("Start the conversion process.") ?></a></p>
<p><?php echo gettext("You can view the conversion process in the Journal.") ?></p>
