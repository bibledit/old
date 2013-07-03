<p><a href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&displaynotecontent="><?php echo gettext ("Go back to the note contents") ?></a></p>
<p><?php echo gettext ("Summary") ?>: <?php echo $this->summary ?></p>
<p>
<?php if ($this->subscribed) { ?>
<?php echo gettext ("You are subscribed to this note") ?> <a href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&consultationnoteunsubscribe=">[<?php echo gettext ("unsubscribe") ?>]</a>
<?php } else { ?>
<a href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&consultationnotesubscribe="><?php echo gettext ("Subscribe to this note") ?></a>
<?php } ?>
<p><?php echo gettext ("The note has been assigned to") ?>:
<?php foreach ($this->assignees as $offset => $assignee) { ?>
  <?php echo $this->assignees[$offset] ?> 
  <?php if ($this->level >= 5) { ?>
    <a href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&consultationnoteremoveassignee=<?php echo $this->assignees[$offset] ?>">[<?php echo gettext ("unassign") ?>]</a>
    |
  <?php } ?>
<?php } ?>
<?php if ($this->level >= 5) { ?>
  <a href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&consultationnoteaddassignee=">[<?php echo gettext ("add assignee") ?>]</a>
<?php } ?>
</p>
<?php if ($this->assignee == true) { ?>
  <p>
  <?php echo gettext ("This note has been specifically assigned to you for taking action on.") ?> 
  <a href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&consultationnoteunassignme="><?php echo gettext ("I have done my part on it.") ?></a>
  </p>
<?php } ?>
<p>
<?php echo gettext ("Status") ?>:
<?php if ($this->level >= 4) { ?>
  <a href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&consultationnotestatus="><?php echo $this->status ?></a>
<?php } else { ?>
  <?php echo $this->status ?>
<?php } ?>
</p>
<p>
<?php echo gettext ("Verses") ?>: <?php echo $this->verses ?> |
<?php if ($this->level >= 3) { ?>
  <a href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&consultationnoteeditverses=">[<?php echo gettext ("edit") ?>]</a>
<?php } else { ?>
  <a href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&consultationnoteaddverse=">[<?php echo gettext ("add") ?>]</a>
<?php } ?>
</p>
<p><?php echo gettext ("Severity") ?>: <a href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&consultationnoteseverity="><?php echo $this->severity ?></a></p>
<p>
</p>
<p><?php echo gettext ("This note is visible to users with the following roles") ?>: <a href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&consultationnoteprivacy="><?php echo $this->privacy ?></a></p>
<p>
<?php echo gettext ("Bible") ?>:
<?php if ($this->consultationnotebible != "") { ?>
  <?php echo $this->consultationnotebible ?>
<?php } else { ?>
  <?php echo gettext ("This is a general note, it does not apply to any specific Bible") ?>
<?php } ?>
<?php if ($this->level >= 5) { ?>
  <a href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&consultationnotechangebible=">[<?php echo gettext ("change") ?>]</a>
<?php } ?>
</p>
<p><?php echo gettext ("Identifier") ?>: <?php echo $this->consultationnote ?></p>
</p>
<?php if ($this->level >= 5) { ?>
  <p><a href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&deleteconsultationnote=<?php echo $this->consultationnote ?>"><?php echo gettext ("Delete this note") ?></a></p>
<?php } ?>
