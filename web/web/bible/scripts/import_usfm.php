<h1><?php echo gettext ("Import USFM") ?></h1>
<p class="success"><?php echo $this->success_message ?></p>
<p class="error"><?php echo $this->error_message ?></p>
<p><?php echo gettext ("Bible") ?>: <a href="settings.php?bible=<?php echo $this->bible ?>"><?php echo $this->bible ?></a></p>
<p>
  <?php echo gettext ("This helps you with importing Unified Standard Format Markers data.") ?>
  <a href="../usfm/usfm.htm"><?php echo gettext ("See the Unified Standard Format Markers User Reference for more information about this format.") ?></a>
</p>
<p>
  <?php echo gettext ("The data to be imported will overwrite similar data already in the Bible.") ?>
  <?php echo gettext ("If you import for example Genesis chapter 1, it will overwrite Genesis 1 if that chapter already was in the Bible.") ?>
  <?php echo gettext ("It will not overwrite anything else.") ?>
  <?php echo gettext ("If Genesis 1 was not yet there, it will be created.") ?>
  <?php echo gettext ("You can import one chapter, or several chapters, or one book, or several books, all in one go.") ?>
  <?php echo gettext ("Bibledit will recognize books through the \\id markup and chapters through the \\c markup.") ?>
  <?php echo gettext ("The text to be imported should be in Unicode UTF-8 format.") ?>
</p>
<br>
<p>
  <?php echo gettext ("To import the data, paste the data into the box below, and then press the Submit button.") ?>
  <a href="import1.php?bible=<?php echo $this->bible ?>"><?php echo gettext ("Or upload a file.") ?></a>
</p>
<form action="import_usfm.php?bible=<?php echo $this->bible ?>" name="form" method="post">
  <p><textarea name="data"></textarea></p>
  <p><input type="submit" name="submit" value=<?php echo gettext ("Submit") ?> /></p>
</form>
