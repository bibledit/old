<h1><?php echo gettext ("Changes") ?></h1>
<p>
  <a href="#help">[<?php echo gettext ("help") ?>]</a>
  <span id="count"></span>
  <?php echo gettext ("items") ?>
</p>
<br>
<p class="error"><?php echo $this->error ?></p>
<p class="success"><?php echo $this->success ?></p>
<?php foreach ($this->modifications as $offset => $modification) { ?>
<div id="<?php echo $this->ids[$offset] ?>">
  <p>
    <a href="changes.php?expand=<?php echo $this->ids[$offset] ?>"> ⊞ </a>
    <a href="changes.php?approve=<?php echo $this->ids[$offset] ?>"> ✗ </a>
    <a href="changes.php?goto=<?php echo $this->ids[$offset] ?>" target="_blank"><?php echo $this->passages[$offset] ?></a>
    <?php echo $this->modifications[$offset] ?>
    <a href="change.php?id=<?php echo $this->ids[$offset] ?>"> ⊞ </a>
  </p>
  <p id="<?php echo $this->ids[$offset] ?>">
  </p>
</div>
<?php } ?>
<br>
<a id="help"></a>
<p>
  <?php echo gettext ("This page lists the change notifications.") ?>
</p>
<p>
  <?php echo gettext ("Use the up and down arrow keys to navigate through the entries.") ?>
  <?php echo gettext ("Use the delete key to remove the selected entry.") ?>
</p>
<p>
  <?php echo gettext ("Click ✗ to remove the change.") ?>
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
<script>
<?php echo $this->script; ?>
</script>
<script type="text/javascript" src="changes.js"></script>
