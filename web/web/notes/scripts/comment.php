<p><a href="index.php"><?php echo gettext ("Back to notes list") ?></a></p>
<p><?php echo gettext ("Summary") ?>: <?php echo $this->summary ?></p>
<div><?php echo $this->content ?></div>
<hr>
<form style="border:1px #bbb solid;" action="comment.php" name="form"  method="post">
  <p><?php echo gettext ("Add a comment to this note:") ?></p>
  <p><textarea name="comment" class="rte-zone fullwidth"></textarea></p>
  <p>
    <input type="submit" name="submit" value=<?php echo gettext ("Save") ?> onClick="this.value = '<?php echo gettext ("Please wait") ?>'; return true;" />
    <input type="submit" name="cancel" value=<?php echo gettext ("Cancel") ?> />
  </p>
  <input type="hidden" name="id" value=<?php echo $this->id ?> />
</form>
<script src="../rte/jquery.rte.js"></script>
<script type="text/javascript">
$(document).ready (function () {
  $('.rte-zone').rte ("css url", "toolbox images url");
  $(".rte-zone").focus();
});
</script>
