<h3><?php echo gettext ("Change Notifications") ?></h3>
<p>
  <a href="#help">[<?php echo gettext ("help") ?>]</a>
  <span id="count"></span>
  <?php echo gettext ("items") ?>
</p>
<br>
<?php foreach ($this->modifications as $offset => $modification) { ?>
  <div id="entry<?php echo $this->ids[$offset] ?>">
    <a href="expand" id="expand<?php echo $this->ids[$offset] ?>"> ⊞ </a>
    <a href="remove" id="remove<?php echo $this->ids[$offset] ?>"> ✗ </a>
    <?php echo $this->passages[$offset] ?>
    |
    <?php echo $this->modifications[$offset] ?>
  </div>
<?php } ?>
<br>
<a id="help"></a>
<p>
  <?php echo gettext ("This page lists the change notifications.") ?>
</p>
<p>
  <?php echo gettext ("Use the up and down arrow keys to navigate through the entries.") ?>
</p>
<p>
  <?php echo gettext ("Press the delete key to remove the selected entry.") ?>
  <?php echo gettext ("Or click the ✗.") ?>
</p>
<p>
  <?php echo gettext ("Use the left and right arrow keys to expand or collapse the selected entry.") ?>
  <?php echo gettext ("Or click the ⊞.") ?>
</p>
<p>
  <?php echo gettext ("Click the passage to open the Text Editor for the chapter.") ?>
</p>
<p>
  <?php echo gettext ("Click the changed text to display the Change Details.") ?>
</p>
<p>
  <?php echo gettext ("Any changes in the text of the Bibles will be added to the list the next day.") ?>
</p>
<p>
  <?php echo gettext ("This works if enabled in your personal notifications.") ?>
</p>
<p>
  <?php echo gettext ("There is a number at the right of each change.") ?>
</p>
<p>
  <?php echo gettext ("It indicates the number of notes you are subscribed to or that have been assigned to you.") ?>
</p>
<p>
  <?php echo gettext ("The change notifications are for you only.") ?>
  <?php echo gettext ("Your colleagues have their own lists.") ?>
</p>


<p>
  <?php echo gettext ("This shows the details of the change in the text of the Bible.") ?>
</p>
<p>
  <?php echo gettext ("It shows the old text, the change, and the new text.") ?>
</p>
<p>
  <?php echo gettext ("Click ✔ to approve a change, and then to go back to the list of changes.") ?>
</p>
<p>
  <?php echo gettext ("It shows all Consultation Notes about the passage.") ?>
</p>
<p>
  <?php echo gettext ("If you are subscribed to any of the notes, you can unsubscribe.") ?>
</p>
<p>
  <?php echo gettext ("If any of the notes is assigned to you, you can unassign it.") ?>
</p>
<p>
  <?php if ($this->level >= 5) { ?>
  <?php echo gettext ("You can delete any of the notes.") ?>
  <?php } ?>
</p>
<p>
  <?php echo gettext ("You can create a new Consultation Notes with a query about the change in the text of the Bible.") ?>
</p>
<p>
  <?php echo gettext ("It displays the date when the modification was processed.") ?>
</p>


<script>
<?php echo $this->script; ?>
</script>
<script type="text/javascript" src="changes.js"></script>
