<p class="error">{t}There was a problem uploading the file.{/t}</p>
<p><a href="import1.php">{t}Yo can go back to retry it.{/t}</a></p>
<p>{t}Or you can manually specify the location of the file, e.g. /home/joe/notes.tar.gz, and then press Submit.{/t}</p>
<form action="import3.php" name="form" method="GET">
  <p><input type="text" name="file" /></p>
  <p><input type="submit" name="submit" value={t}Submit{/t} /></p>
</form>
