<h3><?php echo gettext ("Select the user to assign the note to") ?></h3>
<ul>
<?php foreach ($this->users as $user) { ?>
  <li><a href="assign.php?id=<?php echo $this->id ?>&assign=<?php echo $user ?>"><?php echo $user ?></a></li>
<?php } ?>
</ul> 
<h4><a href="actions.php?id=<?php echo $this->id ?>"><?php echo gettext ("Cancel") ?></a></h4>
