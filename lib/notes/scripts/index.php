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
<p>
  <?php echo Locale_Translate::_("Notes") ?>
  |
  <a href="select.php">[<?php echo Locale_Translate::_("select") ?>]</a>
  <?php if ($this->level >= 5) { ?>
    |
    [<a href="bulk.php"><?php echo Locale_Translate::_("update") ?></a>]
  <?php } ?>
  |
  <a href="create.php"><?php echo Locale_Translate::_("Create note") ?></a>
</p>
<br>
<div id="noteslist"><img src="../pix/loading.gif"></div>
<script type="text/javascript" src="index.js?<?php echo Config_Logic::version () ?>"></script>
<script type="text/javascript" src="poll.js?<?php echo Config_Logic::version () ?>"></script>
