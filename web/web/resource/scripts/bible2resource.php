<h2><?php echo gettext ("Convert Bible to USFM Resource") ?></h2>
<p><?php echo gettext ("Bible") ?>: <?php echo $this->bible ?></p>
<br>
<p>
  <?php echo gettext ("You can convert a Bible to a USFM Resource.") ?>
  <?php echo gettext ("It means that the text of the Bible will be moved to a Resource.") ?>
  <a href="index.php" target="_blank"><?php echo gettext ("The users can display this resource.") ?></a>
</p>
<br>
<p>
  <?php echo gettext ("A Resource will be created with the same name and text as the Bible.") ?>
  <?php echo gettext ("When the conversion is complete, the Bible will be deleted.") ?>
  <?php echo gettext ("A Bible can be edited, but a USFM Resource cannot.") ?>
  <?php echo gettext ("A Bible can be searched, but a USFM Resource cannot.") ?>
  <?php echo gettext ("The USFM Resource uses less space.") ?>
  <?php echo gettext ("Operations on the remaining Bibles will be faster, because the table for Bibles in the database will be smaller.") ?>
</p>
<br>
<p class="error"><?php echo $this->error ?></p>
<p class="success"><?php echo $this->success ?></p>
<p><a href="?bible=<?php echo $this->bible ?>&convert="><?php echo gettext ("Start the conversion process.") ?></a></p>
<p><?php echo gettext ("You can view the conversion process in the logbook.") ?></p>
