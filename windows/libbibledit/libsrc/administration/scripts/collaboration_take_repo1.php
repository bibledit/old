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
<h1><?php echo Locale_Translate::_("Take data in repository") ?></h1>
<p><?php echo Locale_Translate::_("Bible") ?>: <?php echo $this->object ?></p>
<p><?php echo Locale_Translate::_("Repository URL") ?>: <?php echo $this->url ?></p>
<p><?php echo Locale_Translate::_("The data in the remote repository is being copied over into Bibledit-Web's data, overwriting the data that was there before.") ?></p>
<?php foreach ($this->contents as $line) { ?>
  <p><code><?php echo $line ?></code></p>
<?php } ?>
