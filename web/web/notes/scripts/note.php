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
<p><a href="../notes/comment.php?id=<?php echo $this->id ?>"><?php echo gettext ("Add a comment to this note") ?></a></p>
