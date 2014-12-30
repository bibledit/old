<?php
/*
Copyright (©) 2003-2015 Teus Benschop.

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
  gettext("Bible"): 
  $this->bible;
  <a href="select.php" title="gettext("Menu")">☰</a>
</p>
<p>
  <input id="searchentry" type="text" value="" placeholder="gettext("Search for")" />
  ▶
  <input id="replaceentry" type="text" value="" placeholder="gettext("Replace with")" />
  <input id="previewbutton" type="button" value="gettext("Preview")"  />
  <img id="searchloading" src="../pix/loading.gif">
  <input id="applybutton" type="button" value="gettext("Apply all")"  />
  <progress value="0" max="100"></progress>
  <span id="hitcount"></span>
</p>
<p><input type="checkbox" id="casesensitive" checked="checked">gettext("Case sensitive")</p>
<p>gettext("Search") <input type="radio" id="searchplain" name="where" checked="checked">gettext ("main text without notes") <input type="radio" name="where">gettext("underlying USFM")</p>
<div id="searchresults"></div>
<script>$this->script;</script>
<script type="text/javascript" src="replace2.js?config_logic_version ()"></script>
