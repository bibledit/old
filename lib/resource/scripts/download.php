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
<h3>translate("Download resource") "$this->name"</h3>
<p><a href="admin">[translate("back")]</a></p>
<br>
<p>
  translate("Bibledit can display external resources to the user.")
  translate("It fetches the resources from websites on the Internet.")
  translate("Some of those websites may be slow to respond.")
  translate("Bibledit may not always be able to reach them.")
</p>
<p>
  translate("To address these issues, you can download external resources.")
  translate("Bibledit then stores the data in its database.")
  translate("When a downloaded resource is needed, Bibledit can fetch the data from its database rather than from the external website.")
  translate("This works faster.")
  translate("It takes the load off the website.")
</p>
<br>
<p>
  translate("Number of verses available offline for this resource:")
  $this->count
  <?php if ($this->count) {
    [<a href="?name=$this->name&clear">translate("clear")</a>]
  <?php }
</p>
<p><a href="?name=$this->name&download">translate("Start downloading this resource, en view the progress in the Journal.")</a></p>
<br>
<p>
  translate("It takes partial downloads into account, because it only downloads the missing pieces.")
  translate("Clear any offline verses if a full download is needed.")
</p>
