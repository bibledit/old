<p class="error"><?php echo gettext ("There was a problem uploading the file.") ?></p>
<p><a href="import1.php"><?php echo gettext ("Yo can go back to retry it.") ?></a></p>
<p><?php echo gettext ("Or you can manually specify the location of the file, e.g. /home/joe/notes.tar.gz, and then press Submit.") ?></p>
<form action="import3.php" name="form" method="GET">
  <p><input type="text" name="file" /></p>
  <p><input type="submit" name="submit" value=<?php echo gettext ("Submit") ?> /></p>
</form>
