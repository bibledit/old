<h3><?php echo gettext ("Assign the notes to another Bible") ?></h3>
<ul>
<?php foreach ($this->bibles as $offset => $bible) { ?>
  <li><a href="bulk.php?bible=<?php echo $bible ?>"><?php echo $this->bible_names [$offset] ?></a></li>
<?php } ?>
</ul> 
<h4><a href="bulk.php"><?php echo gettext ("Cancel") ?></a></h4>
