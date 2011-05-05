<h1>{t}Backup{/t}</h1>
<p>{t}Bibledit-Web can create a backup of the database. This would backup all the data that is relevant to this site. It can do that every day.{/t}</p>
<p>{t}To enable this, enter a filename where the backup will be stored, and press Submit.{/t}</p>
<p>{t}To disable it, remove any filename that is there, and press Submit.{/t}</p>
<br>
<p class="error">{$error}</p>
<p class="success">{$success}</p>
<form action="backup.php" name="email" method="post">
  <p><input type="text" name="name" maxlength="50" value="{$name}" /></p>
  <p><input type="submit" name="submit" value={t}Submit{/t} /></p>
</form>
<br>
{if $url != ""}
  <p>{t}Once you press the Submit button, a backup will be created. Every day another backup will be created at the same time.{/t}</p
  <p>{t}The Logbook shows the backup progress.{/t}</p>
  <p>{t}Once the backup has been created, it can be downloaded from the following location:{/t}</p>
  <p>{$url}</p>
{/if}
<h2>{t}Restore{/t}</h2>
<p>{t}Make sure you work with an existing an operational system. If this is not the case, then follow all installation procedures before proceeding to the next steps.{/t}</p>
<br>
<p>{t}Login to the server or open a terminal.{/t}</p>
<br>
<p>{t}Go to a directory on the server. Copy the backup file there.{/t}</p>
<p>{t}Assuming that the backup is called backup.tar.gz, enter this to unpack it:{/t}</p>
<code>gunzip backup.sql.gz</code>
<p>{t}This should give a file called backup.sql.{/t}</p>
<br>
<p>{t}Start the mysql client:{/t}</p>
<code>mysql --host={$database_host} --user={$database_user} --password={$database_pass}</code>
<p>{t}It will welcome you to the MySQL monitor.{/t}</p>
<p>{t}Use the correct table:{/t}</p>
<code>use {$database_name}</code>
<p>{t}Restore the backup:{/t}</p>
<code>source backup.sql</code>
<p>{t}Press enter after the above command, and then let it run till it is through.{/t}</p>
<br>
<p>{t}If there were no projects that use collaboration through Send/Receive, then all is fine. If there were such projects, set these up again.{/t}</p>
