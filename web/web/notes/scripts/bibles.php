<h3><?php echo gettext ("Assign this note to another Bible") ?></h3>
<ul>
<?php foreach ($this->bibles as $bible) { ?>
  <li><a href="bibles.php?id=<?php echo $this->id ?>&bible=<?php echo $bible ?>"><?php echo $bible ?></a></li>
<?php } ?>
</ul> 
<h4><a href="actions.php?id=<?php echo $this->id ?>"><?php echo gettext ("Cancel") ?></a></h4>
