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
<form style="border:1px #bbb solid;" action="create.php"  name="form" method="post">
  <p>
    gettext("Create consultation note for")
    $this->passage
  </p>
  <p>gettext("Summary"):</p>
  <p><input type="text" name="summary" maxlength="100" class="fullwidth" value="$this->summary"/></p>
  <p>gettext("Body"): </p>
  <textarea name="contents" class="fullwidth">$this->contents</textarea>
  <p>
    <input type="submit" name="submit" value="gettext("Save")" onClick="this.value = 'gettext ("Please wait")'; return true;" />
    <input type="submit" name="cancel" value="gettext("Cancel")" />
  </p>
  <input type="hidden" name="bible" value="$this->bible" />
  <input type="hidden" name="book" value="$this->book" />
  <input type="hidden" name="chapter" value="$this->chapter" />
  <input type="hidden" name="verse" value="$this->verse" />
</form>
<script src="../nicedit/nicedit.js"></script>
<script type="text/javascript">
	bkLib.onDomLoaded (function () { nicEditors.allTextAreas () });
  $ ("body").on ("paste", function (e) {
    var data = e.originalEvent.clipboardData.getData ('Text');
    e.preventDefault();
    data = data.replace (/\n/g, "<br>");
    document.execCommand ("insertHTML", false, data);
  });
  document.form.summary.focus();
</script>
