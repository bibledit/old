<p><a href="index.php"><?php echo gettext ("Back to notes list") ?></a></p>
<p><?php echo gettext ("Summary") ?>: <?php echo $this->summary ?></p>
<div><?php echo $this->content ?></div>
<hr>
<form style="border:1px #bbb solid;" action="comment.php" name="form"  method="post">
  <p><?php echo gettext ("Add a comment to this note:") ?></p>
  <p><textarea name="comment" class="fullwidth"></textarea></p>
  <p>
    <input type="submit" name="submit" value=<?php echo gettext ("Save") ?> onClick="this.value = '<?php echo gettext ("Please wait") ?>'; return true;" />
    <input type="submit" name="cancel" value=<?php echo gettext ("Cancel") ?> />
  </p>
  <input type="hidden" name="id" value=<?php echo $this->id ?> />
</form>
<script src="../nicedit/nicedit.js"></script>
<script type="text/javascript">
	bkLib.onDomLoaded (function () { nicEditors.allTextAreas () });
  $ ("body").on ("paste", function (e) {
    var data = e.originalEvent.clipboardData.getData ('Text');
    e.preventDefault();
    data = data.replace (/\n/g, "");
    document.execCommand ("insertHTML", false, data);
  });
  
$(document).ready (function () {

  //$("textarea").focus ();
  //$ ("body > form > p > div").focus ();
  //$("body").focus ();
  //$ (".nicEdit-main").focus ();

});
  
  
  
</script>
