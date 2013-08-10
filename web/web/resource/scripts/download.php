<h3><?php echo gettext ("Download resource") ?> "<?php echo $this->name ?>"</h3>
<p><a href="admin.php">[<?php echo gettext ("back") ?>]</a></p>
<br>
<p>
  <?php echo gettext ("Bibledit-Web can display external resources to the user.") ?>
  <?php echo gettext ("It fetches the resources from websites on the Internet.") ?>
  <?php echo gettext ("Some of those websites may be slow to respond.") ?>
  <?php echo gettext ("Bibledit-Web may not always be able to reach them.") ?>
</p>
<p>
  <?php echo gettext ("To address these issues, you can download external resources.") ?>
  <?php echo gettext ("Bibledit-Web then stores the data in its database.") ?>
  <?php echo gettext ("When a downloaded resource is needed, Bibledit-Web can fetch the data from its database rather than from the external website.") ?>
  <?php echo gettext ("This works faster.") ?>
  <?php echo gettext ("It takes the load off the website.") ?>
</p>
<br>
<p><a href="?name=<?php echo $this->name ?>&download"><?php echo gettext ("Start downloading this resource, en view the progress in the logbook.") ?></a></p>
