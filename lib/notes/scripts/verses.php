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
<h3>gettext("Edit the verses?")</h3>
<form action="verses?id=$this->id" name="form" method="post">
  <p><textarea name="verses">$this->verses</textarea></p>
  <p><input type="submit" name="submit" value=gettext("Save") /></p>
</form>
<h4><a href="actions?id=$this->id">gettext("Cancel")</a></h4>
