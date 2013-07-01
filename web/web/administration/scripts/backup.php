<h1><?php echo gettext ("Backup") ?></h1>
<p><?php echo gettext ("Bibledit-Web can create a backup of the database. This would backup all the relevant data.") ?></p>
<p><?php echo gettext ("To enable this, enter a filename where the backup will be stored, and press Submit.") ?></p>
<p><?php echo gettext ("To disable it, remove any filename that is there, and press Submit.") ?></p>
<br>
<p class="error"><?php echo $this->error ?></p>
<p class="success"><?php echo $this->success ?></p>
<form action="backup.php" name="email" method="post">
  <p><input type="text" name="name" maxlength="50" value="<?php echo $this->name ?>" /></p>
  <p><input type="submit" name="submit" value=<?php echo gettext ("Submit") ?> /></p>
</form>
<br>
<?php if ($this->file != "") { ?>
  <p><?php echo gettext ("Once you press the Submit button, a backup will be created. Every day another backup will be created at midnight.") ?></p
  <p><?php echo gettext ("The Logbook shows the backup progress.") ?></p>
  <p><?php echo gettext ("The backup will be stored at the following location:") ?></p>
  <p><?php echo $this->file ?></p>
<?php } ?>
<h2><?php echo gettext ("Restore") ?></h2>
<p><?php echo gettext ("Make sure you work with an existing an operational system. If this is not the case, then follow all installation procedures before proceeding to the next steps.") ?></p>
<br>
<p><?php echo gettext ("Login to the server or open a terminal.") ?></p>
<br>
<p><?php echo gettext ("Go to a directory on the server. Copy the backup file there.") ?></p>
<p><?php echo gettext ("Assuming that the backup is called backup.tar.gz, enter this to unpack it:") ?></p>
<code>gunzip backup.sql.gz</code>
<p><?php echo gettext ("This should give a file called backup.sql.") ?></p>
<br>
<p><?php echo gettext ("Start the mysql client:") ?></p>
<code>mysql --host=<?php echo $this->database_host ?> --user=<?php echo $this->database_user ?> --password=<?php echo $this->database_pass ?></code>
<p><?php echo gettext ("It will welcome you to the MySQL monitor.") ?></p>
<p><?php echo gettext ("Use the correct table:") ?></p>
<code>use <?php echo $this->database_name ?></code>
<p><?php echo gettext ("Restore the backup:") ?></p>
<code>source backup.sql</code>
<p><?php echo gettext ("Press enter after the above command, and then let it run till it is through.") ?></p>
<br>
<p><?php echo gettext ("If there are projects that use collaboration through Send/Receive, then their parameters will have been restored, but their internal git repositories will not have been restored.") ?></p>
<p><?php echo gettext ("Such project will not operate correctly. For correct operation these project need to be set up again.") ?></p>
