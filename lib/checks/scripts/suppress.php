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
<h1>translate("Suppressed check results")</h1>
<p><a href="index">[translate("back")] <a href="#help">[translate ("help")]</a></p>
<br>
<p class="error">$this->error</p>
<p class="success">$this->success</p>
<?php for ($this->ids as $offset => $id) {
  <p style="color:grey;">
  <a href="suppress?release=$id">
  ✗
  </a>
  $this->data[$offset]
  </p>
<?php }
<br>
<a id="help"></a>
<p>
  translate("The page lists the suppressed results of the checks.")
</p>
<p>
  translate("Click ✗ to remove the items from the suppressed list.")
  translate("It will be visible again in the list of check results.")
</p>
