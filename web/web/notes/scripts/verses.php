<h3><?php echo gettext ("Edit the verses?") ?></h3>
<form action="verses.php?id=<?php echo $this->id ?>" name="form" method="post">
  <p><textarea name="verses"><?php echo $this->verses ?></textarea></p>
  <p><input type="submit" name="submit" value=<?php echo gettext ("Save") ?> /></p>
</form>
<h4><a href="actions.php?id=<?php echo $this->id ?>"><?php echo gettext ("Cancel") ?></a></h4>
