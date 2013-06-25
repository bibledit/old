<h1>{t}Patterns{/t}</h1>
<p class="error">{$error}</p>
<p class="success">{$success}</p>
<p>
  {t}Enter the patterns to search for.{/t}
  {t}One per line.{/t}
</p>
<form action="settingspatterns.php" name="form" method="post">
  <p><textarea name="patterns" class="fullwidth">{$patterns}</textarea></p>
  <p><input type="submit" name="submit" value={t}Save{/t} /></p>
</form>
<br>
<p>{t}It will search the plain text of the Bible, without verse numbers, heading, and other additions.{/t}</p>
<p><a href="settings.php">{t}Go back{/t}</a></p>
