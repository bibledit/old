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
<div id="searchbox">
  <?php echo $this->bible; ?>
  <input id="searchentry" type="text" value="" placeholder="<?php echo Locale_Translate::_("Search for") ?>" />
  ▶
  <input id="replaceentry" type="text" value="" placeholder="<?php echo Locale_Translate::_("Replace with") ?>" />
  <input type="checkbox" id="casesensitive" checked="checked"><?php echo Locale_Translate::_("Case sensitive") ?>
  <input id="previewbutton" type="button" value="<?php echo Locale_Translate::_("Preview") ?>"  />
  <img id="searchloading" src="../pix/loading.gif">
  <input id="applybutton" type="button" value="<?php echo Locale_Translate::_("Apply all") ?>"  />
  <progress value="0" max="100"></progress>
  <span id="hitcount"></span>
  <span><a href="select.php" title="<?php echo Locale_Translate::_("Menu") ?>">☰</a></span>
</div>
<div id="searchresults">
</div>
<script>
<?php echo $this->script; ?>
</script>
<script type="text/javascript" src="replace.js?<?php echo Config_Logic::version () ?>"></script>
