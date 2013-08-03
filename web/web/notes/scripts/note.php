<p>
<a href="index.php"><?php echo gettext ("Back to notes list") ?></a>
|
<a href="actions.php?id=<?php echo $this->id ?>"><?php echo gettext ("Actions") ?></a>
</p>
<p>
<?php echo gettext ("Summary") ?>: <?php echo $this->summary ?>
<?php if ($this->level >= 5) { ?>
  [<a href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&consultationnoteeditsummary="><?php echo gettext ("edit") ?></a>]
<?php } ?>
</p>
<div><?php echo $this->content ?></div>
<hr>
<p><a href="comment.php?id=<?php echo $this->id ?>"><?php echo gettext ("Add a comment to this note") ?></a></p>
