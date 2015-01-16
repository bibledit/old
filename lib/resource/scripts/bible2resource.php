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
<h2>gettext("Convert Bible to USFM Resource")</h2>
<p>gettext("Bible"): $this->bible</p>
<br>
<p>
  gettext("You can convert a Bible to a USFM Resource.")
  gettext("It means that the text of the Bible will be moved to a Resource.")
  <a href="index" target="_blank">gettext("The users can display this resource.")</a>
</p>
<br>
<p>
  gettext("A Resource will be created with the same name and text as the Bible.")
  gettext("When the conversion is complete, the Bible will be deleted.")
  gettext("A Bible can be edited, but a USFM Resource cannot.")
  gettext("A Bible can be searched, but a USFM Resource cannot.")
  gettext("The USFM Resource uses less space.")
  gettext("Operations on the remaining Bibles will be faster, because the table for Bibles in the database will be smaller.")
</p>
<br>
<p class="error">$this->error</p>
<p class="success">$this->success</p>
<p><a href="?bible=$this->bible&convert=">gettext("Start the conversion process.")</a></p>
<p>gettext("You can view the conversion process in the Journal.")</p>
