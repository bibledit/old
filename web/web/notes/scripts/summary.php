<h3><?php echo gettext ("Edit note summary") ?></h3>
<form action="summary.php?id=<?php echo $this->id ?>" name="form" method="post">
  <p><input type="text" name="entry" maxlength="300" value="<?php echo $this->summary ?>" /></p>
  <p><input type="submit" name="submit" value=<?php echo gettext ("Save") ?> /></p>
</form>
<h4><a href="note.php?id=<?php echo $this->id ?>"><?php echo gettext ("Cancel") ?></a></h4>


