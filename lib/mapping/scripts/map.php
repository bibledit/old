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
<h1>gettext("Verse mappings") $this->name</h1>
<form action="map.php?name=$this->name" name="form" method="post">
  <p><textarea name="data">$this->data</textarea></p>
  <p><input type="submit" name="submit" value=gettext("Save") /></p>
</form>
<br>
<p>gettext("To update the verse mappings, make changes above, and save them.")</p>
<p>gettext("Let's look at a sample mapping:")</p>
<code>Genesis 31:55 = Genesis 32:1</code>
<p>
  gettext("It means that Genesis 31:55 in this verse mapping matches with Genesis 31:1 in the original Bible text.")
  gettext("If this is the English verse mapping, it means that Genesis 31:55 in Bibles with the English versification system matches with Genesis 31:1 in Hebrew Bibles.")
  gettext("The same applies to verse mappings for the New Testament.")
  gettext("In short: At the left side of the equation sign is the passage in your Bible.")
  gettext("And at the right side is the passage in the Hebrew or Greek Bible.")
</p>
