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
  <input id="searchbutton" type="button" value="gettext("Search")"  />
  <img id="searchloading" src="../pix/loading.gif">
  <span id="hitcount"></span>
  <progress value="0" max="100"></progress>
</p>
<p><input type="checkbox" id="casesensitive">gettext("Case sensitive")</p>
<p>gettext("Search") <input type="radio" id="searchplain" name="where" checked="checked">gettext ("main text without notes") <input type="radio" name="where">gettext("underlying USFM")</p>
<p><input type="checkbox" id="currentbook">gettext("Current book")</p>
<p>gettext("What to do with the search results"):</p>
<p><input type="radio" id="load" name="share" checked="checked">gettext("Display them on the page")</p>
<p><input type="radio" id="add" name="share">gettext("Add them to the ones already on the page")</p>
<p><input type="radio" id="remove" name="share">gettext("Remove them from the ones already on the page")</p>
<p><input type="radio" id="intersect" name="share">gettext("Display the intersection of them and the ones already on the page")</p>
<div id="searchresults"></div>
<script>$this->script;</script>
<script type="text/javascript" src="search2.js?config_logic_version ()"></script>
