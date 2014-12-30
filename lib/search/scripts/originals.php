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
<div>
  gettext("Search for verses with similar Hebrew or Greek in:")
  $this->bible;
  |
  <img id="searchloading" src="../pix/loading.gif">
  <progress value="0" max="100"></progress>
  <span id="hitcount">0</span>
  |
  <span><a href="select.php" title="gettext("Menu")">☰</a></span>
</div>
<div>
  <input id="loadbutton" type="button" value="gettext("Load")" />
</div>
<div>
  <input id="searchentry" class="fullwidth" type="text" value="" placeholder="gettext("Search for words like this")" />
</div>
<div>
  <input id="searchbutton" disabled="disabled" type="button" value="gettext("Search")" />
</div>
<div id="searchresults"></div>
<script>$this->script;</script>
<script type="text/javascript" src="originals.js?config_logic_version ()"></script>
