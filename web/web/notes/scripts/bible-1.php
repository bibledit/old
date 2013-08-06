<h3><?php echo gettext ("Assign this note to another Bible") ?></h3>
<ul>
<?php foreach ($this->bibles as $offset => $bible) { ?>
  <li><a href="bible-1.php?id=<?php echo $this->id ?>&bible=<?php echo $bible ?>"><?php echo $this->bible_names [$offset] ?></a></li>
<?php } ?>
</ul> 
<h4><a href="actions.php?id=<?php echo $this->id ?>"><?php echo gettext ("Cancel") ?></a></h4>
