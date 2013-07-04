<h1><?php echo gettext ("Bibles") ?></h1>
<p class="success"><?php echo $this->success_message ?></p>
<p class="error"><?php echo $this->error_message ?></p>
<p><?php echo gettext ("Available Bibles:") ?></p>
<ul>
<?php foreach ($this->bibles as $offset => $bible) { ?>
  <li><a href="settings.php?bible=<?php echo $this->bibles[$offset] ?>"><?php echo $this->bibles[$offset] ?></a></li>
<?php } ?>
</ul>
<p><a href="manage.php?new="><?php echo gettext ("Create a new empty Bible") ?></a></p>
