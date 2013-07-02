<h2><?php echo $this->header ?></h2>
<p><?php echo $this->info_top ?></p>
<form action="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&<?php echo $this->parameter ?>=" name="form" method="post">
  <p><textarea name="contents"><?php echo $this->text ?></textarea></p>
  <p><input type="submit" name="submit" value=<?php echo gettext ("Save") ?> /></p>
</form>
<p><?php echo $this->info_bottom ?></p>
<h3><a href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>"><?php echo gettext ("Cancel") ?></a></h3>
