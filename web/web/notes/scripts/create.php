<form
  style="border:1px #bbb solid;"
  action="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&createnotebible=<?php echo $this->bible ?>&createnotebook=<?php echo $this->book ?>&createnotechapter=<?php echo $this->chapter ?>&createnoteverse=<?php echo $this->verse ?>&savenewconsultationnote=" 
  name="form" 
  method="post"
>
  <p><?php echo gettext ("Create a new consultation note.") ?></p>
  <p><?php echo gettext ("Summary") ?>:</p>
  <p><input type="text" name="summary" maxlength="100" class="fullwidth" /></p>
  <p><?php echo gettext ("Body") ?>: </p>
  <p><textarea name="contents" class="rte-zone"></textarea></p>
  <p><input type="submit" name="submit" value=<?php echo gettext ("Save") ?> onClick="this.value = '<?php echo gettext ("Please wait") ?>'; return true;" /><input type="submit" name="cancel" value=<?php echo gettext ("Cancel") ?> /></p>
</form>
<script src="../rte/jquery.rte.js"></script>
<script type="text/javascript">
$(document).ready (function () {
  $('.rte-zone').rte ("css url", "toolbox images url");
  document.form.summary.focus();
});
</script>
