<body onload="document.form.entry.focus();">
<h2><?php echo $this->question ?></h2>
<form action="<?php echo $this->base_url ?>" name="form" method="post">
  <p><input type="text" name="entry" maxlength="300" value="<?php echo $this->value ?>" /></p>
  <p><input type="submit" name="<?php echo $this->submit ?>" value=<?php echo gettext ("Submit") ?> /></p>
</form>
<h3><a href="<?php echo $this->base_url ?>"><?php echo gettext ("Cancel") ?></a></h3>
<p><?php echo $this->help ?></p>
