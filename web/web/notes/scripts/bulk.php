<h2><?php echo gettext ("Notes Bulk Update") ?></h2>
<p class="error">
  <?php echo gettext ("This function is powerful.") ?>
  <?php echo gettext ("One mouse click can update many notes at once.") ?>
  <?php echo gettext ("Changes made here are not easily reverted.") ?>
</p>
<br>
<p><?php echo gettext ("Amount of notes to be updated in one go:") ?> <?php echo $this->notescount ?></p>
<br>
<p><a href="bulk.php?subscribe="><?php echo gettext ("Subscribe to the notes") ?></a></p>
<p><a href="bulk.php?unsubscribe="><?php echo gettext ("Unsubscribe from the notes") ?></a></p>
<p><a href="assign-n.php"><?php echo gettext ("Assign the notes to somebody") ?></a></p>
<p><a href="unassign-n.php"><?php echo gettext ("Unassign the notes from somebody") ?></a></p>
<p><a href="status-n.php"><?php echo gettext ("Change the status of the notes") ?></a></p>
<p><a href="severity-n.php"><?php echo gettext ("Change the severity of the notes") ?></a></p>
<p><a href="bible-n.php"><?php echo gettext ("Change the Bible of the notes") ?></a></p>
<p><a href="bulk.php?consultationnotesbulkupdate=delete"><?php echo gettext ("Delete the notes") ?></a></p>
<br>
<p><a href="index.php"><?php echo gettext ("Back to notes list") ?></a></p>
