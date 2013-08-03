<h3><?php echo gettext ("Select the new status for the note") ?></h3>
<ul>
<?php foreach ($this->statuses as $status) { ?>
  <li><a href="status.php?id=<?php echo $this->id ?>&status=<?php echo $status[0] ?>"><?php echo $status[0] ?></a></li>
<?php } ?>
</ul> 
<h4><a href="actions.php?id=<?php echo $this->id ?>"><?php echo gettext ("Cancel") ?></a></h4>
