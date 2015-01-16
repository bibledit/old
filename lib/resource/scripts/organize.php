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
<h3>gettext("Settings for Resources")</h3>
<p><a href="index">[gettext("back")</a>]</p>
<h4>gettext("Active resources")</h4>
<div id="sortable">
<?php for ($this->actives as $active) {
  <p><a href="?remove=$active"> ✗ </a>$active</p>
<?php }
</div>
<br>
<p>
  gettext("To change the order, drag an item to another location.")
  gettext("Click ✗ to remove.")
</p>
<h4>gettext("Available resources")</h4>
<p>
gettext("Click to activate:")
<?php for ($this->selectables as $offset => $selectable) {
  <?php if ($offset) echo " | "
  <a href="?add=$selectable">$selectable</a>
<?php }
</p>
<script type="text/javascript" src="organize.js?##VERSION##"></script>
