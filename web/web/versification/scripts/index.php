<h1><?php echo gettext ("Versifications") ?></h1>
<p><?php echo gettext ("This lists the currently available versification systems.") ?> <a href="index.php?new="><?php echo gettext ("You can create a new versification system.") ?></a></p>
<?php foreach ($this->systems as $system) { ?>
  <p><a href="system.php?name=<?php echo $system ?>"><?php echo $system ?></a></p>
<?php } ?>
