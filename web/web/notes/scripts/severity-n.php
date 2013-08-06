<h3><?php echo gettext ("Update the severity of the notes") ?></h3>
<ul>
<?php foreach ($this->severities as $severity) { ?>
  <li><a href="bulk.php?severity=<?php echo $severity[0] ?>"><?php echo $severity[1] ?></a></li>
<?php } ?>
</ul> 
<h4><a href="bulk.php"><?php echo gettext ("Cancel") ?></a></h4>
