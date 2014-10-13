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
  <?php echo Locale_Translate::_("Bible") ?>: 
  <?php echo $this->bible; ?>
  <a href="select.php" title="<?php echo Locale_Translate::_("Menu") ?>">☰</a>
</p>
<p>
  <input id="searchentry" type="text" value="" placeholder="<?php echo Locale_Translate::_("Search for") ?>" />
  <input id="searchbutton" type="button" value="<?php echo Locale_Translate::_("Search") ?>"  />
  <img id="searchloading" src="../pix/loading.gif">
  <span id="hitcount"></span>
  <progress value="0" max="100"></progress>
</p>
<p><input type="checkbox" id="casesensitive"><?php echo Locale_Translate::_("Case sensitive") ?></p>
<p><?php echo Locale_Translate::_("Search") ?> <input type="radio" id="searchplain" name="where" checked="checked"><?php echo Locale_Translate::_ ("main text without notes") ?> <input type="radio" name="where"><?php echo Locale_Translate::_("underlying USFM") ?></p>
<p><input type="checkbox" id="currentbook"><?php echo Locale_Translate::_("Current book") ?></p>
<p><?php echo Locale_Translate::_("What to do with the search results") ?>:</p>
<p><input type="radio" id="load" name="share" checked="checked"><?php echo Locale_Translate::_("Display them on the page") ?></p>
<p><input type="radio" id="add" name="share"><?php echo Locale_Translate::_("Add them to the ones already on the page") ?></p>
<p><input type="radio" id="remove" name="share"><?php echo Locale_Translate::_("Remove them from the ones already on the page") ?></p>
<p><input type="radio" id="intersect" name="share"><?php echo Locale_Translate::_("Display the intersection of them and the ones already on the page") ?></p>
<div id="searchresults"></div>
<script><?php echo $this->script; ?></script>
<script type="text/javascript" src="search2.js?<?php echo Config_Logic::version () ?>"></script>
