<h3><?php echo gettext ("Select the new status for the notes") ?></h3>
<ul>
<?php foreach ($this->statuses as $status) { ?>
  <li><a href="bulk.php?status=<?php echo $status[0] ?>"><?php echo $status[0] ?></a></li>
<?php } ?>
</ul> 
<h4><a href="bulk.php"><?php echo gettext ("Cancel") ?></a></h4>
