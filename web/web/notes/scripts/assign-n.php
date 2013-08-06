<h3><?php echo gettext ("Select the user to assign the notes to") ?></h3>
<ul>
<?php foreach ($this->users as $user) { ?>
  <li><a href="bulk.php?assign=<?php echo $user ?>"><?php echo $user ?></a></li>
<?php } ?>
</ul> 
<h4><a href="bulk.php"><?php echo gettext ("Cancel") ?></a></h4>
