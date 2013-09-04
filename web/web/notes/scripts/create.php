<form style="border:1px #bbb solid;" action="create.php"  name="form" method="post">
  <p>
    <?php echo gettext ("Create consultation note for") ?>
    <?php echo $this->passage ?>
  </p>
  <p><?php echo gettext ("Summary") ?>:</p>
  <p><input type="text" name="summary" maxlength="100" class="fullwidth" value="<?php echo $this->summary ?>"/></p>
  <p><?php echo gettext ("Body") ?>: </p>
  <textarea name="contents" class="rte-zone fullwidth"><?php echo $this->contents ?></textarea>
  <p>
    <input type="submit" name="submit" value=<?php echo gettext ("Save") ?> onClick="this.value = '<?php echo gettext ("Please wait") ?>'; return true;" />
    <input type="submit" name="cancel" value=<?php echo gettext ("Cancel") ?> />
  </p>
  <input type="hidden" name="bible" value=<?php echo $this->bible ?> />
  <input type="hidden" name="book" value=<?php echo $this->book ?> />
  <input type="hidden" name="chapter" value=<?php echo $this->chapter ?> />
  <input type="hidden" name="verse" value=<?php echo $this->verse ?> />
</form>
<script src="../rte/jquery.rte.js"></script>
<script type="text/javascript">
$(document).ready (function () {
  $('.rte-zone').rte ("css url", "toolbox images url");
  document.form.summary.focus();
});
</script>
