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
<div>
  <?php echo Locale_Translate::_("Search for verses with similar Strong's numbers in:") ?>
  <?php echo $this->bible; ?>
  |
  <img id="searchloading" src="../pix/loading.gif">
  <progress value="0" max="100"></progress>
  <span id="hitcount">0</span>
  |
  <span><a href="select.php" title="<?php echo Locale_Translate::_("Menu") ?>">☰</a></span>
</div>
<div>
  <input id="loadbutton" type="button" value="<?php echo Locale_Translate::_("Load") ?>" />
</div>
<div>
  <input id="searchentry" class="fullwidth" type="text" value="" placeholder="<?php echo Locale_Translate::_("Search for words like this") ?>" />
</div>
<div>
  <input id="searchbutton" disabled="disabled" type="button" value="<?php echo Locale_Translate::_("Search") ?>" />
</div>
<div id="searchresults"></div>
<script><?php echo $this->script; ?></script>
<script type="text/javascript" src="strongs.js?<?php echo Config_Logic::version () ?>"></script>


