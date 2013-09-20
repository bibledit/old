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
<h3><?php echo gettext ("Clean Consultation Notes") ?></h3>
<p><?php echo gettext ("The process for cleaning consultation notes has started.") ?></p>
<p>
  <?php echo gettext ("The progress is visible in the logbook.") ?>
  <a href="../manage/logbook.php"><?php echo gettext ("View logbook.") ?></a>
</p>
<p><?php echo gettext ("It can take time, depending on the amount of consultation notes.") ?></p>
<br>
<p><?php echo gettext ("It starts by removing any markup from the note summaries.") ?></p>
<p><?php echo gettext ("It then removes duplicate consultation notes.") ?></p>
<p><?php echo gettext ("Notes are duplicates when their summary, contents, and verses are the same.") ?></p>
<p>
  <?php echo gettext ("Duplicates may occur for various reasons.") ?>
  <?php echo gettext ("One reason could be that some notes have been imported more than once.") ?>
</p>
