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
<div><?php echo $this->navigationHtml ?></div>
<h4><?php echo gettext ("Resources") ?></h4>
<hr>
<?php foreach ($this->resources as $offset => $resource) { ?>
  <p><?php echo $resource ?></p>
  <div id="bibledit<?php echo $offset ?>"></div>
  <hr>
<?php } ?>
<p><a href="settings.php">[<?php echo gettext ("settings") ?>]</a></p>
<script><?php echo $this->script; ?></script>
<script type="text/javascript" src="index.js"></script>
<?php echo $this->navigationCode; ?>
