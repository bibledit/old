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
<h1><?php echo gettext ("Send/Receive") ?></h1>
<p><?php echo gettext ("Bibledit-Web sends and receives the Bibles every day at midnight.") ?></p>
<p><?php echo gettext ("It receives changes made by others, and it sends changes made here to the server for use by other translators.") ?></p>
<br>
<p class="success"><?php echo $this->success ?></p>
<p><a href="?run="><?php echo gettext ("Send and receive Bibles now.") ?></a></p>
<p>
<a href="?togglerepeat=">
<?php if ($this->repeat == true) { ?> ☑ <?php } else { ?> ☐ <?php } ?>
</a>
<?php echo gettext ("Send and receive every five minutes.") ?>
</p>
<p><a href="../manage/logbook.php"><?php echo gettext ("See the Logbook for details.") ?></a></p>


