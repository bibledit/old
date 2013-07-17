<p>
<a href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&displaynoteslist="><?php echo gettext ("Back to notes list") ?></a>
|
<a href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&displaynotesactions="><?php echo gettext ("Actions") ?></a>
</p>
<p>
<?php echo gettext ("Summary") ?>: <?php echo $this->summary ?>
<?php if ($this->level >= 5) { ?>
  [<a href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&consultationnoteeditsummary="><?php echo gettext ("edit") ?></a>]
<?php } ?>
</p>
<div><?php echo $this->note_content ?></div>
<hr>
<?php if (isset ($this->note_add_comment)) { ?>
  <form 
    style="border:1px #bbb solid;padding:1em;" 
    action="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&saveconsultationnotecomment=" 
    name="form" 
    method="post"
  >
    <p><?php echo gettext ("Add a comment to this note:") ?></p>
    <p><textarea name="comment" class="rte-zone"></textarea></p>
    <p><input type="submit" name="submit" value=<?php echo gettext ("Save") ?> onClick="this.value = '<?php echo gettext ("Please wait") ?>'; return true;" /><input type="submit" name="cancel" value=<?php echo gettext ("Cancel") ?> /></p>
  </form>
  <script src="../rte/jquery.rte.js"></script>
  <script type="text/javascript">
  $(document).ready (function () {
    $('.rte-zone').rte ("css url", "toolbox images url");
  });
</script>
<?php } else { ?>
  <p><a href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&addtoconsultationnote="><?php echo gettext ("Add a comment to this note") ?></a></p>
<?php } ?>
