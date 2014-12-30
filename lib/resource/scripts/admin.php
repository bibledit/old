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
<h2>gettext("External resources")</h2>
<?php for ($this->resources as $resource) {
  <p>
    <a href="?delete=$resource" class="deleteresource" title="gettext("Delete resource")"> ✗ </a>
    <a href="edit.php?name=$resource" title="gettext("Edit resource")"> ✎ </a>
    <a href="download.php?name=$resource" title="gettext("Download resource")"> ⇓ </a>
    $resource
  </p>
<?php }
<br>
<form action="admin.php" name="form" method="post">
  <input type="text" name="entry" maxlength="300"  />
  <input type="submit" name="submit" value=gettext("Add") />
</form>
<div id="dialog-confirm" title="gettext("Remove the resource?")" style="display:none;">
  <p>
    <span class="ui-icon ui-icon-alert" style="float: left; margin: 0 7px 20px 0;"></span>
    gettext("Are you sure?")
  </p>
</div>
<br>
<p>
  gettext("You can add, edit, download, and delete resources.")
  gettext("A new resource needs at least one capital in its name to be valid.")
</p>
<p>
  gettext("Bibledit comes with a set of default resources.")
  gettext("Changes in them will be overwritten during upgrade of Bibledit.")
</p>
<link rel="stylesheet" href="../jquery/smoothness/jquery-ui-1.10.3.css" />
<script type="text/javascript" src="admin.js?config_logic_version ()"></script>
