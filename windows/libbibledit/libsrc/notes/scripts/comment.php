<?php
/*
Copyright (©) 2003-2014 Teus Benschop.

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
<p><a href="index.php"><?php echo Locale_Translate::_("Back to notes list") ?></a></p>
<p><?php echo Locale_Translate::_("Summary") ?>: <?php echo $this->summary ?></p>
<div><?php echo $this->content ?></div>
<hr>
<form style="border:1px #bbb solid;" action="comment.php" name="form"  method="post">
  <p><?php echo Locale_Translate::_("Add a comment to this note:") ?></p>
  <textarea name="comment" id="comment" class="fullwidth"></textarea>
  <p>
    <input type="submit" name="submit" value=<?php echo Locale_Translate::_("Save") ?> onClick="this.value = '<?php echo Locale_Translate::_ ("Please wait") ?>'; return true;" />
    <input type="submit" name="cancel" value=<?php echo Locale_Translate::_("Cancel") ?> />
  </p>
  <input type="hidden" name="id" value=<?php echo $this->id ?> />
</form>
<script src="../nicedit/nicedit.js"></script>
<script type="text/javascript">
  area1 = new nicEditor({fullPanel : true}).panelInstance('comment',{hasPanel : true});
  $ ("body").on ("paste", function (e) {
    var data = e.originalEvent.clipboardData.getData ('Text');
    e.preventDefault();
    data = data.replace (/\n/g, "<br>");
    document.execCommand ("insertHTML", false, data);
  });
  $(document).ready (function () {
    $ (".nicEdit-main").focus ();
  });
</script>
