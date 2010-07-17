<h1>{t}Backup{/t}</h1>
<p>{t}This page deals with setting up the backup system.{/t}</p>

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
<p>{t}Once you press the Submit button, a backup will be created. Every day another backup will be created at the same time.{/t}</p
<p>{t}The Logbook shows the backup progress.{/t}</p>
{if $url != ""}
  <p>{t}Once the backup has been created, it can be downloaded from the following location:{/t}</p>
  <p>{$url}</p>
{/if}
