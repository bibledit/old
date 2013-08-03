<p><a href="note.php?id=<?php echo $this->id ?>"><?php echo gettext ("Go back to the note contents") ?></a></p>
<p><?php echo gettext ("Summary") ?>: <?php echo $this->summary ?></p>
<p>
<?php if ($this->subscribed) { ?>
<?php echo gettext ("You are subscribed to this note") ?> <a href="?id=<?php echo $this->id ?>&unsubscribe=">[<?php echo gettext ("unsubscribe") ?>]</a>
<?php } else { ?>
<a href="?id=<?php echo $this->id ?>&subscribe="><?php echo gettext ("Subscribe to this note") ?></a>
<?php } ?>
<p><?php echo gettext ("The note has been assigned to") ?>:
<?php foreach ($this->assignees as $offset => $assignee) { ?>
  <?php echo $this->assignees[$offset] ?> 
  <?php if ($this->level >= 5) { ?>
    <a href="?id=<?php echo $this->id ?>&unassign=<?php echo $this->assignees[$offset] ?>">[<?php echo gettext ("unassign") ?>]</a>
    |
  <?php } ?>
<?php } ?>
<?php if ($this->level >= 5) { ?>
  <a href="assign.php?id=<?php echo $this->id ?>">[<?php echo gettext ("add assignee") ?>]</a>
<?php } ?>
</p>
<?php if ($this->assignee) { ?>
  <p>
  <?php echo gettext ("This note has been assigned to you for you to take action on.") ?> 
  <a href="?id=<?php echo $this->id ?>&done="><?php echo gettext ("I have done my part on it.") ?></a>
  </p>
<?php } ?>
<p>
<?php echo gettext ("Status") ?>:
<?php if ($this->level >= 4) { ?>
  <a href="status.php?id=<?php echo $this->id ?>"><?php echo $this->status ?></a>
<?php } else { ?>
  <?php echo $this->status ?>
<?php } ?>
</p>
<p>
  <?php echo gettext ("Verses") ?>: <?php echo $this->verses ?> |
  <a href="verses.php?id=<?php echo $this->id ?>">[<?php echo gettext ("edit") ?>]</a>
</p>
  <p><?php echo gettext ("Severity") ?>: 
  <a href="severity.php?id=<?php echo $this->id ?>"><?php echo $this->severity ?></a></p>
<p>
</p>
<p>
<?php echo gettext ("Bible") ?>:
<?php if ($this->bible != "") { ?>
  <?php echo $this->bible ?>
<?php } else { ?>
  <?php echo gettext ("This is a general note, it does not apply to any specific Bible") ?>
<?php } ?>
  <a href="bibles.php?id=<?php echo $this->id ?>">[<?php echo gettext ("change") ?>]</a>
</p>
<p><?php echo gettext ("Identifier") ?>: <?php echo $this->id ?></p>
</p>
<?php if ($this->level >= 5) { ?>
  <p><a href="?id=<?php echo $this->id ?>&delete="><?php echo gettext ("Delete this note") ?></a></p>
<?php } ?>
