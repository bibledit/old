<h1><?php echo gettext ("Patterns") ?></h1>
<p class="error"><?php echo $this->error ?></p>
<p class="success"><?php echo $this->success ?></p>
<p>
  <?php echo gettext ("Enter the patterns to search for.") ?>
  <?php echo gettext ("One per line.") ?>
</p>
<form action="settingspatterns.php" name="form" method="post">
  <p><textarea name="patterns" class="fullwidth"><?php echo $this->patterns ?></textarea></p>
  <p><input type="submit" name="submit" value=<?php echo gettext ("Save") ?> /></p>
</form>
<br>
<p><?php echo gettext ("It will search the plain text of the Bible, without verse numbers, heading, and other additions.") ?></p>
<p><a href="settings.php"><?php echo gettext ("Go back") ?></a></p>
