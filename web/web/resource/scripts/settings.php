<h2><?php echo gettext ("Settings for Resources") ?></h2>
<h3><?php echo gettext ("Active resources") ?></h3>
<?php foreach ($this->actives as $offset => $active) { ?>
  <p><?php echo $active ?></p>
<?php } ?>
<h3><?php echo gettext ("Available internal Bible resources") ?></h3>
<p><?php echo gettext ("Click an item to activate it") ?></p>
<br>
<?php foreach ($this->bibles as $offset => $bible) { ?>
  <p><a href="?add=<?php echo $bible ?>"><?php echo $bible ?></a></p>
<?php } ?>
