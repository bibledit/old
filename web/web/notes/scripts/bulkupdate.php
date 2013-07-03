<h1><?php echo gettext ("Bulk Update Consultation Notes") ?></h1>
<p class="error"><?php echo gettext ("This function is powerful. It can update many notes at once, with one click of the mouse. Changes made here are not easily reverted.") ?></p>
<br>
<p><?php echo gettext ("Amount of notes to be updated in one go:") ?> <?php echo $this->notescount ?></p>
<br>
<p><a href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&consultationnotesbulkupdate=subscribe"><?php echo gettext ("Subscribe to the notes") ?></a></p>
<p><a href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&consultationnotesbulkupdate=unsubscribe"><?php echo gettext ("Unsubscribe from the notes") ?></a></p>
<p><a href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&consultationnotesbulkupdate=assign"><?php echo gettext ("Assign the notes to somebody") ?></a></p>
<p><a href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&consultationnotesbulkupdate=unassign"><?php echo gettext ("Unassign the note from somebody") ?></a></p>
<p><a href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&consultationnotesbulkupdate=status"><?php echo gettext ("Change the status of the notes") ?></a></p>
<p><a href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&consultationnotesbulkupdate=severity"><?php echo gettext ("Change the severity of the notes") ?></a></p>
<p><a href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&consultationnotesbulkupdate=visibility"><?php echo gettext ("Change the visibility of the notes") ?></a></p>
<p><a href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&consultationnotesbulkupdate=bible"><?php echo gettext ("Change the Bible of the notes") ?></a></p>
<p><a href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&consultationnotesbulkupdate=delete"><?php echo gettext ("Delete the notes") ?></a></p>
<br>
<p><a href="<?php echo $this->caller ?>?session={$session}&displaynoteslist="><?php echo gettext ("Back to notes list") ?></a></p>
