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
  <?php echo gettext("Bible") ?>: 
  <?php echo $this->bible; ?>
  <a href="select.php" title="<?php echo gettext("Menu") ?>">☰</a>
</p>
<p>
  <input id="searchentry" type="text" value="" placeholder="<?php echo gettext("Search for") ?>" />
  ▶
  <input id="replaceentry" type="text" value="" placeholder="<?php echo gettext("Replace with") ?>" />
  <input id="previewbutton" type="button" value="<?php echo gettext("Preview") ?>"  />
  <img id="searchloading" src="../pix/loading.gif">
  <input id="applybutton" type="button" value="<?php echo gettext("Apply all") ?>"  />
  <progress value="0" max="100"></progress>
  <span id="hitcount"></span>
</p>
<p><input type="checkbox" id="casesensitive" checked="checked"><?php echo gettext("Case sensitive") ?></p>
<p><?php echo gettext("Search") ?> <input type="radio" id="searchplain" name="where" checked="checked"><?php echo gettext ("main text without notes") ?> <input type="radio" name="where"><?php echo gettext("underlying USFM") ?></p>
<div id="searchresults"></div>
<script><?php echo $this->script; ?></script>
<script type="text/javascript" src="replace2.js?<?php echo config_logic_version () ?>"></script>
