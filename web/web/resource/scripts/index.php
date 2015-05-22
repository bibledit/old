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
  <?php echo Locale_Translate::_("Resources") ?>
  <a href="organize.php">[<?php echo Locale_Translate::_("organize") ?>]</a>
</p>
<hr>
<?php foreach ($this->resources as $offset => $resource) { ?>
  <div id="line<?php echo $offset + 1 ?>" style="clear:both">
  <span id="name<?php echo $offset + 1 ?>" class="small"><?php echo $resource ?></span>
  <span id="loading<?php echo $offset + 1 ?>"><img src="../pix/loading.gif"></span>
  <span id="content<?php echo $offset + 1 ?>"></span>
  <hr style="clear:both">
  </div>
<?php } ?>
<script><?php echo $this->script; ?></script>
<script type="text/javascript" src="index.js?<?php echo Config_Logic::version () ?>"></script>
<script type="text/javascript" src="../navigation/nav.js?<?php echo Config_Logic::version () ?>"></script>
