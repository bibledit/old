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
<h2>gettext("Patterns")</h2>
<p class="error">$this->error</p>
<p class="success">$this->success</p>
<p>
  gettext("Bible"): $this->bible.
  gettext("Enter the patterns to search for.")
  gettext("One per line.")
</p>
<form action="settingspatterns.php" name="form" method="post">
  <p><textarea name="patterns" class="fullwidth">$this->patterns</textarea></p>
  <p><input type="submit" name="submit" value=gettext("Save") /></p>
</form>
<br>
<p>gettext("It will search the plain text of the Bible, without verse numbers, heading, and other additions.")</p>
<p><a href="settings.php">gettext("Go back")</a></p>
