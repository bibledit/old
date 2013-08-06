<h3><?php echo gettext ("Update the severity of this note") ?></h3>
<ul>
<?php foreach ($this->severities as $severity) { ?>
  <li><a href="severity-1.php?id=<?php echo $this->id ?>&severity=<?php echo $severity[0] ?>"><?php echo $severity[1] ?></a></li>
<?php } ?>
</ul> 
<h4><a href="actions.php?id=<?php echo $this->id ?>"><?php echo gettext ("Cancel") ?></a></h4>
